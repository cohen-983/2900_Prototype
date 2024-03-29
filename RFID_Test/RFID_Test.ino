#include <MFRC522.h>
#include <SPI.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN); //instance of the RFID reader object

MFRC522::MIFARE_Key key;

int uidCode1[] = {105,52,80,71}; //initialize array to store uid value
int uidCode2[] = {203,133,44,11}; //initialize array to store uid value
int codeRead = 0;
String uidString; //initialize remaining required components

void readRFID(); //function prototypes
void printUID();

void setup()
{
  Serial.begin(9600); // begin serial transmission
  SPI.begin(); // initialize I2C bus
  rfid.PCD_Init(); // initialize the rfid reader
}

void loop() {
  if(rfid.PICC_IsNewCardPresent())
  {
      readRFID();
  }
  delay(100);

}

void readRFID()
{
  
  rfid.PICC_ReadCardSerial();
  Serial.print(F("\nPICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println("Your tag is not of type MIFARE Classic.");
    return;
  }

    Serial.println("Scanned PICC's UID:");
    printDec(rfid.uid.uidByte, rfid.uid.size);

    uidString = String(rfid.uid.uidByte[0])+" "+String(rfid.uid.uidByte[1])+" "+String(rfid.uid.uidByte[2])+ " "+String(rfid.uid.uidByte[3]);

    int i = 0;
    boolean match = true;
    boolean match2 = true;
    while(i<rfid.uid.size)
    {
      if(!(rfid.uid.uidByte[i] == uidCode1[i]))
      {
      	match = false;
      }
      if(!(rfid.uid.uidByte[i] == uidCode2[i]))
      {
      	match2 = false;
      }
      i++;
    }

    if(match)
    {
      Serial.println("\nI know this card! \nIt's card 1!");
    }
    else if(match2)
    {
    	Serial.println("\nI know this card! \nIt's card 2!");
    }
    else
    {
      Serial.println("\nUnknown Card");
    }


    // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}