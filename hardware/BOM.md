# PicoCrypt FPV - Bauteilliste (BOM)

## Übersicht

Diese Bauteilliste enthält alle notwendigen Komponenten für den Bau eines kompletten PicoCrypt FPV Systems (Sender + Empfänger).

## Kern-Komponenten

### Mikrocontroller
| Menge | Bauteil | Typ | Teilenummer | Hinweise |
|-------|---------|-----|-------------|----------|
| 2 | Raspberry Pi Pico | RP2040 Mikrocontroller | RPI-PICO | Mit 2MB Flash, 264KB RAM |

### Analog-Digital-Wandler (ADC)
| Menge | Bauteil | Typ | Teilenummer | Hinweise |
|-------|---------|-----|-------------|----------|
| 2 | Flash-ADC | 8-bit, 10MS/s | AD9280 | Alternativen: AD9288, ADC08200 |

### Operationsverstärker
| Menge | Bauteil | Typ | Teilenummer | Hinweise |
|-------|---------|-----|-------------|----------|
| 4 | Video-Op-Amp | 1.7GHz Bandbreite | LMH6702 | 2x für Sender, 2x für Empfänger |

## Passive Bauelemente

### Widerstände (0.1% Genauigkeit!)
| Menge | Wert | Typ | Hinweise |
|-------|------|-----|----------|
| 16 | 10kΩ | Metallfilm | Für R-2R Ladder (R-Werte) |
| 16 | 20kΩ | Metallfilm | Für R-2R Ladder (2R-Werte) |
| 8 | 1kΩ | 1% | Für Op-Amp-Schaltungen |
| 8 | 75Ω | 1% | Für Video-Impedanzanpassung |

### Kondensatoren
| Menge | Wert | Typ | Hinweise |
|-------|------|-----|----------|
| 4 | 10μF | Keramik X7R | DC-Kopplung/Entkopplung |
| 8 | 100nF | Keramik | Entkopplung (je IC) |
| 4 | 10pF | Keramik | Kompensation (optional) |

## Verbindungen und Steckverbinder

| Menge | Bauteil | Typ | Hinweise |
|-------|---------|-----|----------|
| 4 | Video-Steckverbinder | RCA/BNC | Für Video-Ein/Ausgang |
| 2 | Stiftleiste 2x20 | 2.54mm | Für Pico-Erweiterung |
| 1 | Jumper-Kabel Set | Diverse | Für Verbindungen |

## Leiterplatte (PCB)

### Option A: Eigenbau
| Menge | Bauteil | Hinweise |
|-------|---------|----------|
| 2 | Lochrasterplatte | 100x160mm oder größer |
| 1 | Kabelset | Für Verbindungen |

### Option B: Professionelle PCB
| Menge | Bauteil | Hinweise |
|-------|---------|----------|
| 2 | PicoCrypt PCB | Siehe docs/PCB_Design.md |

## Werkzeuge und Zubehör

### Notwendige Werkzeuge
- Lötkolben (Feine Spitze, 350°C)
- Lötzinn (0.5mm, bleifrei)
- Seitenschneider
- Kabelstripper
- Multimeter
- Oszilloskop (für Debugging)

### Optional
- Logik-Analysator
- Frequenzzähler
- Signalgenerator

## Ersatzteile und Alternativen

### ADC-Alternativen
- **AD9288**: Dual-Kanal, bis 100MS/s
- **ADC08200**: 8-bit, 200MS/s
- **MAX11905**: 8-bit, 20MS/s

### Op-Amp-Alternativen
- **CLC409**: 1.4GHz, schneller
- **CLC449**: 1.5GHz, niedriger Strom
- **OPA2677**: 1.2GHz, dual

### Kostenübersicht (ca.)

| Komponente | Kosten (EUR) |
|------------|--------------|
| 2x Raspberry Pi Pico | 8,00 |
| 2x AD9280 ADC | 20,00 |
| 4x LMH6702 Op-Amp | 16,00 |
| Passive Bauelemente | 10,00 |
| Steckverbinder/Kabel | 15,00 |
| PCB (Lochraster) | 10,00 |
| **Gesamt** | **79,00** |

## Bezugsquellen

### Deutschland/EU
- **Reichelt Elektronik**: Raspberry Pi, ICs, Passive
- **Conrad Electronic**: Komplette Bauteile
- **Mouser Electronics**: Spezial-ICs
- **Digi-Key**: ADCs, Op-Amps

### International
- **Adafruit**: Raspberry Pi Zubehör
- **SparkFun**: Breakout-Boards
- **LCSC**: Günstige Komponenten

## Lagerung und Handhabung

### ESD-Empfindliche Bauteile
- RP2040 Mikrocontroller
- AD9280 ADC
- LMH6702 Op-Amps

### Temperaturbereiche
- Löten: <400°C, <5 Sekunden
- Betrieb: -20°C bis +85°C
- Lagerung: -40°C bis +125°C

## Qualitätssicherung

### Wichtige Tests
1. **Widerstandswerte messen** (vor dem Löten)
2. **Verbindungen prüfen** (Durchgangstest)
3. **Spannungen messen** (vor IC-Einsetzen)
4. **Signalqualität testen** (mit Oszilloskop)

### Häufige Fehlerquellen
- Falsche Widerstandswerte in R-2R Ladder
- Schlechte Lötverbindungen
- ESD-Schäden an ICs
- Falsche Pin-Zuordnungen

## Dokumentation

- Schaltplan: `docs/Schematic.pdf`
- PCB-Design: `docs/PCB_Design.md`
- Testprozeduren: `docs/Testing.md`
- Troubleshooting: `docs/Troubleshooting.md`

## Versionsverlauf

| Version | Datum | Änderungen |
|---------|-------|------------|
| 1.0 | 2025-10-25 | Erste Version |

## Hinweise

- Alle Widerstände für R-2R Ladder müssen 0.1% Genauigkeit haben!
- ICs vor ESD schützen
- Korrekte Pin-Belegung beachten
- Spezifikationen der Bauteile beachten