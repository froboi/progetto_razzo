#include "FS.h"
#include "SD.h"
#include "SPI.h"

// Dichiarazione dei pin definiti dall'utente all'inizio del codice
const int PIN_SD_CS   = 5;   // Collegato al Pin 2 della SD
const int PIN_SD_MOSI = 23;  // Collegato al Pin 3 della SD
const int PIN_SD_SCK  = 18;  // Collegato al Pin 6 della SD
const int PIN_SD_MISO = 19;  // Collegato al Pin 8 della SD

void setup() {
  Serial.begin(115200);
  delay(1000); // Piccola pausa per stabilizzare la seriale
  Serial.println("\nAvvio test scrittura SD con pin personalizzati...");

  // Configura e inizializza il bus SPI con i tuoi pin specifici
  // La sintassi è: SPI.begin(SCK, MISO, MOSI, CS)
  SPI.begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);

  // Inizializza la libreria integrata SD passando il pin CS dichiarato
  if (!SD.begin(PIN_SD_CS)) {
    Serial.println("Errore: Montaggio della SD fallito! Controlla i collegamenti.");
    return;
  }

  // Verifica il tipo di scheda (opzionale, utile per debug)
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("Errore: Nessuna scheda SD inserita.");
    return;
  }

  Serial.println("Scheda SD inizializzata correttamente!");

  // Definiamo il percorso del file txt
  String path = "/mio_file.txt";
  Serial.printf("Apertura/Creazione del file: %s\n", path.c_str());

  // Apri il file in modalità scrittura (FILE_WRITE)
  File file = SD.open(path, FILE_WRITE);
  
  if (!file) {
    Serial.println("Errore: Impossibile aprire il file per la scrittura.");
    return;
  }

  // Scrivi la stringa nel file di testo
  if (file.println("Test riuscito! Scrittura effettuata con la libreria integrata SD e pin personalizzati.")) {
    Serial.println("Testo scritto con successo sulla SD!");
  } else {
    Serial.println("Errore durante la scrittura dei dati.");
  }

  // Chiudi il file per salvare definitivamente le modifiche
  file.close();
}

void loop() {
  // Lasciato vuoto intenzionalmente per eseguire il test una sola volta
}