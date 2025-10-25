# PicoCrypt FPV - Quelloffenes FPV-Videoverschlüsselungssystem

## Übersicht

PicoCrypt FPV ist ein quelloffenes Echtzeit-Videoverschlüsselungssystem für FPV-Anwendungen. Es verwendet zwei Raspberry Pi Pico-Mikrocontroller, um analoge Videosignale zu digitalisieren, zu verschlüsseln und wieder zurückzuwandeln.

### Hauptmerkmale

- **Echtzeit-Verschlüsselung**: XOR-basierte Stromchiffre mit minimaler Latenz
- **One-Time-Pad-Emulation**: Frame-basierte PRNG-Resynchronisation
- **Dual-Core-Architektur**: Optimierte Performance auf RP2040
- **Line-by-Line-Verarbeitung**: Latenz <1ms pro Zeile
- **Open Source**: Vollständige Quellcode-Verfügbarkeit

## System-Architektur

```
[Analog Camera] → [ADC] → [RP2040 Sender] → [RF Link] → [RP2040 Receiver] → [DAC] → [Display]
```

### Hardware-Komponenten

#### Sender-Modul
- Raspberry Pi Pico (RP2040)
- Flash-ADC (AD9280, 8-bit, 10MS/s)
- R-2R Widerstandsladder (8-bit DAC)
- Operationsverstärker (LMH6702)

#### Empfänger-Modul
- Raspberry Pi Pico (RP2040)
- Flash-ADC (für Tests)
- R-2R Widerstandsladder (8-bit DAC)
- Operationsverstärker (LMH6702)

## Installation und Bau

### Voraussetzungen

- Raspberry Pi Pico (2x)
- Raspberry Pi Pico SDK
- CMake 3.13+
- ARM GCC Toolchain
- Flash-ADC AD9280 oder kompatibel
- Operationsverstärker LMH6702
- Präzise Widerstände (0.1%) für R-2R Ladder

### Bauanleitung

#### Schritt 1: Hardware vorbereiten

1. **R-2R DAC bauen**:
   - 8x Widerstände 10kΩ (R)
   - 8x Widerstände 20kΩ (2R)
   - Genauigkeit: 0.1% oder besser

2. **ADC-Schaltung**:
   - AD9280 mit 3.3V Versorgung
   - Analoge Eingangskonditionierung mit Op-Amp
   - Anti-Aliasing-Filter (optional)

3. **Verbindungen herstellen**:
   ```
   RP2040 GPIO 0-7 → R-2R DAC → Op-Amp → Video-Ausgang
   RP2040 GPIO 8 → H-Sync PIO
   RP2040 GPIO 9-16 → ADC Datenleitungen
   ADC CLK → RP2040 PIO (Timing)
   ```

#### Schritt 2: Software kompilieren

```bash
# Pico SDK vorbereiten
export PICO_SDK_PATH=/path/to/pico-sdk

# Projektordner erstellen
mkdir build
cd build

# CMake konfigurieren
cmake .. -DPICO_BOARD=pico

# Kompilieren
make -j4

# Firmware-Dateien:
# - picocrypt_sender.uf2
# - picocrypt_receiver.uf2
```

#### Schritt 3: Installation

1. Sender-Firmware flashen:
   ```bash
   # Pico im BOOTSEL-Modus anschließen
   cp picocrypt_sender.uf2 /media/pico/
   ```

2. Empfänger-Firmware flashen:
   ```bash
   # Zweiten Pico im BOOTSEL-Modus anschließen
   cp picocrypt_receiver.uf2 /media/pico/
   ```

## Konfiguration

### Schlüssel-Management

Der 64-bit Schlüssel muss vor der Kompilierung festgelegt werden:

```c
// In picocrypt_sender.c und picocrypt_receiver.c
#define PRESHARED_KEY 0x123456789ABCDEF0ULL
```

**Wichtig**: Sender und Empfänger müssen exakt denselben Schlüssel verwenden!

### Video-Parameter

Standardmäßig konfiguriert für PAL-Video:
- Auflösung: 720x576
- Frame-Rate: 25 fps
- Sampling-Rate: 10 MS/s

Für NTSC-Video Parameter anpassen:
- VIDEO_WIDTH: 720
- VIDEO_HEIGHT: 480
- Frame-Rate: 30 fps

## Verwendung

### Testbetrieb

1. **Direkte Verbindung**:
   - Sender-Video-Ausgang → Empfänger-Video-Eingang
   - Beide Picos mit gleichem Schlüssel flashen

2. **Über Funk**:
   - Sender-Video-Ausgang → Video-Sender
   - Empfänger-Video-Empfänger → Video-Eingang
   - Beide Picos mit gleichem Schlüssel flashen

### Debugging

Serielle Ausgabe (115200 Baud) liefert:
- Performance-Statistiken
- Fehler-Meldungen
- Synchronisations-Status

## Performance

### Latenz
- Zeilen-Verarbeitung: <1ms
- Gesamtlatenz: ~2ms (einschließlich ADC/DAC)

### Ressourcenverbrauch
- CPU-Auslastung: <50% (beide Cores)
- RAM-Verbrauch: ~50KB
- Flash: ~100KB

## Kryptographische Sicherheit

### Verfahren
- **Algorithmus**: XOR-Stromchiffre
- **PRNG**: Xorshift128+ (schnell, gute Qualität)
- **Key-Length**: 64-bit
- **Resynchronisation**: Frame-basiert bei V-Sync

### Sicherheitsmerkmale
- Deterministische Keystream-Generierung
- Keine Speicherung des Keystreams
- Frame-Synchronisation als zusätzliche Sicherheitsebene
- One-Time-Pad-Emulation durch Frame-Resynchronisation

**Hinweis**: Dies ist ein Hobby-Projekt. Für professionelle Anwendungen sollten zusätzliche Sicherheitsmaßnahmen implementiert werden.

## Fehlerbehebung

### Häufige Probleme

1. **Kein Bild**:
   - Verbindungen prüfen
   - Schlüssel auf beiden Geräten identisch?
   - ADC/DAC korrekt konfiguriert?

2. **Bildstörungen**:
   - Timing-Parameter prüfen
   - Synchronisations-Fehler beobachten
   - PRNG-Zustände vergleichen

3. **Hohe Latenz**:
   - DMA-Konfiguration prüfen
   - CPU-Auslastung überwachen
   - Buffer-Größe optimieren

### Debugging-Tools

- Oszilloskop für Video-Signale
- Logische Analysatoren für Digital-Signale
- Serielle Debug-Ausgabe
- Test-Pattern-Generierung

## Erweiterungen

### Mögliche Erweiterungen

1. **Verschiedene Video-Standards**:
   - NTSC-Unterstützung
   - HD-Video (720p, 1080p)

2. **Erweiterte Kryptographie**:
   - AES-Verschlüsselung
   - Schlüsselaustausch-Protokoll

3. **Zusätzliche Features**:
   - OSD-Informationen
   - Fehlerkorrektur
   - Komprimierung

## Lizenz

Dieses Projekt ist unter der MIT-Lizenz lizenziert. Siehe LICENSE-Datei für Details.

## Beitragende

Dieses Projekt wurde entwickelt als quelloffene Lösung für die FPV-Community. Beiträge sind willkommen!

## Haftungsausschluss

Dies ist ein Hobby-Projekt ohne Gewährleistung. Die Verwendung erfolgt auf eigene Gefahr. Die Verschlüsselung ist für den Hobby-Gebrauch konzipiert und sollte nicht für sicherheitskritische Anwendungen verwendet werden.

## Quellen und Referenzen

- [Raspberry Pi Pico Datasheet](https://www.raspberrypi.org/documentation/rp2040/)
- [AD9280 ADC Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/AD9280.pdf)
- [LMH6702 Op-Amp Datasheet](https://www.ti.com/lit/gpn/LMH6702)
- [Xorshift PRNG Paper](https://www.jstatsoft.org/article/view/v008i14/xorshift.pdf)