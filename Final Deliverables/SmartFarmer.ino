#include <WiFi.h>//library for wifi
#include <PubSubClient.h>//library for MQtt 
#include"ArduinoJson.h"
#include "DHT.h"// Library for dht11
#include <HTTPClient.h>//library for HTTP requests
#define DHTPIN 4 // what pin we're connected to
#define DHTTYPE DHT22
#define LED 5

DHT dht(DHTPIN,DHTTYPE);// creating the instance by passing pin and typr of dht connected

String serverName = "http://169.51.204.248:31732/command";//server changed after new cluster
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;
String motor = "{\"Motor\":1}";

//-------credentials of IBM Accounts------

#define ORG "qoznfd"//IBM ORGANIZATION ID
#define DEVICE_TYPE "IOTDeviceIBM"//Device type mentioned in ibm watson IOT Platform
#define DEVICE_ID "ESP32"//Device ID mentioned in ibm watson IOT Platform
#define TOKEN "12345678"//Token

char server[] = ORG ".messaging.internetofthings.ibmcloud.com";// Server Name
char publishTopic[] = "iot-2/evt/Data/fmt/json";// topic name and type of event perform and format in which data to be send
char authMethod[] = "use-token-auth";// authentication method
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;//client id

WiFiClient wifiClient; // creating the instance for wificlient
PubSubClient client(server, 1883, wifiClient); //calling the predefined client id by passing parameter like server id,portand wificredential

float h, t,heat_ind,soil_mois,pH,gas,soil_mois_percent,pH_lev;
int wat_lev;
float npk[3];

void setup()// configuring the ESP32 
{
  Serial.begin(115200);
  dht.begin();
  pinMode(LED,OUTPUT);
  delay(10);
  Serial.println();
  wificonnect();
  mqttconnect();
}

void loop()// Recursive Function
{
  delay(2000);
  Serial.println(motor);
  h = dht.readHumidity();
  t = dht.readTemperature();
  heat_ind = dht.computeHeatIndex(t, h, false);
  wat_lev=random(0,2);
  pH=random(30,91);
  pH_lev=pH/10;
  gas=random(10,1001);
  soil_mois=random(0,1024);
  soil_mois_percent=100-((soil_mois/1023.00)*100);

  Serial.print("temperature in °C: ");
  Serial.println(t);
  Serial.print("Humidity in % : ");
  Serial.println(h);
  Serial.print("Apparent Temperature in °C: ");
  Serial.println(heat_ind);
  Serial.print("Water Level (0 for Low, 1 for High): ");
  Serial.println(wat_lev);
  Serial.print("pH Level : ");
  Serial.println(pH_lev);
  Serial.print("Gas particles in ppm: ");
  Serial.println(gas);
  Serial.print("Soil Moisture in %: ");
  Serial.println(soil_mois_percent);

  Serial.print("NPK values in mg/kg : ");
  for (int i=0;i<3;i++)
  {
    npk[i]=random(0,500);
    Serial.print(npk[i]);
    Serial.print(" ");
  }
  Serial.println();

  PublishData(t, h,heat_ind,wat_lev,pH_lev,gas,soil_mois_percent,npk);
  delay(1000);
  if (!client.loop()) {
    mqttconnect();
  }

  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      String serverPath = serverName  ;
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        if(payload == motor)
        {
          digitalWrite(LED,HIGH);
          Serial.println("Motor is ON");
        }
        else
        {
          digitalWrite(LED,LOW);
          Serial.println("Motor is OFF");
        }
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
  
}


/*.....................................retrieving to Cloud...............................*/

void PublishData(float temp, float humid,float heatind,int waterlev, float pHlev,float gaspart,float soilmoisture,float npk[]) {
  mqttconnect();//function call for connecting to ibm
  /*
     creating the String in in form JSon to update the data to ibm cloud
  */
  String payload = "{\"temperature\":";
  payload += temp;
  payload += "," "\"humidity\":";
  payload += humid;
  payload += "," "\"feelslike\":";
  payload += heatind;
  payload += "," "\"waterlevel\":";
  payload += waterlev;
  payload += "," "\"pHlevel\":";
  payload += pHlev;
  payload += "," "\"gasparticles\":";
  payload += gas;
  payload += "," "\"soilmoisture\":";
  payload += soilmoisture;
  payload += "," "\"nitrogen\":";
  payload += npk[0];
  payload += "," "\"phosphorous\":";
  payload += npk[1];
  payload += "," "\"potassium\":";
  payload += npk[2];
  payload += "}";

  Serial.print("Sending payload: ");
  Serial.println(payload);

  delay(2000);
  if (client.publish(publishTopic, (char*) payload.c_str())) {
    Serial.println("Publish ok");// if it sucessfully upload data on the cloud then it will print publish ok in Serial monitor or else it will print publish failed
  } else {
    Serial.println("Publish failed");
  }
  
}

void mqttconnect() {
  if (!client.connected()) {
    Serial.print("Reconnecting client to ");
    Serial.println(server);
    while (!!!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
     Serial.println();
  }
}

void wificonnect() //function definition for wificonnect
{
  Serial.println();
  Serial.print("Connecting to ");

  WiFi.begin("Wokwi-GUEST", "", 6);//passing the wifi credentials to establish the connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
