#include "Adafruit_PN532.h"
#include <map>

// Setup of Particle Argon pins
const int SS_PIN = A5;
const int SCK_PIN = D13;
const int MISO_PIN = D11;
const int MOSI_PIN = D12;
const pin_t Red_LED = D1;
const pin_t Green_LED = D0;

// Set PN532 to SPI-mode (remember physical setup to SPI on module: 1 low, 2 high)
#if PN532_MODE == PN532_SPI_MODE
  Adafruit_PN532 nfc(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
#endif

// Function declarations
int addKey(String ID);
int removeKey(String ID);
void checkNFCID(uint32_t checkID);
void resultHandler(const char *event, const char *data);
void lockUnlock();

// Variable declarations
std::map<uint32_t, String> ids;     // Contains locally stored NFC IDs
uint32_t nfcID;                     // Contains latest read NFC ID returned from PN532
bool userRegistered = false;        // Global var to track whether user is registered locally or in cloud. Used to log succesful user access in database
bool doorState = false;             // Global var tracking door state (locked or unlocked)


void setup() {
    Serial.begin(115200);
    pinMode(Red_LED, OUTPUT);
    pinMode(Green_LED, OUTPUT);

    Particle.function("Add user (example: 3283616780 Tobias)", addKey);
    Particle.function("Remove user (example: 3283616780)", removeKey);

    while(!Serial){
        delay(10);
    }
    
    ids[330474253] = "Anders Brik";

    nfc.begin();

    // Wait for established connection to NFC module PN532
    uint32_t versiondata;
    do {
        versiondata = nfc.getFirmwareVersion();
        if (!versiondata) {
            Serial.println("no board");
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
    
    // Configure board to read RFID tags
    nfc.SAMConfig();

    // Subscribe to Particle topic relating to comparison of read NFC IDs and IDs stored in database
    Particle.subscribe("ResultCheckNFCID", resultHandler);
}


void loop() {
    uint8_t success = 0;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };    // Buffer to store the returned UID
    uint8_t uidLength = 0;                      // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength); // Scan for NFC devices
    
    // If NFC device successfully read, do the following
    if (success) {
        // Display basic information about registered NFC device
        Serial.println("NFC device detected");
        Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
        Serial.print("  UID Value: ");
        nfc.PrintHex(uid, uidLength);
        
        // Convert NFC ID from hex to uint32
        if (uidLength == 4) {
            nfcID = uid[0];
            nfcID <<= 8;
            nfcID |= uid[1];
            nfcID <<= 8;
            nfcID |= uid[2];  
            nfcID <<= 8;
            nfcID |= uid[3]; 
            Serial.print("NFC device ID: "); Serial.println(nfcID);

            checkNFCID(nfcID);  // Run NFC ID check
        }
        Serial.println("");
    }
}


/*  
Method functionality:
    1. Check if passed NFC ID is stored locally in ids map variable. If yes, unlock/lock through function
    2. If NFC ID is NOT stored locally, initialize check to see if stored in cloud database via Particle.publish 
    3. If a user (NFC ID) is registered locally or in cloud, set global var userTracker to true, and publish info to access logging database
*/
void checkNFCID(uint32_t checkID){
    auto it = ids.find(checkID);
    if (it != ids.end()){
        Serial.println("NFC ID stored on device");
        digitalWrite(Green_LED, HIGH);
        userRegistered = true;
        lockUnlock();
        delay(2000); // Wait 2 seconds
        digitalWrite(Green_LED, LOW);
        
    } else {
        Serial.println("NFC ID not stored on device");
        Particle.publish("CheckNFCID", String::format("%lu", checkID));
        delay(3000); // Wait 3 seconds, Ensures next if-block is not skipped if user is found in cloud
    }
    if (userRegistered == true) {
        char data[256];
        snprintf(data, sizeof(data), "{\"nfcid\":%lu, \"doorstate\":%s}", checkID, doorState ? "true" : "false");   // Mustache implementation to pass two data-points along to Particle console
        Particle.publish("logUser", data);
        userRegistered = false;     // Reset variable to false, ready for next check
    }
}


// Adds users to the local storage through the Particle function 'Add user'
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


// Removes users from the local storage through the Particle function 'Remove user'
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

// Control of global var to track state of door (locked / unlocked)
void lockUnlock(){
    if (doorState == false) {
        doorState = true;   // Locked
    }
    else if (doorState == true) {
        doorState = false;  // Unlocked
    }
}


// Handler called when event occurs on Particle topic 'ResultCheckNFCID'
// An event will contain the response of the Particle-publish CheckNFCID. True if ID is found, False if not
void resultHandler(const char *event, const char *data) { 
    std::string res = std::string(data);                            // Converts const char * data to string
    res.erase(remove(res.begin(), res.end(), '\"'), res.end());     // Removes quotation marks around data response

    // If NFC ID was found in cloud database, res = True
    if (res == "True") {                                            
        Serial.println("NFC ID stored in cloud");
        digitalWrite(Green_LED, HIGH);
        userRegistered = true;
        lockUnlock();
        delay(2000);
        digitalWrite(Green_LED, LOW);
    } 
    // If NFC ID was NOT found in cloud database, res = False
    else if (res == "False") {                                      
        Serial.println("NFC ID not stored in cloud");
        digitalWrite(Red_LED, HIGH);
        delay(2000);
        digitalWrite(Red_LED, LOW);
    }
    // If data response was an error, flash red LED
    else {                                                          
        for (int i = 1; i<=10; ++i) {
            digitalWrite(Red_LED, HIGH);
            delay(100);
            digitalWrite(Red_LED, LOW);
            delay(100);
        }
    }
}
