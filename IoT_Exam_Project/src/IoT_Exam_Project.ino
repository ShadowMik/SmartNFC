/*
 * Project IoT_Exam_Project
 * Description: Smart NFC
 * Author: Anders, Leo og Tobias
 * Date: 
 */


// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_PN532.h"
#include <iostream>
#include <String>
#include <map>
#include <iterator>
#include <fcntl.h>
using namespace std;

#define DEBUG_PRINT(...) { Particle.publish( "DEBUG", String::format(__VA_ARGS__) ); }
#define LOG_PRINT(...) { Particle.publish( "LOG", String::format(__VA_ARGS__) ); }

const size_t msgsize = 1000;
const int SS_PIN = A5;
const int SCK_PIN = D13;
const int MISO_PIN = D11;
const int MOSI_PIN = D12;

const pin_t Red_LED = D1;
const pin_t Green_LED = D0;

std::map<uint32_t, String> ids;

// note: these are not used for SPI mode
const int IRQ_PIN = A0;
const int RST_PIN = A1;

// set to SPI or I2C depending on how the header file is configured
#if PN532_MODE == PN532_SPI_MODE
  Adafruit_PN532 nfc(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
#elif PN532_MODE == PN532_I2C_MODE
  Adafruit_PN532 nfc(IRQ_PIN, RST_PIN);
#endif


void checkCardID(uint32_t cardID);

void addKey();

void removeKey();

void addKeyThroughReader();

void showState();

void lockUnlock();

void saveUsers();

void loadUsers();

void setup() {

    Serial.begin(115200);

    pinMode(Red_LED, OUTPUT);
    pinMode(Green_LED, OUTPUT);

    while(!Serial){
        delay(10);
    }


    //manually add two ids
    ids[3283616780] = "Tobias blå";
    ids[3248756763] = "Tobias hvid";
    
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
            checkCardID(cardid);
        }
       
        Serial.println("");
    }
}

void checkCardID(uint32_t cardID){
   
    auto it = ids.find(cardID);
    if (it != ids.end()){
        LOG_PRINT(ids.at(cardID));
        digitalWrite(Green_LED, HIGH);
        lockUnlock();
        delay(2000);
        digitalWrite(Green_LED, LOW);
    } else {
        digitalWrite(Red_LED, HIGH);
        delay(2000);
        digitalWrite(Red_LED, LOW);
    }



}

void addKey(){

}

void removeKey(){

}

void addKeyThroughReader(){

}

void showState(){

}

void lockUnlock(){
    //check if locked
    // unlock
    // else
    // lock
}

/* work in progress
void saveUsers(){
    int fd = open("Users.txt", O_RDWR | O_CREAT);
    if (fd != -1){
        string msg = "hej";
        write(fd, msg.c_str(),msgsize);
    }
    close(fd);
}

void loadUsers(){
    int fd = open("Users.txt", O_RDWR | O_CREAT);
    if (fd != -1){
        void* msg;
        DEBUG_PRINT((char*) read(fd,msg, msgsize));

        Serial.println((char*) msg);
        Serial.println(msgsize);

    }
    close(fd);
}*/