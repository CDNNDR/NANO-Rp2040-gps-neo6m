#include <TinyGPS++.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <Arduino_LSM6DSOX.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>



//float LT;
//float LG;
float Speed; 
TinyGPSPlus gps;


const char* ssid = "teamet";   //Vodafone-A67777465     
const char* pass = "CMSA.wifi.2019";  //e762mMLnq3r2RbCY
WiFiSSLClient client;

// Libreries to get time for MQTT message
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Libreries for MQTT comunication
WiFiClient mqttWifiClient;
PubSubClient mqttClient(mqttWifiClient);

// Values for MQTT broker
const char *MQTT_HOST = "mqtt.datacake.co";
const int MQTT_PORT_INSECURE = 1883;
const int MQTT_PORT = 8883;
const char *MQTT_USER = "1044b4b86f3483781bcc22968e0856eb1de20090";
const char *MQTT_PASS = "1044b4b86f3483781bcc22968e0856eb1de20090";

#define mqtt_topic "dtck/test_gps_wifi/6dafecf5-2401-4dbc-960b-f21de83d104c/+"
#define mqtt_pos_pub "dtck-pub/test_gps_wifi/6dafecf5-2401-4dbc-960b-f21de83d104c/POSITION"
#define mqtt_sp_pub "dtck-pub/test_gps_wifi/6dafecf5-2401-4dbc-960b-f21de83d104c/SPEED"
//#define mqtt_hm_pub  "dtck-pub/ambulatorio_corvettob/82f08c78-4abf-4a9f-9b92-149d77448972/HUMIDITY"



void setup(){
  
  Serial.begin(9600);
  Serial1.begin(9600);
  WiFi.begin(ssid, pass);
if (WiFi.status() == WL_CONNECTED) 
{
  Serial.print("connesso a: ");
  Serial.println(ssid);
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  //se wifi connesso allora connetti al server mqtt

 mqttClient.setServer(MQTT_HOST, MQTT_PORT_INSECURE);
   while (!mqttClient.connected()) {
        Serial.print("MQTT connecting...");
        // MQTT Hostname should be empty for Datacake
        if (mqttClient.connect("", MQTT_USER, MQTT_PASS)) {
            Serial.println("connected");
            mqttClient.subscribe(mqtt_topic);
        } else {
            Serial.print("failed, status code =");
            Serial.print(mqttClient.state());
            Serial.println("try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);

        }
   }
}
}

void loop(){
  
  while (Serial1.available() > 0){
    gps.encode(Serial1.read());
   if (gps.location.isUpdated()){
      
      // Latitude in degrees (double)
      Serial.print("Latitude= "); 
      Serial.print(gps.location.lat(), 6); 
           
      // Longitude in degrees (double)
      Serial.print(" Longitude= "); 
      Serial.println(gps.location.lng(), 6); 

      // Speed in kilometers per hour (double)
      Serial.print("Speed in km/h = "); 
      Serial.println(gps.speed.kmph()); 
         }
 }

float LT = gps.location.lat();
float LG = gps.location.lng();
float Speed = gps.speed.kmph();
char mqtt_payload[50] = "";
snprintf (mqtt_payload, 50, "%lf,%lf", LT, LG);
Serial.println(mqtt_payload);
Serial.println(Speed);

if (Speed <= 0.15){
Serial.println("no sattelites found, back to setup");
delay(10000);
loop();
}

else{
sendmqttdata(mqtt_payload, Speed);
delay(500);


}

delay(120000);
phoenix();
}

void sendmqttdata(char* mqtt_payload, float Speed){
 
//publish position
mqttClient.publish(mqtt_pos_pub, String(mqtt_payload).c_str(), true);

//publish speed
mqttClient.publish(mqtt_sp_pub, String(Speed).c_str(), true);

Serial.print("mqtt done");

}


void phoenix(){

  digitalWrite( NINA_RESETN, LOW );  // Reset NINA module
            delay(500);  // just making sure we get a clean pulse at the NINA_RESETN pin
            NVIC_SystemReset();  // Reset RP2040
  
 }
