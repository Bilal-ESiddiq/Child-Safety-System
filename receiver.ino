/*
   Dec 2014 - TMRh20 - Updated
   Derived from examples by J. Coliz <maniacbug@ymail.com>
*/
/**
 * Example for efficient call-response using ack-payloads 
 * 
 * This example continues to make use of all the normal functionality of the radios including 
 * the auto-ack and auto-retry features, but allows ack-payloads to be written optionlly as well. 
 * This allows very fast call-response communication, with the responding radio never having to 
 * switch out of Primary Receiver mode to send back a payload, but having the option to switch to 
 * primary transmitter if wanting to initiate communication instead of respond to a commmunication. 
 */

#include <SPI.h>
#include "RF24.h"
#define TAG_COUNT 5
/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 1;
/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7,8);
unsigned long timers [TAG_COUNT];
bool alerts[TAG_COUNT];
/**********************************************************/
                                                                           // Topology
byte myAddress = 0x55;              // Radio pipe addresses for the 2 nodes to communicate.
byte receiverAddress [] = "receiver";
// Role management: Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.  

typedef enum { receivingID = 1, checkingMem } state_e;                 // The various roles supported by this sketch

state_e state = receivingID;

void setup(){
  for(int i=0;i<TAG_COUNT;i++){
    timers[i] = -1;
    alerts[i] = false;
  }
  timers[2] = millis();
  Serial.begin(115200);
  Serial.println("Serial Started...");
  // Setup and configure radio
  radio.begin();
  radio.openWritingPipe(receiverAddress);        // Both radios listen on the same pipes by default, but opposite addresses
  radio.openReadingPipe(1,myAddress);      // Open a reading pipe on address 0, pipe 1
}

void loop(void) {
  if(state == checkingMem){
    Serial.println("Checking Memory...");
      for(int i=0;i<TAG_COUNT;i++){
        if(timers[i] == -1) continue;
        if(millis() - timers[i] > 3000){
          alerts[i] = true;
          Serial.print("Alert on tag # ");
          Serial.println(i+50);
        }
      }
     state = receivingID;
    }
/****************** Ping Out Role ***************************/
//Serial.println(radio.isChipConnected());
  if(state == receivingID){
    Serial.println("Waiting for ID...");
    byte pipeNo, gotByte;                          
    while(radio.available(&pipeNo)){
      radio.read( &gotByte, 1 );                   
      if(!(gotByte < 50 || gotByte > TAG_COUNT + 50)){
        Serial.print(F("Received ID : "));
        Serial.println(gotByte);
        alerts[gotByte-50] = false;
        timers[gotByte-50] = millis();
        state = checkingMem;
      }
   }
  }
}
