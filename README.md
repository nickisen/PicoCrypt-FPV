
# PicoCrypt FPV - Open Source FPV Video Encryption System

## Overview

PicoCrypt FPV is an open-source, real-time video encryption system for FPV applications. It uses two Raspberry Pi Pico microcontrollers to digitize, encrypt, and convert analog video signals back.

### Main Features

- **Real-time Encryption**: XOR-based stream cipher with minimal latency
- **One-Time-Pad Emulation**: Frame-based PRNG resynchronization
- **Dual-Core Architecture**: Optimized performance on RP2040
- **Line-by-Line Processing**: Latency <1ms per line
- **Open Source**: Full source code availability

## System Architecture

```
[Analog Camera] → [ADC] → [RP2040 Transmitter] → [RF Link] → [RP2040 Receiver] → [DAC] → [Display]
```

### Hardware Components

#### Transmitter Module
- Raspberry Pi Pico (RP2040)
- Flash ADC (AD9280, 8-bit, 10MS/s)
- R-2R Resistor Ladder (8-bit DAC)
- Operational Amplifier (LMH6702)

#### Receiver Module
- Raspberry Pi Pico (RP2040)
- Flash ADC (for testing)
- R-2R Resistor Ladder (8-bit DAC)
- Operational Amplifier (LMH6702)

## Installation and Build

### Prerequisites

- Raspberry Pi Pico (2x)
- Raspberry Pi Pico SDK
- CMake 3.13+
- ARM GCC Toolchain
- Flash ADC AD9280 or compatible
- Operational Amplifier LMH6702
- Precision resistors (0.1%) for R-2R Ladder

### Build Instructions

#### Step 1: Prepare Hardware

1.  **Build R-2R DAC**:
    -   8x 10kΩ resistors (R)
    -   8x 20kΩ resistors (2R)
    -   Accuracy: 0.1% or better

2.  **ADC Circuit**:
    -   AD9280 with 3.3V supply
    -   Analog input conditioning with Op-Amp
    -   Anti-aliasing filter (optional)

3.  **Make Connections**:
    ```
    RP2040 GPIO 0-7 → R-2R DAC → Op-Amp → Video Output
    RP2040 GPIO 8 → H-Sync PIO
    RP2040 GPIO 9-16 → ADC Data Lines
    ADC CLK → RP2040 PIO (Timing)
    ```

#### Step 2: Compile Software

```bash
# Prepare Pico SDK
export PICO_SDK_PATH=/path/to/pico-sdk

# Create project folder
mkdir build
cd build

# Configure CMake
cmake .. -DPICO_BOARD=pico

# Compile
make -j4

# Firmware files:
# - picocrypt_sender.uf2
# - picocrypt_receiver.uf2
```

#### Step 3: Installation

1.  Flash Transmitter Firmware:
    ```bash
    # Connect Pico in BOOTSEL mode
    cp picocrypt_sender.uf2 /media/pico/
    ```

2.  Flash Receiver Firmware:
    ```bash
    # Connect second Pico in BOOTSEL mode
    cp picocrypt_receiver.uf2 /media/pico/
    ```

## Configuration

### Key Management

The 64-bit key must be set before compilation:

```c
// In picocrypt_sender.c and picocrypt_receiver.c
#define PRESHARED_KEY 0x123456789ABCDEF0ULL
```

**Important**: The transmitter and receiver must use the exact same key!

### Video Parameters

Configured for PAL video by default:
- Resolution: 720x576
- Frame Rate: 25 fps
- Sampling Rate: 10 MS/s

To adjust for NTSC video:
- VIDEO_WIDTH: 720
- VIDEO_HEIGHT: 480
- Frame Rate: 30 fps

## Usage

### Test Operation

1.  **Direct Connection**:
    -   Transmitter Video Output → Receiver Video Input
    -   Flash both Picos with the same key

2.  **Over Radio Link**:
    -   Transmitter Video Output → Video Transmitter
    -   Receiver Video Receiver → Video Input
    -   Flash both Picos with the same key

### Debugging

Serial output (115200 Baud) provides:
- Performance statistics
- Error messages
- Synchronization status

## Performance

### Latency
- Line Processing: <1ms
- Total Latency: ~2ms (including ADC/DAC)

### Resource Consumption
- CPU Load: <50% (both cores)
- RAM Usage: ~50KB
- Flash: ~100KB

## Cryptographic Security

### Method
- **Algorithm**: XOR stream cipher
- **PRNG**: Xorshift128+ (fast, good quality)
- **Key-Length**: 64-bit
- **Resynchronization**: Frame-based at V-Sync

### Security Features
- Deterministic keystream generation
- No storage of the keystream
- Frame synchronization as an additional security layer
- One-Time-Pad emulation through frame resynchronization

**Note**: This is a hobby project. For professional applications, additional security measures should be implemented.

## Troubleshooting

### Common Problems

1.  **No Image**:
    -   Check connections
    -   Is the key identical on both devices?
    -   Is the ADC/DAC configured correctly?

2.  **Image Artifacts**:
    -   Check timing parameters
    -   Observe synchronization errors
    -   Compare PRNG states

3.  **High Latency**:
    -   Check DMA configuration
    -   Monitor CPU load
    -   Optimize buffer size

### Debugging Tools

- Oscilloscope for video signals
- Logic analyzers for digital signals
- Serial debug output
- Test pattern generation

## Enhancements

### Possible Extensions

1.  **Different Video Standards**:
    -   NTSC support
    -   HD video (720p, 1080p)

2.  **Advanced Cryptography**:
    -   AES encryption
    -   Key exchange protocol

3.  **Additional Features**:
    -   OSD information
    -   Error correction
    -   Compression

## License

This project is licensed under the MIT License.

## Contributors

This project was developed as an open-source solution for the FPV community. Contributions are welcome!

## Disclaimer

This is a hobby project provided without warranty. Use at your own risk. The encryption is designed for hobbyist use and should not be used for security-critical applications.

## Sources and References

- [Raspberry Pi Pico Datasheet](https://www.raspberrypi.org/documentation/rp2040/)
- [AD9280 ADC Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/AD9280.pdf)
- [LMH6702 Op-Amp Datasheet](https://www.ti.com/lit/gpn/LMH6702)
- [Xorshift PRNG Paper](https://www.jstatsoft.org/article/view/v008i14/xorshift.pdf)
