# 🦈 Bruce DIY Flipper Zero — ESP32-S3 N16R8

ESP32-S3 N16R8 tabanlı, kendin-yap (DIY) Flipper Zero. [Bruce](https://github.com/pr3y/Bruce) firmware'inin bu donanıma uyarlanmış sürümü. Sub-GHz, NFC/RFID, IR, WiFi/BLE saldırıları, NRF24 ve Si5351 sinyal üreteci tek cihazda.

> ⚠️ **Yasal Uyarı:** Bu yazılım yalnızca **yetkili** güvenlik testleri ve eğitim amaçlıdır. AGPL lisansı altında dağıtılır. İzinsiz veya kötü niyetli kullanım yasaktır. Tüm sorumluluk kullanıcıya aittir.

---

## 📦 İndir / Flash

### 🌐 Tarayıcıdan tek tıkla (en kolay)
**[→ Web Flasher: modabucksmain-pixel.github.io/Bruce-DIY-Flipper](https://modabucksmain-pixel.github.io/Bruce-DIY-Flipper/)**
Chrome/Edge (masaüstü) ile aç, cihazı **sol USB-C** ile bağla, **Flash Et**'e bas. Kurulum gerekmez.

### 📥 Manuel
En son derlenmiş bin'ler: **[Releases → v1.0.0](https://github.com/modabucksmain-pixel/Bruce-DIY-Flipper/releases/tag/v1.0.0)**

### Tek dosya (en kolay)
Birleştirilmiş bin — bootloader + partitions + boot_app0 + firmware hepsi içinde, offset `0x0`:

```sh
esptool.py --chip esp32s3 --port COMx write_flash 0x0 Bruce-esp32-s3-N16R8.bin
```

### Ayrı componentlerle
```sh
esptool.py --chip esp32s3 --port COMx write_flash \
  0x0      bootloader.bin \
  0x8000   partitions.bin \
  0xe000   boot_app0.bin \
  0x10000  firmware.bin
```

> Linux/Mac'te `COMx` yerine `/dev/ttyACM0` veya `/dev/ttyUSB0` kullan. Programlama için **sol USB-C** (UART) portunu kullan.

| Dosya | Offset | İşlev |
|---|---|---|
| `Bruce-esp32-s3-N16R8.bin` | `0x0` | Birleştirilmiş (hepsi-bir) |
| `bootloader.bin` | `0x0` | Önyükleyici |
| `partitions.bin` | `0x8000` | Bölüm tablosu |
| `boot_app0.bin` | `0xe000` | OTA seçici |
| `firmware.bin` | `0x10000` | Uygulama |

---

## 🔧 Donanım

### Ana Kart
- **ESP32-S3 N16R8** — 16 MB QSPI Flash, 8 MB OPI PSRAM
- Çift USB-C: **Sol** = UART/programlama, **Sağ** = Native USB (Bad USB)

### Modüller
| Modül | İşlev |
|---|---|
| CC1101 | Sub-GHz 300–928 MHz |
| PN532 | NFC 13.56 MHz + RFID |
| NRF24L01 ×2 | 2.4 GHz, MouseJack, jammer |
| Si5351 | Sinyal üreteci 8 kHz–160 MHz |
| MicroSD | Dosya depolama |
| OLED 1.3" SH1106 (SSD1106G) | Ekran |
| VS1838B | IR alıcı 38 kHz |
| IR LED + 2N2222 | IR verici |
| Pasif Buzzer | Ses |
| 6× Buton | Navigasyon |

---

## 📌 Pin Haritası

### I2C (paylaşımlı) — OLED `0x3C`, PN532 `0x24`, Si5351 `0x60`
```
IO8  = SDA
IO18 = SCL
```

### SPI (paylaşımlı hat) — CC1101 + MicroSD + NRF24 ×2
```
IO11 = MOSI
IO12 = SCK
IO13 = MISO
```

### CS / kontrol pinleri (unique, conflict yok)
```
CC1101    GDO0=IO9   CS=IO10
MicroSD   CS=IO14
NRF24 #1  CE=IO4     CSN=IO5
NRF24 #2  CE=IO6     CSN=IO7
IR        RX=IO1     TX=IO2
Buzzer    IO47
```

### Butonlar (GND'ye bağlı, pull-up)
```
UP=IO15  DOWN=IO16  LEFT=IO38  RIGHT=IO39  OK=IO40  BACK=IO41
```

### ⛔ KULLANMA — kart kitlenir
```
GPIO 26-32  → QSPI Flash
GPIO 33-37  → OPI PSRAM (N16R8'de yasak)
GPIO 19-20  → Native USB D+/D- (Bad USB için ayrılmış)
GPIO 45-46  → Strapping
GPIO 43-44  → UART0 TX/RX (debug)
```

---

## 🧰 Kaynaktan Derleme

[PlatformIO](https://platformio.org/) gerekli.

```sh
# Derle
pio run -e esp32-s3-devkitc-1-psram

# Derle + cihaza yükle
pio run -e esp32-s3-devkitc-1-psram -t upload

# Seri monitör
pio device monitor
```

Çıktı: `.pio/build/esp32-s3-devkitc-1-psram/firmware.bin`

Aktif board `platformio.ini` içindeki `default_envs` ile belirlenir (`esp32-s3-devkitc-1-psram`).

---

## ✨ Özellikler

| Kategori | İçerik |
|---|---|
| **WiFi** | AP/scan, Beacon spam, Deauth, Evil Portal, Wardriving, Sniffer, ARP spoof/poison, Responder |
| **BLE** | Scan, Bad BLE (Ducky), iOS/Windows/Samsung/Android spam |
| **RF (CC1101)** | Scan/Copy, Custom SubGhz, Spectrum, Replay, Jammer |
| **RFID/NFC (PN532)** | Read/Clone/Write, NDEF, Chameleon, Amiibolink |
| **IR** | TV-B-Gone, Receiver, Custom IR (NEC, SIRC, RC5/6, Samsung32…) |
| **NRF24** | Jammer, 2.4G Spectrum, Mousejack |
| **Si5351** | Frekans üretimi 8 kHz–160 MHz (CLK0/1/2), sweep |
| **Bad USB** | Ducky script (sağ USB-C / Native USB) |
| **Diğer** | JS interpreter, WebUI, SD/LittleFS yöneticisi, QR, iButton |

---

## 🙏 Teşekkür

Bu proje [**Bruce**](https://github.com/pr3y/Bruce) firmware'ine dayanır. Orijinal geliştiriciler [@pr3y](https://github.com/pr3y), [@bmorcelli](https://github.com/bmorcelli) ve tüm Bruce katkıcılarına teşekkürler.

- Resmi proje: https://bruce.computer
- Wiki: https://wiki.bruce.computer
- Discord: https://discord.gg/WJ9XF9czVT

## 📄 Lisans

AGPL — bkz. [LICENSE](./LICENSE).
