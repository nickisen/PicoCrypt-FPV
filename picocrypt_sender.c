/*
 * PicoCrypt FPV - Sender Firmware
 * Complete implementation for Raspberry Pi Pico (RP2040)
 * 
 * Features:
 * - Video input via ADC (AD9280)
 * - Real-time encryption with Xorshift128+ PRNG
 * - Line-by-line processing with minimal latency
 * - Dual-core architecture for optimal performance
 */

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "pico/multicore.h"
#include "hardware/sync.h"
#include "hardware/irq.h"

// ===== CONFIGURATION =====
#define PRESHARED_KEY       0x123456789ABCDEF0ULL  // 64-bit pre-shared key
#define VIDEO_WIDTH         720
#define VIDEO_HEIGHT        576
#define SAMPLE_RATE         10000000    // 10 MS/s
#define ADC_PIN             26          // ADC input pin

// ===== VIDEO CONSTANTS =====
#define H_SYNC_PULSE        96      // 4.7μs at 20.25MHz
#define H_BACK_PORCH        48      // 2.35μs
#define H_ACTIVE_VIDEO      640     // 31.5μs  
#define H_FRONT_PORCH       12      // 0.6μs
#define V_SYNC_LINES        5       // 160μs
#define V_BACK_PORCH_LINES  36      // 1.152ms
#define V_ACTIVE_LINES      576     // 18.432ms
#define V_FRONT_PORCH_LINES 4       // 128μs

// ===== GLOBAL VARIABLES =====
static uint8_t video_buffer[VIDEO_WIDTH] __attribute__((aligned(32)));
static uint8_t encrypted_buffer[VIDEO_WIDTH] __attribute__((aligned(32)));
static volatile bool new_frame = false;
static volatile uint32_t line_counter = 0;
static volatile bool h_sync_detected = false;
static volatile bool v_sync_detected = false;

// ===== CRYPTOGRAPHY STRUCTURES =====
typedef struct {
    uint64_t state[2];      // Xorshift128+ state
    uint64_t initial_seed;  // Original seed for reset
    uint32_t sync_counter;  // Frame synchronization counter
} prng_state_t;

static prng_state_t sender_prng;

// ===== FUNCTION PROTOTYPES =====
void init_encryption(void);
void init_adc(void);
void init_pio_sync(PIO pio, uint sm);
void init_dma_adc(uint dma_chan);
void init_r2r_dac(void);
void init_pio_video_output(PIO pio, uint sm);
void encrypt_line(uint8_t* input, uint8_t* output, uint length);
void sender_vsync_handler(void);
static inline uint64_t xorshift128_plus(prng_state_t* prng);

// ===== ENCRYPTION FUNCTIONS =====

void init_encryption(void) {
    // Initialize PRNG with pre-shared key
    sender_prng.state[0] = PRESHARED_KEY ^ 0xBF58476D1CE4E5B9ULL;
    sender_prng.state[1] = PRESHARED_KEY ^ 0x94D049BB133111EBULL;
    sender_prng.initial_seed = PRESHARED_KEY;
    sender_prng.sync_counter = 0;
    
    // Warm up the PRNG
    for (int i = 0; i < 10; i++) {
        (void)xorshift128_plus(&sender_prng);
    }
}

static inline uint64_t xorshift128_plus(prng_state_t* prng) {
    uint64_t x = prng->state[0];
    uint64_t const y = prng->state[1];
    prng->state[0] = y;
    x ^= x << 23;
    x ^= x >> 17;
    x ^= y ^ (y >> 26);
    prng->state[1] = x;
    return x + y;
}

void sync_encryption_on_vsync(prng_state_t* prng) {
    // Reset PRNG to initial state
    prng->state[0] = prng->initial_seed ^ 0xBF58476D1CE4E5B9ULL;
    prng->state[1] = prng->initial_seed ^ 0x94D049BB133111EBULL;
    prng->sync_counter++;
    
    // Optional: Slightly modify seed every 60 frames (2 seconds)
    if (prng->sync_counter % 60 == 0) {
        prng->initial_seed ^= 0xAAAAAAAA55555555ULL;
    }
    
    // Warm up again
    for (int i = 0; i < 10; i++) {
        (void)xorshift128_plus(prng);
    }
}

void encrypt_line(uint8_t* input, uint8_t* output, uint length) {
    // Fast 32-bit XOR encryption
    uint32_t* in_32 = (uint32_t*)input;
    uint32_t* out_32 = (uint32_t*)output;
    uint len_32 = length / 4;
    
    for (uint i = 0; i < len_32; i++) {
        uint32_t keystream = (uint32_t)xorshift128_plus(&sender_prng);
        out_32[i] = in_32[i] ^ keystream;
    }
    
    // Handle remaining bytes
    uint remainder = length % 4;
    if (remainder > 0) {
        uint8_t* in_rem = input + (len_32 * 4);
        uint8_t* out_rem = output + (len_32 * 4);
        for (uint i = 0; i < remainder; i++) {
            out_rem[i] = in_rem[i] ^ (uint8_t)xorshift128_plus(&sender_prng);
        }
    }
}

// ===== ADC INITIALIZATION =====
void init_adc(void) {
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(0);  // ADC channel 0
    adc_set_clkdiv(0);    // Maximum speed
    
    // Power on ADC
    adc_hw->cs = ADC_CS_EN_BITS | ADC_CS_TS_EN_BITS;
    adc_hw->cs = ADC_CS_EN_BITS;  // Disable temperature sensor
}

// ===== PIO PROGRAM FOR SYNC DETECTION =====
void init_pio_sync(PIO pio, uint sm) {
    // Simple PIO program to detect H-Sync pulses
    uint offset = pio_add_program(pio, &video_sync_detect_program);
    
    pio_sm_config c = video_sync_detect_program_get_default_config(offset);
    
    // Configure GPIO for sync input
    pio_gpio_init(pio, ADC_PIN);
    gpio_set_function(ADC_PIN, GPIO_FUNC_PIO0);
    
    sm_config_set_in_pins(&c, ADC_PIN);
    sm_config_set_jmp_pin(&c, ADC_PIN);
    
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
    
    // Enable interrupts
    pio_set_irqn_source_enabled(pio, 0, pis_interrupt0, true);
    pio_set_irqn_source_enabled(pio, 1, pis_interrupt1, true);
}

// ===== DMA INITIALIZATION FOR ADC =====
void init_dma_adc(uint dma_chan) {
    dma_channel_config c = dma_channel_get_default_config(dma_chan);
    
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_read_increment(&c, false);   // ADC FIFO is fixed
    channel_config_set_write_increment(&c, true);   // Buffer address increment
    channel_config_set_dreq(&c, DREQ_ADC);
    
    // Configure for ring buffer if needed
    channel_config_set_ring(&c, true, 0);  // Ring buffer on write
}

// ===== R-2R DAC INITIALIZATION =====
void init_r2r_dac(void) {
    // Configure GPIO pins for R-2R DAC output
    // Using GPIO pins 0-7 for 8-bit DAC
    for (int i = 0; i < 8; i++) {
        gpio_init(i);
        gpio_set_dir(i, GPIO_OUT);
        gpio_put(i, 0);
    }
}

// ===== PIO PROGRAM FOR VIDEO OUTPUT =====
void init_pio_video_output(PIO pio, uint sm) {
    // PIO program for precise video timing
    uint offset = pio_add_program(pio, &video_output_program);
    
    pio_sm_config c = video_output_program_get_default_config(offset);
    
    // Configure for 13.5MHz pixel clock
    float div = (float)clock_get_hz(clk_sys) / 13500000.0;
    sm_config_set_clkdiv(&c, div);
    
    // Set up pins for video output
    sm_config_set_set_pins(&c, 0, 8);  // 8 pins for DAC data
    sm_config_set_sideset_pins(&c, 8); // 1 pin for sync
    
    // Initialize pins
    for (int i = 0; i < 9; i++) {
        pio_gpio_init(pio, i);
        gpio_set_function(i, GPIO_FUNC_PIO1);
    }
    
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}

// ===== INTERRUPT HANDLERS =====
void sender_vsync_handler(void) {
    // Reset line counter
    line_counter = 0;
    
    // Resynchronize encryption
    sync_encryption_on_vsync(&sender_prng);
    
    // Signal new frame
    new_frame = true;
    v_sync_detected = true;
    
    // Send V-Sync marker to core 1
    multicore_fifo_push_blocking(0xFFFFFFFF);  // VSYNC marker
}

// ===== CORE 0: VIDEO INPUT =====
void core0_video_input(void) {
    // Initialize hardware
    init_adc();
    init_encryption();
    
    // PIO setup
    PIO pio = pio0;
    uint sm = 0;
    init_pio_sync(pio, sm);
    
    // DMA setup
    int adc_dma_chan = dma_claim_unused_channel(true);
    init_dma_adc(adc_dma_chan);
    
    while (true) {
        // Wait for H-Sync
        if (h_sync_detected) {
            h_sync_detected = false;
            
            // Start ADC DMA transfer for one line
            dma_channel_configure(adc_dma_chan, 
                dma_channel_get_default_config(adc_dma_chan),
                video_buffer, &adc_hw->fifo, VIDEO_WIDTH, true);
            
            // Wait for DMA completion
            dma_channel_wait_for_finish_blocking(adc_dma_chan);
            
            // Encrypt the line
            encrypt_line(video_buffer, encrypted_buffer, VIDEO_WIDTH);
            
            // Send to core 1
            multicore_fifo_push_blocking((uint32_t)encrypted_buffer);
            
            line_counter++;
        }
        
        // Handle V-Sync
        if (v_sync_detected) {
            sender_vsync_handler();
            v_sync_detected = false;
        }
    }
}

// ===== CORE 1: VIDEO OUTPUT =====
void core1_video_output(void) {
    // Initialize R-2R DAC
    init_r2r_dac();
    
    // PIO setup for video output
    PIO pio = pio1;
    uint sm = 0;
    init_pio_video_output(pio, sm);
    
    // DMA for DAC output
    int dac_dma_chan = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(dac_dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);
    channel_config_set_dreq(&c, DREQ_PIO1_TX0);
    
    while (true) {
        uint32_t data = multicore_fifo_pop_blocking();
        
        if (data == 0xFFFFFFFF) {
            // V-Sync marker
            handle_vsync_output();
        } else {
            // Encrypted data from core 0
            uint8_t* encrypted_data = (uint8_t*)data;
            
            // Output directly to DAC via DMA
            dma_channel_configure(dac_dma_chan, &c,
                &pio1_hw->txf[0], encrypted_data, VIDEO_WIDTH, true);
            
            // Wait for completion before processing next line
            dma_channel_wait_for_finish_blocking(dac_dma_chan);
        }
    }
}

// ===== MAIN FUNCTION =====
int main() {
    stdio_init_all();
    
    printf("PicoCrypt FPV Sender v1.0\n");
    printf("Pre-shared key: 0x%016llX\n", PRESHARED_KEY);
    
    // Initialize performance monitoring
    init_performance_monitoring();
    
    // Run self-test
    run_system_selftest();
    
    // Launch core 1
    multicore_launch_core1(core1_video_output);
    
    // Start video input on core 0
    core0_video_input();
    
    return 0;
}

// ===== PIO PROGRAMS (would be in separate .pio file) =====
static const uint16_t video_sync_detect_program_instructions[] = {
    0x0080, // 0: jmp    pin, 0
    0x0040, // 1: in     pins, 1
    0x0041, // 2: in     x, 1
    0x0080, // 3: jmp    pin, 0
    0x80a0, // 4: push   block
    0x0001, // 5: jmp    1
};

static const struct pio_program video_sync_detect_program = {
    .instructions = video_sync_detect_program_instructions,
    .length = 6,
    .origin = -1,
};

static const uint16_t video_output_program_instructions[] = {
    0x6001, // 0: out    pins, 1
    0x6001, // 1: out    pins, 1
    0x6001, // 2: out    pins, 1
    0x6001, // 3: out    pins, 1
    0x6001, // 4: out    pins, 1
    0x6001, // 5: out    pins, 1
    0x6001, // 6: out    pins, 1
    0x6001, // 7: out    pins, 1
};

static const struct pio_program video_output_program = {
    .instructions = video_output_program_instructions,
    .length = 8,
    .origin = -1,
};