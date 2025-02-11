#include <Wire.h>
#include <Adafruit_PN532.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Display Settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// PN532 NFC Settings
Adafruit_PN532 nfc(A4, A5);  

// Define multiple known card UIDs
struct Card {
  uint8_t uid[7];  // Max UID length
  uint8_t uidLength;
  const char* name; // Label for the card
};

// Store known cards
Card knownCards[] = {
  {{0xE3, 0xA8, 0xBA, 0x14}, 4, "Card 1"},
  {{0x35, 0x74, 0x43, 0x2}, 4, "Card 2"},  // Example second card UID
  {{0x04, 0x91, 0xA2, 0xB3, 0xC4, 0xD5, 0xE6}, 7, "Card 3"}  // Example 7-byte UID
};

void setup() {
  Serial.begin(115200);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("NFC Scanner Ready!");
  display.display();

  // Initialize PN532
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Didn't find PN532 board");
    while (1);
  }

  nfc.SAMConfig();
  Serial.println("Ready to scan NFC cards...");
}

void loop() {
  uint8_t success;
  uint8_t uid[7];  // Buffer for UID
  uint8_t uidLength;

  // Wait for an NFC card
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    Serial.println("Card detected!");
    Serial.print("UID: ");
    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print(" 0x");
      Serial.print(uid[i], HEX);
    }
    Serial.println("");

    // Compare the scanned UID with stored UIDs
    bool cardFound = false;
    for (uint8_t i = 0; i < sizeof(knownCards) / sizeof(knownCards[0]); i++) {
      if (uidLength == knownCards[i].uidLength && memcmp(uid, knownCards[i].uid, uidLength) == 0) {
        Serial.println(knownCards[i].name);
        
        // Display on OLED
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(10, 20);
        display.println(knownCards[i].name);
        display.display();

        cardFound = true;
        break;
      }
    }

    if (!cardFound) {
      Serial.println("Unknown card!");

      // Display on OLED
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(10, 20);
      display.println("Unknown Card");
      display.display();
    }

    delay(2000);  // Delay before scanning again
  }
}