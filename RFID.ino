#include <EEPROM.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 53
#define RST_PIN 5
#define WARNING_LED 13

byte currentCard[4];
byte cardList[2][4];

MFRC522 rfReader(SS_PIN, RST_PIN);

void setup() {
  setupPins();
  Serial.begin(9600);
  SPI.begin();
  rfReader.PCD_Init();

  // Kiem tra xem it nhat 1 card luu trong EEPROM
  // Check if atleast 1 card saved in EEPROM
  if (numberOfSavedCards() == 0) {
    Serial.println("No card found");
    newCardWizard();
  }
  Serial.print("Number of saved cards: ");
  Serial.println(numberOfSavedCards(), DEC);
  for (size_t i = 1; i < 5; i++) {
    Serial.print(EEPROM.read(i), HEX);
  }
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

// Scan a card to currentCard
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
