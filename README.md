# 🌍 ESP32 Offline Air Quality Monitor

Un sistema di monitoraggio ambientale autonomo e offline basato su ESP32. Il dispositivo rileva temperatura, umidità e qualità dell'aria (gas/inquinanti) salvando i dati localmente su una scheda SD. Genera inoltre una propria rete Wi-Fi (Access Point) per ospitare una dashboard web analitica, accessibile da qualsiasi smartphone o PC senza necessità di connessione internet.

## ✨ Funzionalità Principali

* **100% Offline:** L'ESP32 opera come Access Point indipendente. Nessun router o server cloud esterno richiesto.
* **Data Logging su SD:** I dati dei sensori vengono scritti ogni 2 secondi in un file JSON strutturato direttamente su una MicroSD.
* **Web Dashboard Integrata:** Un'interfaccia moderna in Dark Mode ospitata direttamente sulla scheda SD.
* **Motore Grafico Nativo:** Grafici storici generati in puro JavaScript tramite HTML5 `<canvas>`, eliminando la dipendenza da librerie esterne (es. Chart.js) che richiederebbero una connessione internet.
* **Analisi Dati in Tempo Reale:** Calcolo dinamico della percentuale di saturazione dell'aria, medie storiche globali (all-time) e valori istantanei.
* **Download Diretto:** Pulsante integrato nella dashboard per scaricare l'intero database JSON per analisi future.

---

## 🛠️ Hardware Necessario

* 1x **ESP32** Development Board
* 1x Sensore di Temperatura e Umidità **DHT11**
* 3x Sensori Gas **MQ135**
* 1x Modulo **MicroSD Card Reader** (Interfaccia SPI)
* 1x MicroSD Card (formattata rigorosamente in **FAT32** con tabella partizioni **MBR**)

### 📌 Schema Collegamenti (Pinout)

| Componente | Pin ESP32 | Note |
| :--- | :--- | :--- |
| **SD Card CS** | `GPIO 5` | |
| **SD Card MOSI** | `GPIO 23` | |
| **SD Card MISO** | `GPIO 19` | |
| **SD Card SCK** | `GPIO 18` | |
| **SD Card VCC** | `VIN / 5V` | Necessario per moduli con regolatore integrato |
| **DHT11 Data** | `GPIO 27` | |
| **MQ135 (1) A0** | `GPIO 34` | Lettura analogica 12-bit (0-4095) |
| **MQ135 (2) A0** | `GPIO 32` | Lettura analogica 12-bit (0-4095) |
| **MQ135 (3) A0** | `GPIO 35` | Lettura analogica 12-bit (0-4095) |

---

## 💻 Installazione e Configurazione

### 1. Preparazione della Scheda SD
La scheda SD deve utilizzare una tabella delle partizioni MBR (MS-DOS) ed essere formattata in FAT32.
Salva il file `index.html` presente in questo repository nella directory principale (root) della scheda SD e inseriscila nel lettore dell'ESP32.

### 2. Dipendenze Software
Il codice è scritto in C++ per l'IDE di Arduino. Assicurati di avere installato il core ESP32 e le seguenti librerie (solitamente preinstallate o reperibili dal Library Manager):
* `FS.h`
* `SD.h`
* `SPI.h`
* `WiFi.h`
* `WebServer.h`
* `DHT.h` (di Adafruit)

### 3. Flash del Firmware
1. Apri il file `.ino` principale.
2. (Opzionale) Modifica le credenziali dell'Access Point alle righe:
   ```cpp
   const char* ssid = "tom^2";
   const char* password = "LA_TUA_PASSWORD"; // Minimo 8 caratteri