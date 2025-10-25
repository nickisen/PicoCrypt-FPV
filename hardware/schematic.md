# PicoCrypt FPV - Schaltplan

## Übersicht

Dieses Dokument beschreibt den vollständigen Schaltplan für das PicoCrypt FPV System. Es gibt zwei identische Module (Sender und Empfänger) mit geringfügigen Unterschieden in der Signalverarbeitung.

## System-Übersicht

```
┌─────────────────────────────────────────────────────────────────┐
│                    PicoCrypt FPV Sender                         │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  [Video In] → [ADC] → [RP2040] → [DAC] → [Video Out]          │
│      ↓            ↓        ↓         ↓           ↓              │
│    CVBS      Digital   Encrypt   Digital     Encrypted        │
│  (Analog)    (8-bit)   Signal    (8-bit)     Video           │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

## Detaillierter Schaltplan

### 1. Video-Eingangsstufe

```
CVBS-IN (1Vpp, 75Ω)
    │
    ├──[C1 10µF]──┬──[R1 1kΩ]──┬──[U1 LMH6702 +]──┬──[R3 1kΩ]──┐
    │              │            │                  │            │
    │              └──[R2 1kΩ]──┘                  │            │
    │                                              │            │
    └──────────────────────────────────────────────┘            │
                                                               [GND]
                                                                   │
                                                                   v
                                                        [AD9280 Analog IN]
```

**Komponenten:**
- C1: 10µF Keramik-Kondensator (DC-Entkopplung)
- R1, R2: 1kΩ 1% (Eingangsteiler)
- R3: 1kΩ 1% (Bias-Widerstand)
- U1: LMH6702 (Video-Operationsverstärker)

**Berechnungen:**
- Eingangsimpedanz: ~2kΩ
- Verstärkung: 1 (Buffer)
- Bandbreite: >100MHz (für Video)

### 2. ADC-Schaltung (AD9280)

```
RP2040 GPIO 9-16 (8-bit Daten)
        │
        v
┌─────────────────────────────────┐
│            AD9280               │
│                                 │
│  CLK IN ◄── PIO (Timing)        │
│                                 │
│  Analog IN ◄── [Op-Amp Out]     │
│                                 │
│  Digital OUT ◄──┐               │
│                 └─► RP2040 GPIO │
│                                 │
│  REF OUT ──[R4 1kΩ]──┐          │
│                      [C2 100nF] │
│                      │          │
│  REF IN ◄────────────┘          │
│                                 │
│  VDD ◄── [3.3V]                 │
│  GND ◄── [GND]                  │
└─────────────────────────────────┘
```

**Komponenten:**
- AD9280: 8-bit Flash-ADC, 10MS/s
- R4: 1kΩ (Referenz-Last)
- C2: 100nF (Referenz-Filter)

**Anschlüsse:**
- CLK IN: PIO-generiertes Clock-Signal
- Digital OUT: 8 parallele Datenleitungen
- REF OUT/IN: Interne Referenzspannung

### 3. RP2040 Mikrocontroller

```
┌─────────────────────────────────────────────────────────────┐
│                        RP2040                               │
│                                                             │
│  GPIO 0-7  ◄──┐  R-2R DAC (8-bit)                         │
│               │                                            │
│  GPIO 8    ◄──┘  H-Sync PIO                               │
│                                                             │
│  GPIO 9-16 ◄──┐  ADC Daten (8-bit)                        │
│               │                                            │
│  GPIO 17   ◄──┘  ADC Clock (PIO)                          │
│                                                             │
│  GPIO 18-25 ◄──  Optional: Erweiterungen                  │
│                                                             │
│  USB       ◄──  Debugging/Programmierung                  │
│                                                             │
│  VSYS      ◄──  5V Versorgung                            │
│  GND       ◄──  Ground                                   │
└─────────────────────────────────────────────────────────────┘
```

**Wichtige Pins:**
- GPIO 0-7: R-2R DAC Daten (8-bit)
- GPIO 8: Video-Sync Steuerung
- GPIO 9-16: ADC Daten-Eingang
- GPIO 17: ADC Clock-Ausgang

### 4. R-2R DAC-Schaltung

```
RP2040 GPIO 0-7 (8-bit Daten)
        │
        ├──[R 10kΩ]──┬──[2R 20kΩ]──┬──┬──[Op-Amp +]──┐
        │             │             │  │              │
        ├──[R 10kΩ]──┼──[2R 20kΩ]──┼──┤              │
        │             │             │  │              │
        ├──[R 10kΩ]──┼──[2R 20kΩ]──┼──┤              │
        │             │             │  │              │
        ├──[R 10kΩ]──┼──[2R 20kΩ]──┼──┤              │
        │             │             │  │              │
        ├──[R 10kΩ]──┼──[2R 20kΩ]──┼──┤              │
        │             │             │  │              │
        ├──[R 10kΩ]──┼──[2R 20kΩ]──┼──┤              │
        │             │             │  │              │
        ├──[R 10kΩ]──┼──[2R 20kΩ]──┼──┤              │
        │             │             │  │              │
        └──[R 10kΩ]──┼──[2R 20kΩ]──┼──┘              │
                     │             │                 │
                     └─[R5 1kΩ]──┬─[R6 75Ω]──[Video Out]
                                  │                 │
                                  └─[C3 100pF]─────┘
```

**Komponenten:**
- R: 10kΩ 0.1% (16 Stück)
- 2R: 20kΩ 0.1% (16 Stück)
- R5: 1kΩ (Serienwiderstand)
- R6: 75Ω (Impedanzanpassung)
- C3: 100pF (Kompensation)

**Wichtig**: Die Widerstände müssen 0.1% Genauigkeit haben!

### 5. Video-Ausgangsstufe

```
[R-2R DAC Out]──[U2 LMH6702 +]──┬──[R7 75Ω]──[Video Out]
                               │
                               └──[R8 1kΩ]──┐
                                            │
                               [U2 LMH6702 -]─┘
                                            │
                                            v
                                           [GND]
```

**Komponenten:**
- U2: LMH6702 (Video-Op-Amp)
- R7: 75Ω (Video-Impedanz)
- R8: 1kΩ (Rückkopplung)

### 6. Spannungsversorgung

```
USB 5V ──[D1 Schottky]──┬──[U3 3.3V Reg]──┬──[Digital Logic]
                        │                  │
                        │                  └──[ADC VDD]
                        │
                        └──[U4 ±5V Reg]──┬──[Op-Amps ±5V]
                                           │
                                           └──[ADC VREF]
```

**Komponenten:**
- D1: Schottky-Diode (1A)
- U3: 3.3V LDO-Regler (300mA)
- U4: Dual-Spannungsregler ±5V

## Anschluss-Übersicht

### Sender-Modul

| Pin | Funktion | Verbindung |
|-----|----------|------------|
| GPIO 0-7 | DAC Daten | R-2R Ladder |
| GPIO 8 | H-Sync | PIO-Steuerung |
| GPIO 9-16 | ADC Daten | AD9280 D0-D7 |
| GPIO 17 | ADC Clock | AD9280 CLK |
| USB | Debug | PC |

### Empfänger-Modul

| Pin | Funktion | Verbindung |
|-----|----------|------------|
| GPIO 0-7 | DAC Daten | R-2R Ladder |
| GPIO 8 | H-Sync | PIO-Steuerung |
| USB | Debug | PC |

## Signal-Fluss

### Sender
1. **Video-Eingang**: CVBS-Signal (1Vpp)
2. **Konditionierung**: Op-Amp-Puffer, Level-Shifting
3. **Digitalisierung**: AD9280 ADC (8-bit, 10MS/s)
4. **Verschlüsselung**: XOR mit PRNG-Keystream
5. **Ausgabe**: Verschlüsseltes Video-Signal

### Empfänger
1. **Empfang**: Verschlüsseltes Video-Signal
2. **Entschlüsselung**: XOR mit synchronisiertem PRNG
3. **Digital-Analog**: R-2R DAC (8-bit)
4. **Konditionierung**: Op-Amp-Puffer, 75Ω Anpassung
5. **Video-Ausgang**: CVBS-Signal (1Vpp)

## Kritische Design-Aspekte

### 1. Timing-Synchronisation
- H-Sync: 4.7μs ±0.1μs
- V-Sync: 160μs (PAL) / 123μs (NTSC)
- Pixel-Clock: 13.5MHz

### 2. Signalqualität
- ADC-SNR: >40dB
- DAC-Linearität: <1LSB
- Op-Amp-Bandbreite: >100MHz

### 3. Stromversorgung
- Digital: 3.3V ±5%
- Analog: ±5V ±2%
- Stromaufnahme: ~200mA pro Modul

### 4. PCB-Design
- Ground-Plane für minimales Rauschen
- Getrennte Analog-/Digital-Grounds
- Kurze Signalwege für hohe Frequenzen
- Impedanzkontrolle für Video-Signale

## Testpunkte

### Wichtige Messpunkte
1. **TP1**: Video-Eingang (CVBS)
2. **TP2**: ADC-Eingang (0-1V)
3. **TP3**: R-2R DAC-Ausgang
4. **TP4**: Video-Ausgang (CVBS)
5. **TP5**: 3.3V Versorgung
6. **TP6**: Ground

### Test-Signale
- Testbild: Farbbalken-Pattern
- Frequenzgang: Sinus-Sweep
- Linearität: Rampe/Sägezahn
- Synchronisation: V-Sync-Trigger

## Fehlerbehebung

### Häufige Probleme

1. **Kein Bild**:
   - Versorgungsspannungen prüfen
   - Verbindungen kontrollieren
   - ICs korrekt eingesetzt?

2. **Bildstörungen**:
   - Timing-Signale messen
   - R-2R Widerstände prüfen
   - Ground-Verbindungen

3. **Synchronisationsfehler**:
   - V-Sync-Signale beobachten
   - PRNG-Zustände vergleichen
   - Frame-Zähler überprüfen

## Versionsverlauf

| Version | Datum | Änderungen |
|---------|-------|------------|
| 1.0 | 2025-10-25 | Erste Version |

## Anmerkungen

- Alle Widerstände für R-2R Ladder: 0.1% Toleranz!
- ICs vor ESD schützen
- Korrekte Pin-Belegung beachten
- Spezifikationen der Bauteile beachten
- Ground-Plane für beste Performance