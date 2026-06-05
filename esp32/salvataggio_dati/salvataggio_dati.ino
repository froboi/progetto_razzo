#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "DHT.h"

// --- Definizione PIN SD Card (Bus SPI) ---
const int PIN_SD_CS   = 5;   // Chip Select
const int PIN_SD_MOSI = 23;  // MOSI
const int PIN_SD_SCK  = 18;  // Clock
const int PIN_SD_MISO = 19;  // MISO

// --- Definizione PIN Sensori ---
#define DHTPIN 27     // Pin dati del DHT11
#define DHTTYPE DHT11 // Tipo di sensore
#define MQPIN 34      // Pin analogico per MQ135

// Inizializzazione sensore DHT
DHT dht(DHTPIN, DHTTYPE);

// Percorso e nome del file
const String path = "/speriamo_che_funziona.json";

void setup() {
  Serial.begin(115200);
  delay(1000); // Pausa per stabilizzare la seriale
  Serial.println("\nAvvio sistema: Sensori + SD Card...");

  // 1. Inizializzazione Sensori
  analogReadResolution(12); // Risoluzione ADC a 12-bit per l'MQ135
  dht.begin();
  
  // 2. Inizializzazione SD Card
  SPI.begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
  
  if (!SD.begin(PIN_SD_CS)) {
    Serial.println("Errore: Montaggio della SD fallito! Controlla i collegamenti.");
    return;
  }
  
  Serial.println("Scheda SD inizializzata correttamente!");

  // 3. Creazione/Apertura del file iniziale
  // Se il file non esiste, lo creiamo
  if (!SD.exists(path)) {
    Serial.println("Il file non esiste. Creazione in corso...");
    // Su ESP32 si usa FILE_WRITE per sovrascrivere/creare
    File file = SD.open(path, FILE_WRITE);
    if (file) {
      file.println("// Inizio registrazione dati");
      file.close();
    }
  } else {
    Serial.println("File già esistente. I nuovi dati verranno aggiunti in coda.");
  }

  // Il DHT11 ha bisogno di assestarsi all'avvio
  delay(2000); 
}

void loop() {
  // --- LETTURA SENSORI ---
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int sensorValue = analogRead(MQPIN); 

  // Controllo errori DHT11
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Errore: Impossibile leggere dal sensore DHT11.");
    delay(2000);
    return; 
  }

  // Controllo errori MQ135
  if (sensorValue <= 0) {
    Serial.println("Errore: Valore anomalo dal sensore MQ135.");
    delay(2000);
    return; 
  }

  // --- CREAZIONE PAYLOAD JSON ---
  // Costruiamo un'unica stringa formattata in JSON con tutti i dati
  String jsonPayload = "{\n";
  jsonPayload += "  \"board\": \"ESP32\",\n";
  jsonPayload += "  \"sensor_DHT11\": {\n";
  jsonPayload += "    \"temperature_C\": " + String(temperature, 1) + ",\n";
  jsonPayload += "    \"humidity_%\": " + String(humidity, 1) + "\n";
  jsonPayload += "  },\n";
  jsonPayload += "  \"sensor_MQ135\": {\n";
  jsonPayload += "    \"rawValue\": " + String(sensorValue) + "\n";
  jsonPayload += "  }\n";
  jsonPayload += "},"; // La virgola finale prepara il campo per il dato successivo

  // Mostriamo il JSON sul Monitor Seriale per debug
  Serial.println("Nuovo dato generato:");
  Serial.println(jsonPayload);

  // --- SALVATAGGIO SU SD CARD ---
  // Usiamo FILE_APPEND per aggiungere i dati alla fine del file senza cancellare i precedenti
  File file = SD.open(path, FILE_APPEND);
  
  if (file) {
    file.println(jsonPayload); // Scrive la stringa JSON nel file
    file.close();              // Chiude il file salvando i dati
    Serial.println("-> Dato salvato correttamente su SD!");
  } else {
    Serial.println("-> ERRORE: Impossibile scrivere sul file " + path);
  }

  Serial.println("-------------------------");

  // Attesa di 2 secondi (limite fisico del DHT11 e tempistica richiesta)
  delay(2000); 
}