/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/Tobias/Documents/Particle_IO/SmartNFC/IoT_Exam_Project/src/IoT_Exam_Project.ino"
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
//using namespace std;

#include <vector>
#include <string>
#include <sstream>
#include <iostream>


void setup();
void loop();
#line 24 "c:/Users/Tobias/Documents/Particle_IO/SmartNFC/IoT_Exam_Project/src/IoT_Exam_Project.ino"
#define DEBUG_PRINT(...) { Particle.publish( "DEBUG", String::format(__VA_ARGS__) ); }
#define LOG_PRINT(...) { Particle.publish( "LOG", String::format(__VA_ARGS__) ); }

const size_t msgsize = 1000;

uint32_t cardid;
int counter = 0;
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

void NFCloop();

void checkCardID(uint32_t cardID);

int addKey(String ID);

int removeKey(String ID);

int addKeyThroughReader(String ID);

void showState();

void lockUnlock();

void saveUsers();

void loadUsers();

void setup() {

    Serial.begin(115200);

    pinMode(Red_LED, OUTPUT);
    pinMode(Green_LED, OUTPUT);

    Particle.function("add user example (3283616780 Tobias)", addKey);
    Particle.function("remove user example (3283616780)", removeKey);
    Particle.function("add user through reader example (name to card)", addKeyThroughReader);

    while(!Serial){
        delay(10);
    }


    //manually add two ids
    //ids[3283616780] = "Tobias blå";
    //ids[347178822] = "Tobias studiekort"
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

    NFCloop();

}

void NFCloop(){
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
            counter = 0;
            cardid = uid[0];
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
    Serial.println(cardid);
    counter ++;
    if (counter > 60){
        cardid = 0;
        counter = 0;
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

int addKey(String ID){

    char *name;
    u_int32_t number = strtoul(ID,&name,10); 

    if (number > 1) {
        ids[number] = name;
        return 1;
    } else {
        return 0;
    }
}

int removeKey(String ID){
    char *name;
    u_int32_t number = strtoul(ID,&name,10); 

    auto it = ids.find(number);
    if (it != ids.end()){
        ids.erase(number);
        return 1;
    } else {
        return 0;
    }
}

int addKeyThroughReader(String ID){
    //add name and then scan the card

    auto it = ids.find(cardid);
    if (it != ids.end()){
        return -1; //is allready there
    } else if (cardid > 1) {
        ids[cardid] = ID;
        return 1;
    } else {
        return 0;
    }
}

void showState(){
    //particle variable
}

void lockUnlock(){
    //check if locked
    // unlock
    // else
    // lock
}

/*
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
        (char*) read(fd,msg, msgsize);

        Serial.println((char*) msg);
        Serial.println(msgsize);

    }
    close(fd);
}*/