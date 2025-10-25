
# PicoCrypt FPV - Schematic

## Overview

This document describes the complete schematic for the PicoCrypt FPV system. There are two identical modules (transmitter and receiver) with minor differences in signal processing.

## System Overview

┌─────────────────────────────────────────────────────────────────┐
│ PicoCrypt FPV Transmitter │
├─────────────────────────────────────────────────────────────────┤
│ │
│ [Video In] → [ADC] → [RP2040] → [DAC] → [Video Out] │
│ ↓ ↓ ↓ ↓ ↓ │
│ CVBS Digital Encrypt Digital Encrypted │
│ (Analog) (8-bit) Signal (8-bit) Video │
│ │
└─────────────────────────────────────────────────────────────────┘

code
Code
download
content_copy
expand_less
## Detailed Schematic

### 1. Video Input Stage

CVBS-IN (1Vpp, 75Ω)
│
├──[C1 10µF]──┬──[R1 1kΩ]──┬──[U1 LMH6702 +]──┬──[R3 1kΩ]──┐
│ │ │ │ │
│ └──[R2 1kΩ]──┘ │ │
│ │ │
└──────────────────────────────────────────────┘ │
[GND]
│
v
[AD9280 Analog IN]

code
Code
download
content_copy
expand_less
**Components:**
- C1: 10µF Ceramic Capacitor (DC decoupling)
- R1, R2: 1kΩ 1% (Input divider)
- R3: 1kΩ 1% (Bias Resistor)
- U1: LMH6702 (Video Operational Amplifier)

**Calculations:**
- Input Impedance: ~2kΩ
- Gain: 1 (Buffer)
- Bandwidth: >100MHz (for Video)

### 2. ADC Circuit (AD9280)

RP2040 GPIO 9-16 (8-bit Data)
│
v
┌─────────────────────────────────┐
│ AD9280 │
│ │
│ CLK IN ◄── PIO (Timing) │
│ │
│ Analog IN ◄── [Op-Amp Out] │
│ │
│ Digital OUT ◄──┐ │
│ └─► RP2040 GPIO │
│ │
│ REF OUT ──[R4 1kΩ]──┐ │
│ [C2 100nF] │
│ │ │
│ REF IN ◄────────────┘ │
│ │
│ VDD ◄── [3.3V] │
│ GND ◄── [GND] │
└─────────────────────────────────┘

code
Code
download
content_copy
expand_less
**Components:**
- AD9280: 8-bit Flash-ADC, 10MS/s
- R4: 1kΩ (Reference Load)
- C2: 100nF (Reference Filter)

**Connections:**
- CLK IN: PIO-generated Clock Signal
- Digital OUT: 8 parallel data lines
- REF OUT/IN: Internal reference voltage

### 3. RP2040 Microcontroller

┌─────────────────────────────────────────────────────────────┐
│ RP2040 │
│ │
│ GPIO 0-7 ◄──┐ R-2R DAC (8-bit) │
│ │ │
│ GPIO 8 ◄──┘ H-Sync PIO │
│ │
│ GPIO 9-16 ◄──┐ ADC Data (8-bit) │
│ │ │
│ GPIO 17 ◄──┘ ADC Clock (PIO) │
│ │
│ GPIO 18-25 ◄── Optional: Expansions │
│ │
│ USB ◄── Debugging/Programming │
│ │
│ VSYS ◄── 5V Supply │
│ GND ◄── Ground │
└─────────────────────────────────────────────────────────────┘

code
Code
download
content_copy
expand_less
**Important Pins:**
- GPIO 0-7: R-2R DAC Data (8-bit)
- GPIO 8: Video-Sync Control
- GPIO 9-16: ADC Data Input
- GPIO 17: ADC Clock Output

### 4. R-2R DAC Circuit

RP2040 GPIO 0-7 (8-bit Data)
│
├──[R 10kΩ]──┬──[2R 20kΩ]──┬──┬──[Op-Amp +]──┐
│ │ │ │ │
├──[R 10kΩ]──┼──[2R 20kΩ]──┼──┤ │
│ │ │ │ │
├──[R 10kΩ]──┼──[2R 20kΩ]──┼──┤ │
│ │ │ │ │
├──[R 10kΩ]──┼──[2R 20kΩ]──┼──┤ │
│ │ │ │ │
├──[R 10kΩ]──┼──[2R 20kΩ]──┼──┤ │
│ │ │ │ │
├──[R 10kΩ]──┼──[2R 20kΩ]──┼──┤ │
│ │ │ │ │
├──[R 10kΩ]──┼──[2R 20kΩ]──┼──┤ │
│ │ │ │ │
└──[R 10kΩ]──┼──[2R 20kΩ]──┼──┘ │
│ │ │
└─[R5 1kΩ]──┬─[R6 75Ω]──[Video Out]
│ │
└─[C3 100pF]─────┘

code
Code
download
content_copy
expand_less
**Components:**
- R: 10kΩ 0.1% (16 pieces)
- 2R: 20kΩ 0.1% (16 pieces)
- R5: 1kΩ (Series Resistor)
- R6: 75Ω (Impedance Matching)
- C3: 100pF (Compensation)

**Important**: The resistors must have 0.1% accuracy!

### 5. Video Output Stage

[R-2R DAC Out]──[U2 LMH6702 +]──┬──[R7 75Ω]──[Video Out]
│
└──[R8 1kΩ]──┐
│
[U2 LMH6702 -]─┘
│
v
[GND]

code
Code
download
content_copy
expand_less
**Components:**
- U2: LMH6702 (Video Op-Amp)
- R7: 75Ω (Video Impedance)
- R8: 1kΩ (Feedback)

### 6. Power Supply

USB 5V ──[D1 Schottky]──┬──[U3 3.3V Reg]──┬──[Digital Logic]
│ │
│ └──[ADC VDD]
│
└──[U4 ±5V Reg]──┬──[Op-Amps ±5V]
│
└──[ADC VREF]

code
Code
download
content_copy
expand_less
**Components:**
- D1: Schottky Diode (1A)
- U3: 3.3V LDO Regulator (300mA)
- U4: Dual Voltage Regulator ±5V

## Connection Overview

### Transmitter Module

| Pin | Function | Connection |
|-----|----------|------------|
| GPIO 0-7 | DAC Data | R-2R Ladder |
| GPIO 8 | H-Sync | PIO Control |
| GPIO 9-16 | ADC Data | AD9280 D0-D7 |
| GPIO 17 | ADC Clock | AD9280 CLK |
| USB | Debug | PC |

### Receiver Module

| Pin | Function | Connection |
|-----|----------|------------|
| GPIO 0-7 | DAC Data | R-2R Ladder |
| GPIO 8 | H-Sync | PIO Control |
| USB | Debug | PC |

## Signal Flow

### Transmitter
1.  **Video Input**: CVBS signal (1Vpp)
2.  **Conditioning**: Op-Amp buffer, Level-Shifting
3.  **Digitization**: AD9280 ADC (8-bit, 10MS/s)
4.  **Encryption**: XOR with PRNG keystream
5.  **Output**: Encrypted video signal

### Receiver
1.  **Reception**: Encrypted video signal
2.  **Decryption**: XOR with synchronized PRNG
3.  **Digital-to-Analog**: R-2R DAC (8-bit)
4.  **Conditioning**: Op-Amp buffer, 75Ω matching
5.  **Video Output**: CVBS signal (1Vpp)

## Critical Design Aspects

### 1. Timing Synchronization
- H-Sync: 4.7μs ±0.1μs
- V-Sync: 160μs (PAL) / 123μs (NTSC)
- Pixel-Clock: 13.5MHz

### 2. Signal Quality
- ADC-SNR: >40dB
- DAC-Linearity: <1LSB
- Op-Amp-Bandwidth: >100MHz

### 3. Power Supply
- Digital: 3.3V ±5%
- Analog: ±5V ±2%
- Current Draw: ~200mA per module

### 4. PCB-Design
- Ground-Plane for minimal noise
- Separate Analog/Digital Grounds
- Short signal paths for high frequencies
- Impedance control for video signals

## Test Points

### Important Measurement Points
1.  **TP1**: Video Input (CVBS)
2.  **TP2**: ADC Input (0-1V)
3.  **TP3**: R-2R DAC Output
4.  **TP4**: Video Output (CVBS)
5.  **TP5**: 3.3V Supply
6.  **TP6**: Ground

### Test Signals
- Test Pattern: Color Bar Pattern
- Frequency Response: Sine Sweep
- Linearity: Ramp/Sawtooth
- Synchronization: V-Sync Trigger

## Troubleshooting

### Common Issues

1.  **No Image**:
    - Check supply voltages
    - Check connections
    - Are ICs inserted correctly?

2.  **Image Distortions**:
    - Measure timing signals
    - Check R-2R resistors
    - Check ground connections

3.  **Synchronization Errors**:
    - Observe V-Sync signals
    - Compare PRNG states
    - Check frame counter

## Version History

| Version | Date | Changes |
|---------|-------|------------|
| 1.0 | 2025-10-25 | First Version |

## Notes

- All resistors for R-2R Ladder: 0.1% tolerance!
- Protect ICs from ESD
- Pay attention to correct pin assignments
- Observe component specifications
- Use a ground plane for best performance