#include <SoftwareSerial.h>
#define RX 4
#define TX 5
#define TAG_COUNT 5
#define RECEIVER_ALERT_TIMEOUT 15   //in seconds
String AP = "Siddiq Netw0rk";       // CHANGE ME
String PASS = "milanfan1995";       // CHANGE ME
String API = "147LROQ302VWEG33";
String HOST = "api.thingspeak.com";
String PORT = "80";
String field = "field1";
int countTrueCommand;
int countTimeCommand; 
boolean found = false;
unsigned long timers [TAG_COUNT];
bool alerts[TAG_COUNT];
String data;
SoftwareSerial esp8266(RX,TX); 

void setup() {
  for(int i=0;i<TAG_COUNT;i++){
    timers[i] = -1;
    alerts[i] = false;
  }
  timers[2] = millis();
  Serial.begin(9600);
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}
void loop() {
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 for(int i=0;i<TAG_COUNT;i++){
   if(timers[i] == -1) continue;
   if(millis() - timers[i] > RECEIVER_ALERT_TIMEOUT*1000){
     alerts[i] = true;
     sendCommand("AT+CIPSEND=0," +String(data.length()+4),4,">");
     data = "GET /update?api_key="+ API +"&"+ field +"="+String(i+50);
     esp8266.println(data);
     delay(1500);
     countTrueCommand++;
     //Serial.print("Alert on tag # ");
     //Serial.println(i+50);
   }
 }
 sendCommand("AT+CIPCLOSE=0",5,"OK");
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("SUCCESS!");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
