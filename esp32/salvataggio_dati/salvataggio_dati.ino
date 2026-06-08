#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "DHT.h"
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Configurazione Display OLED GME12864 (I2C) ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Frame Animazione Razzo ---
const unsigned char PROGMEM rocket_frame1[] = {
  0x00, 0x03, 0xc0, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x0f, 0xf0, 0x00,
  0x00, 0x1f, 0xf8, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x3f, 0xfc, 0x00,
  0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00,
  0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00,
  0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00,
  0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00,
  0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00,
  0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00,
  0x00, 0x7f, 0xfe, 0x00, 0x00, 0xff, 0xff, 0x00, 0x01, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0xc0,
  0x03, 0xff, 0xff, 0xc0, 0x07, 0xff, 0xff, 0xe0, 0x07, 0xf3, 0xcf, 0xe0, 0x0f, 0xe3, 0xc7, 0xf0,
  0x0f, 0xc3, 0xc3, 0xf0, 0x1f, 0x83, 0xc1, 0xf8, 0x1f, 0x03, 0xc0, 0xf8, 0x1e, 0x03, 0xc0, 0x78,
  0x1c, 0x03, 0xc0, 0x38, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x01, 0x80, 0x00,
  0x00, 0x01, 0x80, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x06, 0x60, 0x00,
  0x00, 0x04, 0x20, 0x00, 0x00, 0x0c, 0x30, 0x00, 0x00, 0x08, 0x10, 0x00, 0x00, 0x08, 0x10, 0x00,
  0x00, 0x18, 0x18, 0x00, 0x00, 0x10, 0x08, 0x00, 0x00, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char PROGMEM rocket_frame2[] = {
  0x00, 0x03, 0xc0, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x0f, 0xf0, 0x00,
  0x00, 0x1f, 0xf8, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x3f, 0xfc, 0x00,
  0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00,
  0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00,
  0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00,
  0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00,
  0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00,
  0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00,
  0x00, 0x7f, 0xfe, 0x00, 0x00, 0xff, 0xff, 0x00, 0x01, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0xc0,
  0x03, 0xff, 0xff, 0xc0, 0x07, 0xff, 0xff, 0xe0, 0x07, 0xf3, 0xcf, 0xe0, 0x0f, 0xe3, 0xc7, 0xf0,
  0x0f, 0xc3, 0xc3, 0xf0, 0x1f, 0x83, 0xc1, 0xf8, 0x1f, 0x03, 0xc0, 0xf8, 0x1e, 0x03, 0xc0, 0x78,
  0x1c, 0x03, 0xc0, 0x38, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x02, 0x40, 0x00,
  0x00, 0x06, 0x60, 0x00, 0x00, 0x04, 0x20, 0x00, 0x00, 0x0c, 0x30, 0x00, 0x00, 0x1c, 0x38, 0x00,
  0x00, 0x18, 0x18, 0x00, 0x00, 0x30, 0x0c, 0x00, 0x00, 0x20, 0x04, 0x00, 0x00, 0x60, 0x06, 0x00,
  0x00, 0x40, 0x02, 0x00, 0x00, 0xc0, 0x03, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x80, 0x01, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// --- Configurazione Hardware ---
const char* ssid = "tom^2";
const char* password = "@PlusGiocando26"; 
String ipString = "0.0.0.0"; 

const int PIN_SD_CS   = 5;   
#define DHTPIN 27     
#define DHTTYPE DHT11 
#define MQPIN_34 34   
#define MQPIN_32 32   
#define MQPIN_35 35   

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80); 
const String path = "/speriamo_che_funziona.json";

// --- IL VIGILE URBANO: Il Mutex per proteggere la SD Card ---
SemaphoreHandle_t sdMutex;


// =====================================================================
// TASK 1: Gestione Schermo OLED (CORE 0)
// Questo task gira all'infinito e si occupa SOLO di disegnare a schermo
// =====================================================================
void TaskDisplay(void *pvParameters) {
  bool mostraRazzo = false;
  bool flagFrame = false;
  unsigned long tempoUltimoCambioPagina = millis();

  for (;;) {
    unsigned long tempoAttuale = millis();

    // Cambio pagina ogni 4 secondi
    if (tempoAttuale - tempoUltimoCambioPagina >= 4000) {
      mostraRazzo = !mostraRazzo;
      tempoUltimoCambioPagina = tempoAttuale;
      display.clearDisplay();
      
      if (!mostraRazzo) {
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);   display.println("----- RETE WI-FI -----");
        display.setCursor(0, 16);  display.print("SSID: "); display.println(ssid);
        display.setCursor(0, 28);  display.print("PSW : "); display.println(password);
        display.setCursor(0, 44);  display.println("DASHBOARD URL:");
        display.print("http://");  display.println(ipString);
        display.display();
      }
    }

    // Animazione Razzo (Aggiorna solo se siamo sulla pagina del razzo)
    if (mostraRazzo) {
      flagFrame = !flagFrame;
      display.clearDisplay();
      if (flagFrame) {
        display.drawBitmap(48, 0, rocket_frame2, 32, 64, SSD1306_WHITE);
      } else {
        display.drawBitmap(48, 0, rocket_frame1, 32, 64, SSD1306_WHITE);
      }
      display.display();
      vTaskDelay(pdMS_TO_TICKS(150)); // Attende 150ms per il frame successivo
    } else {
      vTaskDelay(pdMS_TO_TICKS(100)); // Attesa passiva mentre mostra il testo
    }
  }
}

// =====================================================================
// TASK 2: Gestione Web Server (CORE 0)
// Risponde in tempo reale alle richieste HTTP del telefono
// =====================================================================
void TaskWebServer(void *pvParameters) {
  for (;;) {
    server.handleClient();
    vTaskDelay(pdMS_TO_TICKS(10)); // Fondamentale per far "respirare" il Core 0
  }
}

// =====================================================================
// TASK 3: Lettura Sensori e Scrittura SD (CORE 1)
// Questo è il "Magazziniere". Lavora isolato dagli altri processi.
// =====================================================================
void TaskSensoriSD(void *pvParameters) {
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(2000)); // Attende 2 secondi tra una lettura e l'altra

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    int valMQ34 = analogRead(MQPIN_34); 
    int valMQ32 = analogRead(MQPIN_32); 
    int valMQ35 = analogRead(MQPIN_35); 

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("[Sensori] Attesa DHT11...");
      continue; // Salta il ciclo e riprova
    }

    String jsonPayload = "{\n  \"board\": \"ESP32\",\n  \"sensor_DHT11\": {\n    \"temperature_C\": " + String(temperature, 1) + ",\n    \"humidity_%\": " + String(humidity, 1) + "\n  },\n  \"sensor_MQ135\": {\n    \"MQ_Pin34\": " + String(valMQ34) + ",\n    \"MQ_Pin32\": " + String(valMQ32) + ",\n    \"MQ_Pin35\": " + String(valMQ35) + "\n  }\n},"; 

    // CHIEDE IL PERMESSO AL SEMAFORO HARDWARE (Aspetta massimo 1,5 secondi)
    if (xSemaphoreTake(sdMutex, pdMS_TO_TICKS(1500)) == pdTRUE) {
      Serial.println("[SD Task] Salvo i dati...");
      File file = SD.open(path, FILE_APPEND);
      
      if (file) {
        file.println(jsonPayload); 
        file.flush();
        file.close();              
        Serial.println("[SD Task] -> Dati salvati con successo.");
      } else {
        Serial.println("[SD Task] -> ERRORE CRITICO FATFS. Attivo defibrillatore...");
        SD.end();
        vTaskDelay(pdMS_TO_TICKS(200));
        SD.begin(PIN_SD_CS);
      }
      
      xSemaphoreGive(sdMutex); // RILASCIA IL SEMAFORO FONDAMENTALE
    } else {
      Serial.println("[SD Task] SD bloccata dal WebServer. Salto salvataggio per non creare conflitti.");
    }
  }
}

// =====================================================================
// SETUP: Inizializza tutto e lancia i Task sui vari Core
// =====================================================================
void setup() {
  Serial.begin(115200);
  delay(1000); 
  Serial.println("\n--- Avvio sistema ESP32 FreeRTOS ---");

  // Creazione del Mutex
  sdMutex = xSemaphoreCreateMutex();

  // Inizializzazione Display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("Errore CRITICO: Schermo fallito!");
  } else {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0); display.println("Avvio RTOS...");
    display.display();
  }

  // Rete Wi-Fi
  WiFi.softAP(ssid, password);
  ipString = WiFi.softAPIP().toString(); 
  Serial.print("Indirizzo IP: "); Serial.println(ipString);

  // Sensori
  analogReadResolution(12); 
  dht.begin();
  
  // SD Card
  SPI.begin(18, 19, 23, 5); // SCK, MISO, MOSI, CS
  
  // Passiamo esplicitamente l'oggetto SPI e abbassiamo la frequenza a 4 MHz per stabilità
  if (!SD.begin(PIN_SD_CS, SPI, 4000000)) {
    Serial.println("Errore CRITICO: Montaggio SD fallito!");
    
    // Mostra l'errore anche sullo schermo OLED per comodità
    if(display.getCursorY() >= 0) {
      display.clearDisplay();
      display.setCursor(0,0);
      display.println("SD: ERRORE!");
      display.display();
    }
  } else {
    Serial.println("Scheda SD inizializzata con successo.");
    if (!SD.exists(path)) {
      File f = SD.open(path, FILE_WRITE);
      if (f) { 
        f.println("// Inizio registrazione dati"); 
        f.close(); 
      }
    }
  }

  // Rotte Web Server (Protetto dal Mutex)
  server.on("/", HTTP_GET, []() {
    if (xSemaphoreTake(sdMutex, pdMS_TO_TICKS(1500)) == pdTRUE) {
      if (SD.exists("/index.html")) {
        File file = SD.open("/index.html", "r");
        server.streamFile(file, "text/html");
        file.close();
      } else {
        server.send(404, "text/plain", "ERRORE: index.html non trovato sulla SD!");
      }
      xSemaphoreGive(sdMutex);
    } else {
      server.send(503, "text/plain", "SD occupata (Lettura index), riprova.");
    }
  });

  server.on("/speriamo_che_funziona.json", HTTP_GET, []() {
    if (xSemaphoreTake(sdMutex, pdMS_TO_TICKS(1500)) == pdTRUE) {
      if (SD.exists(path)) {
        File file = SD.open(path, "r");
        server.streamFile(file, "application/json");
        file.close();
      } else {
        server.send(404, "text/plain", "ERRORE: JSON non trovato!");
      }
      xSemaphoreGive(sdMutex);
    } else {
      server.send(503, "text/plain", "SD occupata (Scrittura sensori in corso), riprova tra 1 sec.");
    }
  });

  server.begin();
  Serial.println("Server Web avviato.");

  // Lancia i Task di FreeRTOS assegnandoli ai Core specifici
  xTaskCreatePinnedToCore(TaskDisplay,   "DisplayTask", 4096, NULL, 1, NULL, 0); // Core 0
  xTaskCreatePinnedToCore(TaskWebServer, "WebTask",     4096, NULL, 1, NULL, 0); // Core 0
  xTaskCreatePinnedToCore(TaskSensoriSD, "SensorTask",  4096, NULL, 1, NULL, 1); // Core 1

  Serial.println("Task Multi-Core assegnati con successo. Sistema Operativo attivo.");
}

// =====================================================================
// LOOP: Vuoto. Tutto il lavoro è gestito da FreeRTOS in background.
// =====================================================================
void loop() {
  vTaskDelete(NULL); // Uccide questo loop inutile per liberare RAM. I Task continueranno a girare per sempre.
}