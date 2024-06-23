#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

// DHT sensor setup
#define DHTPIN1 7        // Pin connected to the first DHT sensor
//#define DHTPIN2 6        // Pin connected to the second DHT sensor
#define DHTTYPE DHT11    // Type of DHT sensor: DHT11

// MQ135 sensor setup
#define MQ135_ANALOG_PIN A0 // Analog pin connected to MQ135 sensor
#define MQ135_DIGITAL_PIN 2 // Digital pin connected to MQ135 sensor

// Initialize DHT sensors
DHT dht1(DHTPIN1, DHTTYPE);
//DHT dht2(DHTPIN2, DHTTYPE);

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 4); // I2C address 0x27, 16 columns, 4 rows

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("DHT11 Dual Sensor and MQ135 Gas Sensor Reading!");

  // Initialize the DHT sensors
  dht1.begin();
  //dht2.begin();

  // Initialize the LCD
  lcd.init();
  lcd.backlight();

  // Initialize the MQ135 digital pin as input
  pinMode(MQ135_DIGITAL_PIN, INPUT);
}

void loop() {
  // Wait a few seconds between measurements
  delay(2000);

  // Read humidity from both DHT sensors
  float humidity1 = dht1.readHumidity();
 // float humidity2 = dht2.readHumidity();

  // Read temperature in Celsius from both DHT sensors
  float tempC1 = dht1.readTemperature();
  //float tempC2 = dht2.readTemperature();

  // Read temperature in Fahrenheit from both DHT sensors
  float tempF1 = dht1.readTemperature(true);
  //float tempF2 = dht2.readTemperature(true);

  // Check if any reads failed for either sensor
  if (isnan(humidity1) || isnan(tempC1) || isnan(tempF1) ){
      //isnan(humidity2) || isnan(tempC2) || isnan(tempF2)) {
    Serial.println("Failed to read from one or both DHT sensors!");
    return;
  }

  // Compute average humidity and temperature
  float avgHumidity = humidity1;
  float avgTempC = tempC1;
  float avgTempF = tempF1;

  // Compute average heat index in Fahrenheit
  float heatIndexF1 = dht1.computeHeatIndex(tempF1, humidity1);
  //float heatIndexF2 = dht2.computeHeatIndex(tempF2, humidity2);
  float avgHeatIndexF = heatIndexF1;

  // Compute average heat index in Celsius
  float heatIndexC1 = dht1.computeHeatIndex(tempC1, humidity1, false);
  //float heatIndexC2 = dht2.computeHeatIndex(tempC2, humidity2, false);
  float avgHeatIndexC = heatIndexC1;

  // Read the MQ135 sensor values
  int mq135AnalogValue = analogRead(MQ135_ANALOG_PIN);
  int mq135DigitalValue = digitalRead(MQ135_DIGITAL_PIN);

  // Convert the analog value to a gas concentration value
  // For simplicity, we'll just map the raw analog value
  // Actual conversion will depend on calibration and specific gas
  float co2Level = map(mq135AnalogValue, 0, 1023, 400, 10000); // CO2 in ppm (example range)
  float ammoniaLevel = map(mq135AnalogValue, 0, 1023, 0, 500); // Ammonia in ppm (example range)
  float benzeneLevel = map(mq135AnalogValue, 0, 1023, 0, 200); // Benzene in ppm (example range)

  // Display DHT11 average readings on the LCD
  lcd.clear();
  lcd.setCursor(0, 0); // Line 1
  lcd.print("A.Humi: ");
  lcd.print(avgHumidity);
  lcd.print(" %");

  lcd.setCursor(0, 1); // Line 2
  lcd.print("A.Temp: ");
  lcd.print(avgTempC);
  lcd.print(" C");

  // Delay to allow DHT11 readings to be visible
  delay(4000);

  // Display MQ135 gas sensor readings on the LCD
  lcd.clear();
  lcd.setCursor(0, 0); // Line 1
  lcd.print("CO2: ");
  lcd.print(co2Level);
  lcd.print(" ppm");

  lcd.setCursor(0, 1); // Line 2
  lcd.print("NH3: ");
  lcd.print(ammoniaLevel);
  lcd.print(" ppm");

  lcd.setCursor(-4, 2); // Line 3
  lcd.print("Benz: ");
  lcd.print(benzeneLevel);
  lcd.print(" ppm");

  lcd.setCursor(-4, 3); // Line 4
  lcd.print("Analog: ");
  lcd.print(mq135AnalogValue);

  // Print gas sensor values to Serial Monitor
  Serial.print("MQ135 Analog Value: ");
  Serial.print(mq135AnalogValue);
  Serial.print("\tCO2 Level: ");
  Serial.print(co2Level);
  Serial.print(" ppm\tAmmonia Level: ");
  Serial.print(ammoniaLevel);
  Serial.print(" ppm\tBenzene Level: ");
  Serial.print(benzeneLevel);
  Serial.println(" ppm");

  // Delay to allow MQ135 readings to be visible
  delay(5000);
}
