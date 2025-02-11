

#include <Wire.h>
#include <Adafruit_PN532.h>

#define SDA_PIN A4
#define SCL_PIN A5

Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

void setup(void) {
  Serial.begin(115200);
  Serial.println("Hello! Scan an NFC/RFID tag to see the UID!");

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Didn't find PN53x board");
    while (1);
  }

  // Configure to read RFID tags
  nfc.SAMConfig();
}

void loop(void) {
  uint8_t success;
  uint8_t uid[7];  // Buffer to store the returned UID
  uint8_t uidLength;

  // Wait for an NFC card
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    Serial.println("Found a card!");
    Serial.print("UID Length: ");
    Serial.print(uidLength, DEC);
    Serial.println(" bytes");
    Serial.print("UID Value: ");

    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print(" 0x");
      Serial.print(uid[i], HEX);
    }
    Serial.println("");

    // Define the UID of "card 1"
    uint8_t card1UID[] = {0xE3, 0xA8, 0xBA, 0x14};

    // Check if the scanned UID matches card1UID
    if (uidLength == 4 && memcmp(uid, card1UID, 4) == 0) {
      Serial.println("Card 1 read");
    } else {
      Serial.println("Unknown card");
    }

    delay(1000);  // Wait before scanning again
  }
}