#include <MIDI.h>
#include <midi_Defs.h>
#include <midi_Message.h>
#include <midi_Namespace.h>
#include <midi_Settings.h>

// this is the code for the MISSION CONTROL module
// the idea: hook up all the pins of an arduino to a panel
// connect the arduino to my computer using a USB cable
// send midi from my computer to the arduino via serial messages
// using the ttymidi (linux) or hairless midi bridge (mac)

// in practice (as of January 2018):
// midi white key notes from C1 to A1 trigger the top row of pins on the arduino
// I use this for gate/trigger voltages to control the modular and sync to bitwig, etc.

// requirements:
// Mozzi Arduino library
// <https://sensorium.github.io/Mozzi/>


// SETUP ------------------------------------------------
MIDI_CREATE_DEFAULT_INSTANCE();

#include <MIDI.h>
#include <MozziGuts.h> // 3rd party MIDI library

// digital pins
static const unsigned pin2 = 2;      
static const unsigned pin4 = 4;      
static const unsigned pin7 = 7;
static const unsigned pin8 = 8;      
static const unsigned pin12 = 12;      
static const unsigned pin13 = 13;    

// PWM pins
int pin3 = 3;
int pin5 = 5;
int pin6 = 6;
int pin9 = 9;
int pin10 = 10;
int pin11 = 11;

// analog input pins
int pinA0 = A0;
int pinA1 = A1;
int pinA2 = A2;
int pinA3 = A3;
int pinA4 = A4;
int pinA5 = A5;

// create places to hold values
int pinA0val = 0;
int pinA0midi = 0;
int pinA1val = 0;
int pinA1midi = 0;
int pinA2val = 0;
int pinA2midi = 0;
int pinA3val = 0;
int pinA3midi = 0;
int pinA4val = 0;
int pinA4midi = 0;
int pinA5val = 0;
int pinA5midi = 0;

int pin3val = 0;

int state = 0;

// -----------------------------------------------------------------------------
// This function will be automatically called when a NoteOn is received.
// It must be a void-returning function with the correct parameters,
// see documentation here:
// http://arduinomidilib.fortyseveneffects.com/a00022.html


// NOTE ON ----------------------------------------
void handleNoteOn(byte channel, byte pitch, byte velocity)
{
    // Do whatever you want when a note is pressed.
    //Serial.print("midi note: ");
    //Serial.print(pitch);
    // Try to keep your callbacks short (no delays ect)
    // otherwise it would slow down the loop() and have a bad impact
    // on real-time performance.

  // map the white keys to the top row of pins
  // C1=36  D1=38 E=40 F=41 G=43 A=45

  if (pitch == 36){
        digitalWrite(pin2, HIGH);
  }

  if (pitch == 38){
        digitalWrite(pin4, HIGH);
  }

  if (pitch == 40){
        digitalWrite(pin7, HIGH);
  }

  if (pitch == 41){
        digitalWrite(pin8, HIGH);
  }

  if (pitch == 43){
        digitalWrite(pin12, HIGH);
  }

  if (pitch == 45){
        digitalWrite(pin13, HIGH);
  }


}


// NOTE OFF -----------------------------------
void handleNoteOff(byte channel, byte pitch, byte velocity)
{
    // Do something when the note is released.
    // map the white keys to the top row of pins
          
  if (pitch == 36){
        digitalWrite(pin2, LOW);
  }

  if (pitch == 38){
           digitalWrite(pin4, LOW);  
  }

  if (pitch == 40){
        digitalWrite(pin7, LOW);
  }

  if (pitch == 41){
        digitalWrite(pin8, LOW);
  }

  if (pitch == 43){
           digitalWrite(pin12, LOW);  
  }

  if (pitch == 45){
        digitalWrite(pin13, LOW);
  }
}

// -----------------------------------------------------------------------------

// **TODO** this is probably where I can add MIDI cc -> pwm functionality

void setup()
{
    // Connect the handleNoteOn function to the library, 
    // so it is called upon reception of a NoteOn.
    MIDI.setHandleNoteOn(handleNoteOn);  // Put only the name of the function

    // Do the same for NoteOffs
    MIDI.setHandleNoteOff(handleNoteOff);

    // Initiate MIDI communications, listen channel 1
    MIDI.begin(1);

    // TOP ROW - digital pins
    pinMode(pin2, OUTPUT);
    pinMode(pin4, OUTPUT);
    pinMode(pin7, OUTPUT);
    pinMode(pin8, OUTPUT);
    pinMode(pin12, OUTPUT);
    pinMode(pin13, OUTPUT);

    // MIDDLE ROW - PWM pins
    pinMode(pin3, INPUT);
    pinMode(pin5, OUTPUT);
    pinMode(pin6, OUTPUT);
    pinMode(pin9, OUTPUT);
    pinMode(pin10, OUTPUT);
    pinMode(pin11, OUTPUT);


    // BOTTOM ROW - analog input pins
    pinMode(pinA0, INPUT);
    pinMode(pinA1, INPUT);
    pinMode(pinA2, INPUT);
    pinMode(pinA3, INPUT);
    pinMode(pinA4, INPUT);
    pinMode(pinA5, INPUT);

    //MIDI.begin(1);
    Serial.begin(115200); // opens serial port, sets data rate to ? bps
}

//  Send a three byte midi message  
//  I think this sends a midi message back into the computer
//  this isn't very useful, maybe I should turn this offr
//  or keep it around for sending midi from the modular to the computer

void midiSend(char status, char data1, char data2) {
  Serial.print(status);
  Serial.print(data1);
  Serial.print(data2);
}


void loop()
{
    // read incoming MIDI messages and do stuff
    MIDI.read(1);

    // read values of pins and generate MIDI messages
    // more info about midi cc bytes here:
    // https://www.midi.org/specifications/item/table-3-control-change-messages-data-bytes-2

    // I should really figure out how to do a function here
    // until then, I'll just copy-paste like a hack :-)

    // I did this to experiment with sending voltages to arduino and converting to midi cc
    // may need to deactivate for now
    pinA0val = analogRead(pinA0);   // read the input pin
    pinA0midi = map(pinA0val, 0, 1023, 0, 127);
    // format:
    // midiSend(channel, control number, value)
    midiSend(0xB1, 0x72, pinA0midi);


    pinA1val = analogRead(pinA1);   // read the input pin
    pinA1midi = map(pinA1val, 0, 1023, 0, 127);
    midiSend(0xB0, 0x73, pinA1midi);


    pinA3val = analogRead(A3);
    if (pinA3val > 1000 & state != 1) {
      noteOn(0x90, 0x1E, 0x45); // 0x1E = midi note 30
      state = 1;
    }
    if (pinA3val < 0x1E & state != 0) {
      noteOn(0x90, 0x1E, 0x00);
      state = 0;
    }


    pinA4val = analogRead(A4);
    if (pinA4val > 1000 & state != 1) {
      noteOn(0x90, 0x1F, 0x45); // 0x1F = midi note 31
      state = 1;
    }
    if (pinA4val < 1000 & state != 0) {
      noteOn(0x90, 0x1F, 0x00);
      state = 0;
    }

    pinA5val = analogRead(A5);
    if (pinA5val > 1000 & state != 1) {
      noteOn(0x90, 0x20, 0x45); //
      state = 1;
    }
    if (pinA5val < 1000 & state != 0) {
      noteOn(0x90, 0x20, 0x00); // 0x20 = midi note 32
      state = 0;
    }
    // experimenting with generating voltages from MIDI ccs
    
//    noteOn(0x90, note, 0x45);
//    delay(100);
//    //Note on channel 1 (0x90), some note value (note), silent velocity (0x00):
//    noteOn(0x90, note, 0x00);
//    delay(100);


   
    

    // There is no need to check if there are messages incoming
    // if they are bound to a Callback function.
    // The attached method will be called automatically
    // when the corresponding message has been received.
}


void noteOn(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}
