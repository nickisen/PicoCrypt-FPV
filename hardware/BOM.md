# PicoCrypt FPV - Bill of Materials (BOM)

## Overview

This bill of materials includes all necessary components for building a complete PicoCrypt FPV system (transmitter + receiver).

## Core Components

### Microcontroller
| Quantity | Component | Type | Part Number | Notes |
|---|---|---|---|---|
| 2 | Raspberry Pi Pico | RP2040 Microcontroller | RPI-PICO | With 2MB Flash, 264KB RAM |

### Analog-to-Digital Converter (ADC)
| Quantity | Component | Type | Part Number | Notes |
|---|---|---|---|---|
| 2 | Flash-ADC | 8-bit, 10MS/s | AD9280 | Alternatives: AD9288, ADC08200 |

### Operational Amplifier
| Quantity | Component | Type | Part Number | Notes |
|---|---|---|---|---|
| 4 | Video Op-Amp | 1.7GHz Bandwidth | LMH6702 | 2x for transmitter, 2x for receiver |

## Passive Components

### Resistors (0.1% accuracy!)
| Quantity | Value | Type | Notes |
|---|---|---|---|
| 16 | 10kΩ | Metal film | For R-2R Ladder (R-values) |
| 16 | 20kΩ | Metal film | For R-2R Ladder (2R-values) |
| 8 | 1kΩ | 1% | For Op-Amp circuits |
| 8 | 75Ω | 1% | For Video impedance matching |

### Capacitors
| Quantity | Value | Type | Notes |
|---|---|---|---|
| 4 | 10μF | Ceramic X7R | DC-Coupling/Decoupling |
| 8 | 100nF | Ceramic | Decoupling (per IC) |
| 4 | 10pF | Ceramic | Compensation (optional) |

## Connections and Connectors

| Quantity | Component | Type | Notes |
|---|---|---|---|
| 4 | Video connector | RCA/BNC | For Video In/Out |
| 2 | Pin header 2x20 | 2.54mm | For Pico expansion |
| 1 | Jumper cable set | Various | For connections |

## Printed Circuit Board (PCB)

### Option A: DIY
| Quantity | Component | Notes |
|---|---|---|
| 2 | Perfboard | 100x160mm or larger |
| 1 | Cable set | For connections |

### Option B: Professional PCB
| Quantity | Component | Notes |
|---|---|---|
| 2 | PicoCrypt PCB | See docs/PCB_Design.md |

## Tools and Accessories

### Required Tools
- Soldering iron (Fine tip, 350°C)
- Solder (0.5mm, lead-free)
- Side cutters
- Wire stripper
- Multimeter
- Oscilloscope (for debugging)

### Optional
- Logic analyzer
- Frequency counter
- Signal generator

## Spare Parts and Alternatives

### ADC Alternatives
- **AD9288**: Dual-channel, up to 100MS/s
- **ADC08200**: 8-bit, 200MS/s
- **MAX11905**: 8-bit, 20MS/s

### Op-Amp Alternatives
- **CLC409**: 1.4GHz, faster
- **CLC449**: 1.5GHz, low current
- **OPA2677**: 1.2GHz, dual

### Cost Overview (approx.)

| Component | Cost (EUR) |
|---|---|
| 2x Raspberry Pi Pico | 8.00 |
| 2x AD9280 ADC | 20.00 |
| 4x LMH6702 Op-Amp | 16.00 |
| Passive components | 10.00 |
| Connectors/Cables | 15.00 |
| PCB (Perfboard) | 10.00 |
| **Total** | **79.00** |

## Sources

### Germany/EU
- **Reichelt Elektronik**: Raspberry Pi, ICs, Passives
- **Conrad Electronic**: Complete components
- **Mouser Electronics**: Specialty ICs
- **Digi-Key**: ADCs, Op-Amps

### International
- **Adafruit**: Raspberry Pi accessories
- **SparkFun**: Breakout boards
- **LCSC**: Inexpensive components

## Storage and Handling

### ESD Sensitive Components
- RP2040 Microcontroller
- AD9280 ADC
- LMH6702 Op-Amps

### Temperature Ranges
- Soldering: <400°C, <5 seconds
- Operation: -20°C to +85°C
- Storage: -40°C to +125°C

## Quality Assurance

### Important Tests
1. **Measure resistor values** (before soldering)
2. **Check connections** (Continuity test)
3. **Measure voltages** (before inserting ICs)
4. **Test signal quality** (with oscilloscope)

### Common Sources of Error
- Incorrect resistor values in R-2R Ladder
- Poor solder joints
- ESD damage to ICs
- Incorrect pin assignments

## Documentation

- Schematic: `docs/Schematic.pdf`
- PCB Design: `docs/PCB_Design.md`
- Test procedures: `docs/Testing.md`
- Troubleshooting: `docs/Troubleshooting.md`

## Version History

| Version | Date | Changes |
|---|---|---|
| 1.0 | 2025-10-25 | First version |

## Notes

- All resistors for R-2R Ladder must have 0.1% accuracy!
- Protect ICs from ESD
- Observe correct pin assignments
- Note component specifications