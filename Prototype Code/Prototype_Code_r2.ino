#include <MFRC522.h>
#include <SPI.h>
#include <ShiftLCD.h>

#define SS_PIN 10
#define RST_PIN 9

#define SERIAL_DATA 2
#define SERIAL_CLOCK 4
#define RESET_CLOCK 3

MFRC522 rfid(SS_PIN, RST_PIN); //instance of the RFID reader object

MFRC522::MIFARE_Key key;

int uidCode1[] = {105,52,80,71}; //initialize array to store uid value
int uidCode2[] = {203,133,44,11}; //initialize array to store uid value
int codeRead = 0;
String uidString; //initialize remaining required components

ShiftLCD lcd(SERIAL_DATA, SERIAL_CLOCK, RESET_CLOCK); //initialize the LCD with the defined pins

void readRFID(); //function prototypes
void printDec();

void setup()
{
  Serial.begin(9600); // begin serial transmission
  SPI.begin(); // initialize I2C bus
  rfid.PCD_Init(); // initialize the rfid reader
  lcd.begin(16, 2); // initialize the LCD with its number of columns and rows
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
      Serial.println("\nI know this card! \nIt's your keys!");
      lcd.clear();
      lcd.print("Keys found!");
    }
    else if(match2)
    {
      Serial.println("\nI know this card! \nIt's something other than your keys!");
      lcd.clear();
      lcd.print("Not your keys!");
    }
    else
    {
      Serial.println("\nUnknown Card");
      lcd.clear();
      lcd.print("Unknown!");
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