#include <EEPROM.h>
#include <LiquidCrystal.h>
#include "pitches.h"

#define SENSORPIN A0 // select the input pin for the sensor
#define SPEAKERPIN 13 // select the output pin for the speaker
#define BUTTONPIN 8 // select the input pin for the button
#define LEDPIN 7 // select the output pin for the led
#define READ_STATE 0
#define WRITE_STATE 1
#define END_RECORD 255

unsigned long targetTime = 0;
const unsigned long interval = 250;

int state = WRITE_STATE;
int writeAddress = 0;
int readAddress = 0;

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup(){
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Music Machine!");

  // initialize the speaker pin as an output:
  pinMode(SPEAKERPIN, OUTPUT);

  // initialize the button pin as an input:
  pinMode(BUTTONPIN, INPUT);

  // initialize the LED pin as an output:
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, HIGH);
  
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void loop(){
  // check the button state to flip states if needed
  int buttonState = digitalRead(BUTTONPIN);
  if (buttonState == HIGH) {
    switch (state) {
      case READ_STATE:
        state = WRITE_STATE;
        writeAddress = 0;
        digitalWrite(LEDPIN, HIGH);
        break;

      case WRITE_STATE:
        state = READ_STATE;
        readAddress = 0;
        digitalWrite(LEDPIN, LOW);
        EEPROM.write(writeAddress, END_RECORD);
        break;
    }
  }

  // complete a single read or write on time interval
  if(millis() >= targetTime) {
    switch (state) {
      case READ_STATE:
        doRead();
        break;

      case WRITE_STATE:
        doWrite();
        break;
    }
  
    targetTime = millis() + interval;
  }
}

void doRead() {
  // there are no more values to be read
  if (readAddress > 1022 || EEPROM.read(readAddress) == END_RECORD) {
    return;
  }

  int nIndex = EEPROM.read(++readAddress);
  int dIndex = EEPROM.read(++readAddress);

  playSound(nIndex, dIndex);
}

void doWrite() {
  int sensorValue = analogRead(SENSORPIN);
  int nIndex = noteIndex(sensorValue);
  int dIndex = durationIndex(sensorValue);
  String data = String(sensorValue) + "," + noteType(nIndex) + "," + durationType(dIndex) + "\r\n";

  writeSound(nIndex, dIndex);
  playSound(nIndex, dIndex);
  Serial.write(data.c_str());
}

void writeSound(int noteIndex, int durationIndex) {
  // we have run out of space to write the values
  if (writeAddress > 1022) {
    return;
  }
  
  EEPROM.write(++writeAddress, noteIndex);
  EEPROM.write(++writeAddress, durationIndex);
}

void playSound(int noteIndex, int durationIndex) {  
  // to calculate the note duration, take one second divided by the note type.
  // e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
  tone(SPEAKERPIN, noteValue(noteIndex), 1000 / durationIndex);
}

int noteIndex(int sensorValue) {
  return map(sensorValue, 100, 800, 1, 7);
}

int durationIndex(int sensorValue) {
  return map(sensorValue, 100, 800, 1, 8);
}

String durationType(int durationIndex) {
  switch (durationIndex) {
    case 1:
      return "4";
    case 2:
      return "2";
    case 3:
      return "4/3";
    case 4:
      return "1";
    case 5:
      return "4/5";
    case 6:
      return "2/3";
    case 7:
      return "4/7";
    case 8:
      return "1/2";
  }
}

String noteType(int noteIndex) {
  switch (noteIndex) {
    case 1:
      return "C";
    case 2:
      return "D";
    case 3:
      return "E";
    case 4:
      return "F";
    case 5:
      return "G";
    case 6:
      return "A";
    case 7:
      return "B";
  }
}

int noteValue(int noteIndex) {
  switch (noteIndex) {
    case 1:
      return NOTE_C4;
    case 2:
      return NOTE_D4;
    case 3:
      return NOTE_E4;
    case 4:
      return NOTE_F4;
    case 5:
      return NOTE_G4;
    case 6:
      return NOTE_A4;
    case 7:
      return NOTE_B4;
  }
}
