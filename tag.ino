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

/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 0;
/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7,8);
/**********************************************************/
                                                                           // Topology
byte myAddress = 0x55;              // Radio pipe addresses for the 2 nodes to communicate.
byte receiverAddress [] = "receiver";
// Role management: Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.  

typedef enum { sendingID = 1, sleeping } state_e;                 // The various roles supported by this sketch

state_e state = sendingID;                                       // The role of the current running sketch
unsigned long sleepTimer;
byte myID;
bool inBuilding;
unsigned long delayTimer;

void buildingOrBus(){
  inBuilding = !inBuilding;
  if(inBuilding) Serial.println("Entered a Building/Bus");
    else Serial.println("Went out of Building/Bus");
}

void setup(){
  attachInterrupt(digitalPinToInterrupt(3), buildingOrBus, RISING);
  Serial.begin(115200);
  pinMode(5,OUTPUT);
  myID = 50;
  // Setup and configure radio
  inBuilding = false;
  delayTimer = millis();
  radio.begin();
  radio.openWritingPipe(myAddress);        // Both radios listen on the same pipes by default, but opposite addresses
  radio.openReadingPipe(1,receiverAddress);      // Open a reading pipe on address 0, pipe 1
}

void loop(void) {
  digitalWrite(5,inBuilding);
  if(state == sleeping){
    if(millis()-sleepTimer > 10000)
      state = sendingID;
  }
  if(inBuilding)
     state = sleeping;
  
  if(state == sendingID){                               // Radio is in ping mode
    byte gotByte;                                           // Initialize a variable for the incoming response
    bool sentSomething = 0;
    radio.stopListening();                                  // First, stop listening so we can talk.      
    Serial.print(F("Now sending "));                         // Use a simple byte counter as payload
    Serial.println(myID);
    
    unsigned long time = millis();                          // Record the current microsecond count   
    while((millis()-time)<10000){
      if(!radio.write(&myID,1)) continue;
      else {
        sentSomething = 1;
        break;
      }
    }
                                                            
    if ( sentSomething ){                         // Send the counter variable to the other radio 
         Serial.print(F("Sent ID : "));
         Serial.println(myID);
         sleepTimer  = millis();
         state = sleeping;
        }
    }
}
