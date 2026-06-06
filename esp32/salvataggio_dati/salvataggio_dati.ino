#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "DHT.h"
#include <WiFi.h>
#include <WebServer.h>

// --- Credenziali Hotspot Wi-Fi (Access Point) ---
const char* ssid = "tom^2";
// INSERISCI QUI LA TUA PASSWORD (deve essere di almeno 8 caratteri)
const char* password = ""; //Rimuovere priam di commit

// --- Definizione PIN SD Card (Bus SPI) ---
const int PIN_SD_CS   = 5;   
const int PIN_SD_MOSI = 23;  
const int PIN_SD_SCK  = 18;  
const int PIN_SD_MISO = 19;  

// --- Definizione PIN Sensori ---
#define DHTPIN 27     
#define DHTTYPE DHT11 
#define MQPIN_34 34   // 1° MQ135
#define MQPIN_32 32   // 2° MQ135
#define MQPIN_35 35   // 3° MQ135

// Oggetti e Variabili
DHT dht(DHTPIN, DHTTYPE);
WebServer server(80); // Creazione del server Web sulla porta standard 80
const String path = "/speriamo_che_funziona.json";

// Variabili per il timer non bloccante (sostituisce il delay)
unsigned long tempoPrecedente = 0;
const long intervallo = 2000; // 2000 millisecondi = 2 secondi

void setup() {
  Serial.begin(115200);
  delay(1000); 
  Serial.println("\n--- Avvio sistema ESP32 Offline Server ---");

  // 1. Inizializzazione Rete Wi-Fi (Modalità Access Point)
  Serial.print("Creazione rete Wi-Fi: ");
  Serial.println(ssid);
  WiFi.softAP(ssid, password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Indirizzo IP per accedere alla Dashboard: http://");
  Serial.println(IP);

  // 2. Inizializzazione Sensori
  analogReadResolution(12); 
  dht.begin();
  
  // 3. Inizializzazione SD Card
  SPI.begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
  if (!SD.begin(PIN_SD_CS)) {
    Serial.println("Errore CRITICO: Montaggio SD fallito!");
    return;
  }
  Serial.println("Scheda SD inizializzata.");

  // 4. Creazione file JSON se non esiste
  if (!SD.exists(path)) {
    File file = SD.open(path, FILE_WRITE);
    if (file) {
      file.println("// Inizio registrazione dati");
      file.close();
    }
  }

  // 5. Configurazione delle rotte del Server Web
  
  // Quando il telefono cerca l'IP, inviamo la pagina index.html
  server.on("/", HTTP_GET, []() {
    if (SD.exists("/index.html")) {
      File file = SD.open("/index.html", "r");
      server.streamFile(file, "text/html");
      file.close();
    } else {
      server.send(404, "text/plain", "ERRORE: File index.html non trovato nella SD Card!");
    }
  });

  // Quando la dashboard richiede i dati, inviamo il file JSON
  server.on("/speriamo_che_funziona.json", HTTP_GET, []() {
    if (SD.exists(path)) {
      File file = SD.open(path, "r");
      server.streamFile(file, "application/json");
      file.close();
    } else {
      server.send(404, "text/plain", "ERRORE: File JSON non trovato!");
    }
  });

  // Avvio effettivo del server
  server.begin();
  Serial.println("Server Web avviato correttamente.\n");
  
  delay(2000); // Tempo di assestamento per il DHT11 solo all'avvio
}

void loop() {
  // Questa funzione deve girare continuamente per ascoltare il tuo telefono
  server.handleClient(); 

  // Timer per la lettura dei sensori (Ogni 2 secondi)
  unsigned long tempoAttuale = millis();
  
  if (tempoAttuale - tempoPrecedente >= intervallo) {
    tempoPrecedente = tempoAttuale;

    // --- LETTURA SENSORI ---
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    int valMQ34 = analogRead(MQPIN_34); 
    int valMQ32 = analogRead(MQPIN_32); 
    int valMQ35 = analogRead(MQPIN_35); 

    // Controllo errori
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Attenzione: Impossibile leggere dal DHT11.");
      return; 
    }
    if (valMQ34 <= 0 || valMQ32 <= 0 || valMQ35 <= 0) {
      Serial.println("Attenzione: Uno o più sensori MQ135 restituiscono valori anomali.");
      // Continuiamo comunque per non bloccare tutto il sistema
    }

    // --- CREAZIONE PAYLOAD JSON ---
    String jsonPayload = "{\n";
    jsonPayload += "  \"board\": \"ESP32\",\n";
    jsonPayload += "  \"sensor_DHT11\": {\n";
    jsonPayload += "    \"temperature_C\": " + String(temperature, 1) + ",\n";
    jsonPayload += "    \"humidity_%\": " + String(humidity, 1) + "\n";
    jsonPayload += "  },\n";
    jsonPayload += "  \"sensor_MQ135\": {\n";
    jsonPayload += "    \"MQ_Pin34\": " + String(valMQ34) + ",\n";
    jsonPayload += "    \"MQ_Pin32\": " + String(valMQ32) + ",\n";
    jsonPayload += "    \"MQ_Pin35\": " + String(valMQ35) + "\n";
    jsonPayload += "  }\n";
    jsonPayload += "},"; 

    // Debug sul Monitor Seriale
    Serial.println("Salvataggio dati in corso...");
    Serial.println(jsonPayload);

    // --- SALVATAGGIO SU SD CARD ---
    File file = SD.open(path, FILE_APPEND);
    if (file) {
      file.println(jsonPayload); 
      file.close();              
      Serial.println("-> OK: Dato salvato su SD");
    } else {
      Serial.println("-> ERRORE: Scrittura su SD fallita");
    }
    Serial.println("-------------------------");
  }
}