#include <EEPROM.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 53
#define RST_PIN 5
#define WARNING_LED 13

byte currentCard[4];
// Toi da 50 card co the luu vao EEPROM
// Maximum 50 cards could be saved to EEPROM
byte cardList[50][4];

MFRC522 rfReader(SS_PIN, RST_PIN);

void setup() {
  setupPins();
  Serial.begin(9600);
  SPI.begin();
  rfReader.PCD_Init();

  newCardWizard();

  checkForEmptyCardSaved();
  addCardsToRAM();
  printAllCards();
}

void loop() {

}

void setupPins() {
  pinMode(WARNING_LED, OUTPUT);
  digitalWrite(WARNING_LED, LOW);
}

// Chop tat den led
// Flash the led with delay time
void flashLED(int pin, int delayTime) {
  digitalWrite(pin, HIGH);
  delay(delayTime);
  digitalWrite(pin, LOW);
  delay(delayTime);
}

// Kiem tra xem it nhat 1 card luu trong EEPROM
// Check if atleast 1 card saved in EEPROM
void checkForEmptyCardSaved() {
  if (numberOfSavedCards() == 0) {
    Serial.println("No card found");
    newCardWizard();
  }
}

// Luu the tu EEPROM vao RAM
// Add cards from EEPROM to RAM
void addCardsToRAM() {
  int savedCards = numberOfSavedCards();
  for (int i = 0; i < savedCards; i++) {
    // Luu the vao bien currentCard
    // Assign card to currentCard
    int slot = 1;
    for (int j = 0; j < 4; j++) {
      currentCard[j] = EEPROM.read(i * 4 + slot);
      slot++;
    }
    // Luu currentCard vao cardList
    // Append currentCard to cardList
    for (int j = 0; j < 4; j++) {
      cardList[i][j] = currentCard[j];
    }
  }
  Serial.println("Cards added to RAM");
}

// In card trong RAM ra Serial
// Print cards from RAM
void printAllCards() {
  int savedCards = numberOfSavedCards();
  for (int i = 0; i < savedCards; i++) {
    Serial.print("Card ");
    Serial.print(i + 1);
    Serial.print(": ");
    for (int j = 0; j < 4; j++) {
      Serial.print(cardList[i][j], HEX);
    }
    Serial.println("");
  }
}

// Doc byte dau tien trong EEPROM
// Read the first byte in the EEPROM
byte numberOfSavedCards() {
  return EEPROM.read(0);
}

// Giao dien them card
// Interface to add card
void newCardWizard() {
  Serial.println("--ADD NEW CARD--");
  Serial.println("Please scan...");
  bool cardDectected = false;
  do {
    cardDectected = scanCard();
  } while (!cardDectected);
  saveCard();
}

// Luu the vao EEPROM
// Save card to EEPROM
void saveCard() {
  int startIndex = numberOfSavedCards() * 4 + 1;
  int endIndex = startIndex + 4;
  int i = 0;
  for (int address = startIndex; address < endIndex; address++) {
    EEPROM.write(address, currentCard[i]);
    i++;
  }
  EEPROM.write(0, EEPROM.read(0) + 1);
  Serial.println("Card saved");
}

// Scan the va luu gia tri vao bien currentCard
// Scan a card and assign to currentCard
bool scanCard() {
  if (!rfReader.PICC_IsNewCardPresent()) { return false; }
  if (!rfReader.PICC_ReadCardSerial()) { return false; }
  Serial.print("Card Detected: ");
  for (size_t i = 0; i < 4; i++) {
    currentCard[i] = rfReader.uid.uidByte[i];
    Serial.print(currentCard[i], HEX);
  }
  Serial.println("");
  rfReader.PICC_HaltA();
  return true;
}
