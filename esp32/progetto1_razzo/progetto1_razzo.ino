#include "DHT.h"//Definizione PIN
#define DHTPIN 27     // Pin dati del DHT11
#define DHTTYPE DHT11 // Stiamo usando il DHT11
#define mqPin 34      // Pin analogico per MQ135

int sensorValue = 0;

// Inizializza il sensore DHT
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println("Avvio sensori DHT11 e MQ135 su ESP32...");

  // Opzionale ma consigliato: assicuriamoci che l'ADC sia impostato a 12-bit
  analogReadResolution(12);
  
  dht.begin();
  
  // Il DHT11 è un sensore lento, ha bisogno di assestarsi all'avvio
  delay(2000); 
}

void loop() {
  // Lettura dell'umidità (ci vogliono circa 250 millisecondi)
  float humidity = dht.readHumidity();
  // Lettura della temperatura in Celsius
  float temperature = dht.readTemperature();

  sensorValue = analogRead(mqPin); 

  // Controlla se le letture sono andate a buon fine (isnan = Is Not a Number)
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("{\"error\": \"Impossibile leggere dal sensore DHT11\"}");
    delay(2000);
    return; // Ricomincia il loop se il sensore principale fallisce
  }

  // 2. Controllo errori MQ135 (valore anomalo)
  if (sensorValue <= 0) {
    Serial.println("{\"error\": \"Valore anomalo dal sensore MQ135\"}");
    delay(2000);
    return; // Ricomincia il loop
  }

  // costruzione payload JSON
  Serial.println("{\"board\": \"ESP32\"}");

  // con i dati letti dal DHT
  Serial.print("{\"sensor\": \"DHT11\", \"temperature_C\": ");
  Serial.print(temperature);
  Serial.print(", \"humidity_%\": ");
  Serial.print(humidity);
  Serial.println("}");

  // payload JSON per mq135
  Serial.print("{\"sensor\": \"MQ\", \"rawValue\": ");
  Serial.print(sensorValue);
  Serial.println("}");

  // Divisionne lettutre
  Serial.println("{\----------------------\}");

  // Il DHT11 può essere letto al massimo una volta ogni 2 secondi
  delay(2000); 
}