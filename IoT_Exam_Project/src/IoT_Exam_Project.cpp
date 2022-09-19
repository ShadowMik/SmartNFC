/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/Tobias/Documents/Particle_IO/IoT_Exam_Project/src/IoT_Exam_Project.ino"
/*
 * Project IoT_Exam_Project
 * Description: Smart NFC
 * Author: Anders, Leo og Tobias
 * Date: 
 */


// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_PN532.h"


void setup();
void loop();
#line 13 "c:/Users/Tobias/Documents/Particle_IO/IoT_Exam_Project/src/IoT_Exam_Project.ino"
#define DEBUG_PRINT(...) { Particle.publish( "DEBUG", String::format(__VA_ARGS__) ); }

const int SS_PIN = A5;
const int SCK_PIN = D13;
const int MISO_PIN = D11;
const int MOSI_PIN = D12;

// note: these are not used for SPI mode
const int IRQ_PIN = A0;
const int RST_PIN = A1;

// set to SPI or I2C depending on how the header file is configured
#if PN532_MODE == PN532_SPI_MODE
  Adafruit_PN532 nfc(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
#elif PN532_MODE == PN532_I2C_MODE
  Adafruit_PN532 nfc(IRQ_PIN, RST_PIN);
#endif

void setup() {

    Serial.begin(115200);
    //Wire.setSpeed(115200);

    while(!Serial){
        delay(10);
    }

    nfc.begin();

    uint32_t versiondata;

    do {
        versiondata = nfc.getFirmwareVersion();
        if (!versiondata) {
            // tast "particle serial monitor " i CLI
            Serial.println("no board");
            DEBUG_PRINT("no board");
            delay(1000);
        }
    }
    while (!versiondata);
    
    Serial.print("Found chip PN5"); 
    Serial.println((versiondata>>24) & 0xFF, HEX); 
    Serial.print("Firmware ver. "); 
    Serial.print((versiondata>>16) & 0xFF, DEC); 
    Serial.print('.'); 
    Serial.println((versiondata>>8) & 0xFF, DEC);
    
    // configure board to read RFID tags
    nfc.SAMConfig();
  
    Serial.println("Waiting for an ISO14443A Card ...");
}

void loop() {
    uint8_t success = 0;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
    uint8_t uidLength = 0;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);

    if (success) {
        // Display some basic information about the card
        Serial.println("Found an ISO14443A card");
        Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
        Serial.print("  UID Value: ");
        nfc.PrintHex(uid, uidLength);
     
        if (uidLength == 4) {
            uint32_t cardid = uid[0];
            cardid <<= 8;
            cardid |= uid[1];
            cardid <<= 8;
            cardid |= uid[2];  
            cardid <<= 8;
            cardid |= uid[3]; 
            Serial.print("Mifare Classic card #");
            Serial.println(cardid);
        }
       
        Serial.println("");
    }
}