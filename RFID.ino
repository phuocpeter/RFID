#include <EEPROM.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 53
#define RST_PIN 5
#define WARNING_LED 13

byte currentCard[4];
byte cardList[2][4];
int i = 0;

MFRC522 rfReader(SS_PIN, RST_PIN);

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

void setup() {
  setupPins();
  Serial.begin(9600);
  SPI.begin();
  rfReader.PCD_Init();

  // Kiem tra xem it nhat 1 card luu trong EEPROM
  // Check if atleast 1 card saved in EEPROM
  if (numberOfSavedCards() == 0) {

  }

  Serial.println(EEPROM.length());
  Serial.println("Please scan");
  do {
    readCard(i);
  } while (i <= 1);
  printAll();
}

void loop() {

}

void readCard(int a) {
  if (!rfReader.PICC_IsNewCardPresent()) { return; }
  if (!rfReader.PICC_ReadCardSerial()) { return; }
  for (size_t i = 0; i < 4; i++) {
    currentCard[i] = rfReader.uid.uidByte[i];
    //Serial.print(currentCard[i], HEX);
    cardList[a][i] = rfReader.uid.uidByte[i];
  }
  rfReader.PICC_HaltA();
  Serial.println("Saved");
  i++;
}

void printAll() {
  for (size_t i = 0; i < 2; i++) {
    Serial.print("Card " + (String)i + ": ");
    for (size_t j = 0; j < 4; j++) {
      Serial.print(cardList[i][j], HEX);
    }
    Serial.println("");
  }
}
