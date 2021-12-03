/******************  LIBRARY SECTION *************************************/
#include <PubSubClient.h>         //https://github.com/knolleary/pubsubclient
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>           
#include <RCSwitch.h>             //https://github.com/sui77/rc-switch
#include <stdlib.h>

/*****************   USER CONFIG  *********************************/
#define USER_SSID                 "<YOUR-SSID>"            //WIFI SSID
#define USER_PASSWORD             "<Your-WIFI-Password"         //WIFI pass
#define USER_MQTT_SERVER          "<Your-MQTT-IP-Address>"          //MQTT broker address
#define USER_MQTT_PORT            1883                    //MQTT Port
#define USER_MQTT_USERNAME        "YOUR_MQTT_LOGIN"       //MQTT login
#define USER_MQTT_PASSWORD        "YOUR_MQTT_PASSWORD"    //MQTT password
#define USER_MQTT_CLIENT_NAME     "RadioTrans"            //used to define MQTT topics, MQTT Client ID, and ArduinoOTA
#define DATA_PIN                  D2                      //Data pin from receiver

/***********************  WIFI AND MQTT SETUP *****************************/
const char* ssid = USER_SSID ;
const char* password = USER_PASSWORD ;
const char* mqtt_server = USER_MQTT_SERVER ;
const int mqtt_port = USER_MQTT_PORT ;
const char *mqtt_user = USER_MQTT_USERNAME ;
const char *mqtt_pass = USER_MQTT_PASSWORD ;
const char *mqtt_client_name = USER_MQTT_CLIENT_NAME ;
/*****************  DECLARATIONS  ****************************************/
WiFiClient espClient;
PubSubClient client(espClient);
RCSwitch mySwitch = RCSwitch();
/*****************  GENERAL VARIABLES  *************************************/
bool boot = true;


void setup_wifi()
{
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.hostname(USER_MQTT_CLIENT_NAME);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  // Loop until we're reconnected
  int retries = 0;
  while (!client.connected()) {
    if (retries < 150)
    {
      Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (client.connect(mqtt_client_name, mqtt_user, mqtt_pass))
      {
        Serial.println("connected");
        // Announcement...
        if (boot == true)
        {
          client.publish(USER_MQTT_CLIENT_NAME"/checkIn", "Rebooted");
          boot = false;
        }
        if (boot == false)
        {
          client.publish(USER_MQTT_CLIENT_NAME"/checkIn", "Reconnected");
        }
      }
      else
      {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        retries++;
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
    else
    {
      ESP.restart();
    }
  }
}
//   ***************************    SETUP    ***************************************
void setup()
{
  Serial.begin(115200);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_STA);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  ArduinoOTA.setHostname(USER_MQTT_CLIENT_NAME);
  ArduinoOTA.begin();
  mySwitch.enableReceive(DATA_PIN);  // Receiver data pin
}

//   ***************************    LOOP    ****************************************
void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop(); //mqtt checking and publishing
  ArduinoOTA.handle(); //arduino remote programming checking.

  if (mySwitch.available()) {
    Serial.print(mySwitch.getReceivedValue());
    char X[12];
    itoa(mySwitch.getReceivedValue(), X, 10);
    client.publish(USER_MQTT_CLIENT_NAME"/received", X);  //publish the incoming remote control packet to the MQTT topic
    mySwitch.resetAvailable();
  }
}
