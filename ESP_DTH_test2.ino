/**
 * Example for reading temperature and humidity
 * using the DHT22 and ESP8266
 *
 * Copyright (c) 2016 Losant IoT. All rights reserved.
 * https://www.losant.com
 */

#include "DHT.h"
//#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
 #include <ESP8266WiFi.h> // Enables the ESP8266 to connect to the local network (via WiFi)
 #include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker


#define DHTPIN 14     // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors

// Include the UI lib
//#include "OLEDDisplayUi.h"

//SSD1306Wire  display(0x3c, D2, D1);


//   Wire.begin ( 4 ,   0 ) ;            // set I2C pins [SDA = GPIO4 (D2), SCL = GPIO0 (D3)], default clock is 100kHz
 

DHT dht(DHTPIN, DHTTYPE);



// const int ledPin = 0; // This code uses the built-in led for visual feedback that the button has been pressed
// const int buttonPin = 13; // Connect your button to pin #13

 // WiFi
 // Make sure to update this for your own WiFi network!
 const char* ssid = "********";
 const char* wifi_password = "********";

 // MQTT
 // Make sure to update this for your own MQTT Broker!
 const char* mqtt_server = "192.168.1.231";
  const char* mqtt_topic_t = "temperatura/ESP3";
 const char* mqtt_topic_u = "umidita/ESP3";
 // The client id identifies the ESP8266 device. Think of it a bit like a hostname (Or just a name, like Greg).
 const char* clientID = "esp8266_3";

 int i=0;

 // Initialise the Pushbutton Bouncer object
 //Bounce bouncer = Bounce();

 // Initialise the WiFi and MQTT Client objects
 WiFiClient wifiClient;
 PubSubClient client(mqtt_server, 1883, wifiClient); // 1883 is the listener port for the Broker


void setup() {
  Serial.begin(9600);
  Serial.setTimeout(2000);
  delay ( 1000 ) ;

   Serial.print("Connecting to ");
   Serial.println(ssid);

  dht.begin();

  // Wait for serial to initialize.
  while(!Serial) { }

  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  Serial.println("Running DHT!");
  Serial.println("-------------------------------------");


   // Connect to the WiFi
   WiFi.begin(ssid, wifi_password);

   // Wait until the connection has been confirmed before continuing
   while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
 }

   // Debugging - Output the IP Address of the ESP8266
   Serial.println("WiFi connected");
   Serial.print("IP address: ");
   Serial.println(WiFi.localIP());

   // Connect to MQTT Broker
   // client.connect returns a boolean value to let us know if the connection was successful.
   if (client.connect(clientID)) {
     Serial.println("Connected to MQTT Broker!");
   }
   else {
     Serial.println("Connection to MQTT Broker failed...");
   }
   

}

int timeSinceLastRead = 0;
void loop() {

  // Report every 2 seconds.
  if(timeSinceLastRead > 2000) {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      timeSinceLastRead = 0;
      return;
    }

    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print(" *C ");
    Serial.print(hif);
    Serial.println(" *F");

    timeSinceLastRead = 0;

    client.publish(mqtt_topic_t, String(t).c_str());
    client.publish(mqtt_topic_u, String(h).c_str());
    delay(1000); // This delay ensures that client.publish doesn't clash with the client.connect call

  }
  delay(100);
  timeSinceLastRead += 100;

  
}
