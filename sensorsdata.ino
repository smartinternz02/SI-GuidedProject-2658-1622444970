//integration of sensor for sending data of   *dht*ultrasonic
//we need to control exhaust fans automatically and manually
//lights and water pump using mobile and web application
//the ammonia gas and fire sensors values are taken form  python code randomly
#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"  
#define DHTPIN 4   
#define DHTTYPE DHT11   
DHT dht (DHTPIN, DHTTYPE);

float temperature;
int humidity;
String command;
String data="";

void callback(char* topic, byte* payload, unsigned int payloadLength);

// CHANGE TO YOUR WIFI CREDENTIALS
const char* ssid = "Praveen reddy";//your wifi ssid
const char* password = "8013121408";//your password

// CHANGE TO YOUR DEVICE CREDENTIALS AS PER IN IBM BLUMIX
#define ORG "ujiha4"
#define DEVICE_TYPE "Esp32"
#define DEVICE_ID "12345"
#define TOKEN "12345678" //  Authentication Token OF THE DEVICE

//  PIN DECLARATIONS 

#define led1 2
#define pump 36
#define exhaust 39


//-------- Customise the above values --------
const char publishTopic[] = "iot-2/evt/Data/fmt/json";
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/cmd/home/fmt/String";// cmd  REPRESENT command type AND COMMAND IS TEST OF FORMAT STRING
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;


WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

int publishInterval = 5000; // 30 seconds
long lastPublishMillis;
void publishData();
int echo=12;
int trig=5;
void setup() {
  
  Serial.begin(115200);
  Serial.println();

  pinMode(led1,OUTPUT);
 
  dht.begin(); 
  wifiConnect();
  mqttConnect();
 
 pinMode(echo,INPUT);
 pinMode(trig ,OUTPUT); 

 
}

void loop() {


 if (millis() - lastPublishMillis > publishInterval)
  {
    publishData();
    lastPublishMillis = millis();
  }
  
  if (!client.loop()) {
    mqttConnect();
  }
}

void wifiConnect() {
  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("WiFi connected, IP address: "); 
  Serial.println(WiFi.localIP());
}

void mqttConnect() {
  if (!client.connected()) 
  {
    Serial.print("Reconnecting MQTT client to "); Serial.println(server);
    while (!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
    
    initManagedDevice();
    Serial.println();
  }
}

void initManagedDevice() {
  if (client.subscribe(topic)) {
   
    Serial.println("subscribe to cmd OK");
  } else {
    Serial.println("subscribe to cmd FAILED");
  }
}

void callback(char* topic, byte* payload, unsigned int payloadLength) {
  
  Serial.print("callback invoked for topic: ");
  Serial.println(topic);

  for (int i = 0; i < payloadLength; i++) {
    
    command+= (char)payload[i];
  }
  
  Serial.print("data: "+ command);
  control_func();
  
  command= "";
}

void control_func()
{
  
   
  if(command== "lightoff")
 {

 digitalWrite(led1,LOW);
     Serial.println(".......lights are off..........");
    
  }
  else if(command=="pumpon")
  {
    digitalWrite(pump,HIGH);
  }
  else if (command=="pumpoff")
  {
    digitalWrite(pump,LOW);
    
  }
  else if (command=="exhauston"){
    digitalWrite(exhaust,HIGH);
    
  }
   else if (command=="exhaustoff"){
    digitalWrite(exhaust,LOW);
   }
  else if(command== "lighton")
  {
     digitalWrite(led1,HIGH);
     Serial.println(".......lights are on..........");

  }
else if(temperature>=40){
  digitalWrite(exhaust,HIGH);
}
else if(temperature<40){
  digitalWrite(exhaust,LOW);
}
   
  else
  {
    Serial.println("......no commands have been subscribed..........");
   
    }
}

void publishData() 
{
  humidity = dht.readTemperature();
  temperature = dht.readHumidity();
    digitalWrite(trig,HIGH);
  delay(1000);
  digitalWrite(trig,LOW);
  int duration=pulseIn(echo,HIGH);
int distance=duration*0.0343/2;

  String payload = "{\"d\":{\"temperature\":";
  payload += temperature;
  payload += ",""\"humidity\":";
  payload +=  humidity;
  payload += ",""\"distance\":";
  payload +=  distance;
  payload += "}}";



  Serial.print("\n");
  Serial.print("Sending payload: "); Serial.println(payload);

  if (client.publish(publishTopic, (char*) payload.c_str())) {
    Serial.println("Publish OK");
  } else {
    Serial.println("Publish FAILED");
  }
}
