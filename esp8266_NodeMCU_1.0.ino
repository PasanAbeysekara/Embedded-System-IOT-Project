#include <Arduino.h>
#include <ESP8266WiFi.h> 
#include <Firebase_ESP_Client.h>
#include <SoftwareSerial.h>
#include <DHT.h>

// Include the Firebase and WiFi credentials
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "NOT_FOUND"
#define WIFI_PASSWORD "ED9E10DF"

#define API_KEY "AIzaSyBPs3uqNz7FbSkdTjaYoBGh_156s2WXx4s"

#define DATABASE_URL "https://iot-project-f9f9c-default-rtdb.asia-southeast1.firebasedatabase.app/" 

// Define DHT11 sensor
#define DHTPIN D4 // Define the pin where the DHT11 data pin is connected
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

SoftwareSerial NodeMCU(D2, D3); // RX, TX

void setup(){
  Serial.begin(115200);
  NodeMCU.begin(4800);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Initialize DHT sensor
  dht.begin();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    
    // Reading temperature and humidity from DHT11
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    String occupancy = "PlaceHolder"; // Assuming 1 means occupied and 0 means not occupied

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    if (Firebase.RTDB.setFloat(&fbdo, "DHT_11/Humidity", humidity)){
      Serial.println("PASSED: Humidity");
    } else {
      Serial.println("FAILED: Humidity");
    }

    if (Firebase.RTDB.setFloat(&fbdo, "DHT_11/Temperature", temperature)){
      Serial.println("PASSED: Temperature");
    } else {
      Serial.println("FAILED: Temperature");
    }

    if (Firebase.RTDB.getString(&fbdo, "OccupancyStatus/status")) {
      occupancy = fbdo.stringData();
      Serial.print("Occupancy Status: ");
      Serial.println(occupancy);
    } else {
      Serial.print("Failed to get occupancy status, reason: ");
      Serial.println(fbdo.errorReason());
    }
    
    // Send data to Arduino Uno
    NodeMCU.print("Hum: ");
    NodeMCU.println(humidity);
    NodeMCU.print("Temp: ");
    NodeMCU.println(temperature);
    NodeMCU.print("Occ: ");
    NodeMCU.println(occupancy);
    
    count++;
  }
}
