#include <MFRC522.h>
#include <SPI.h>
#include <ShiftLCD.h> //See reference [3]

#define SS_PIN 10
#define RST_PIN 9

#define SERIAL_DATA 2
#define SERIAL_CLOCK 4
#define RESET_CLOCK 3

#define LED_R 14
#define LED_G 15
#define LED_B 16

#define BUZZER 6

#define BUTTON 19
#define SWITCH 18

MFRC522 rfid(SS_PIN, RST_PIN); //instance of the RFID reader object [2]

MFRC522::MIFARE_Key key; //[2]

int uidCode1[] = {105,52,80,71}; //initialize array to store uid value
int uidCode2[] = {203,133,44,11}; //initialize array to store uid value
int codeRead = 0;
String uidString; //initialize remaining required components

ShiftLCD lcd(SERIAL_DATA, SERIAL_CLOCK, RESET_CLOCK); //initialize the LCD with the defined pins

void readRFID(); //function prototypes
void printDec();
void buzzAlert(int x); //this one only really exists for contraining values and ease of reading
bool buttonCheck(int buttPin); //same for this one, debouncing is nasty, so it's hidden away
void ledColor(int col); //same drill

int doorOpen = HIGH; //initalize a tracking boolean
bool itemOnePresent = false; //booleans to track the state of the rfid enabled
bool itemTwoPresent = false; //items in question.

bool alarmOverride = false; //boolean to ignore the alarm

void setup() {

  Serial.begin(9600); // begin serial transmission
  SPI.begin(); // initialize I2C bus
  rfid.PCD_Init(); // initialize the rfid reader
  lcd.begin(16, 2); // initialize the LCD with its number of columns and rows
  lcd.setCursor(0,0);
  
  pinMode(BUZZER, OUTPUT); //buzzer pin init

  pinMode(LED_R, OUTPUT); //rgb led pin inits
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  pinMode(BUTTON, INPUT); //button and
  pinMode(SWITCH, INPUT); //switch pin inits

}

void loop() {

  doorOpen = digitalRead(SWITCH);

  while(doorOpen == LOW){ //while the door is open
    if(rfid.PICC_IsNewCardPresent()) //check for a new RFID card
    {
        readRFID(); //if one is present, read it.  If one isn't, just continue.
    }


    if(itemOnePresent == false && itemTwoPresent == false && alarmOverride == false)
    {
      //Serial.println("Case 1");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Nothing's here!");
      buzzAlert(50);
      ledColor(3);
      delay(200);
    }
    else if(itemOnePresent == true && itemTwoPresent == false && alarmOverride == false)
    {
      //Serial.println("Case 2");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Item 1 is here!");
      lcd.setCursor(0,1);
      lcd.print("Item 2 is not!");
      buzzAlert(50);
      ledColor(3);
      delay(200);
    }
    else if(itemOnePresent == false && itemTwoPresent == true && alarmOverride == false)
    {
      //Serial.println("Case 3");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Item 2 is here!");
      lcd.setCursor(0,1);
      lcd.print("Item 1 is not!");
      buzzAlert(50);
      ledColor(3);
      delay(200);
    }
    else if(itemOnePresent == true && itemTwoPresent == true && alarmOverride == false)
    {
      //Serial.println("Case 4");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Both items are");
      lcd.setCursor(0,1);
      lcd.print("here!");
      buzzAlert(0);
      ledColor(1);
      delay(200);
    }

    if(alarmOverride == false)
      alarmOverride = buttonCheck(BUTTON);

    if(alarmOverride == true)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Alarm bypassed!");
      buzzAlert(0);
      ledColor(2);
      delay(200);
    }
    doorOpen = digitalRead(SWITCH);
    delay(100); // then wait so the systems doen't get bogged down
  }
  itemOnePresent = false; //rewrite the booleans once/if the door is closed.
  itemTwoPresent = false;
  alarmOverride = false;
  ledColor(0);
  lcd.clear();
  buzzAlert(0);

  delay(50); //wait before checking the switch for a kind of janky debounce.
}

void readRFID() { //A great deal of the initial code in this function and its subfunctions are from reference [2].
  
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
      //lcd.clear();
      //lcd.print("Keys found!");
      //LCD print statements in this code are vestigal, now addressed with the boolean below.
      itemOnePresent = true;
    }
    else if(match2)
    {
      Serial.println("\nI know this card! \nIt's something other than your keys!");
      //lcd.clear();
      //lcd.print("Not your keys!");
      itemTwoPresent = true;
    }
    else
    {
      Serial.println("\nUnknown Card");
      //lcd.clear();
      //lcd.print("Unknown!");
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
} // after this point, all code is more or less original.

void buzzAlert(int x) { //given a volume value between 0 and 100, sounds the buzzer
  int y = constrain(x, 0, 100);
  y = map(y, 0, 100, 0, 255);
  analogWrite(BUZZER, y);
}

bool buttonCheck(int buttPin) {
  int butt1 = HIGH;
  int butt2 = LOW;
  butt1 = digitalRead(buttPin);
  delay(75);
  butt2 = digitalRead(buttPin);
  if(butt1 == HIGH && butt1 == butt2)
  {
    return true;
  }
  else if(butt1 == LOW && butt1 == butt2)
  {
    return false;
  }
  else
    return false;
}

void ledColor(int col) {
  int rgb = constrain(col, 0, 3);
  switch (col) {
      case 0: // off
        digitalWrite(LED_B, LOW);
        digitalWrite(LED_R, LOW);
        digitalWrite(LED_G, LOW);
        break;
      case 1: //green
        digitalWrite(LED_B, LOW);
        digitalWrite(LED_R, LOW);
        digitalWrite(LED_G, HIGH);
        break;
      case 2: //amber
        digitalWrite(LED_B, LOW);
        digitalWrite(LED_R, HIGH);
        digitalWrite(LED_G, HIGH);
        break;
      case 3: //red
        digitalWrite(LED_B, LOW);
        digitalWrite(LED_R, HIGH);
        digitalWrite(LED_G, LOW);
        break;
  }
}