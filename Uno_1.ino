#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

SoftwareSerial NodeMCU(3, 2);  // RX, TX

LiquidCrystal_I2C lcd(0x27, 16, 4);  // Address 0x27, 16 columns, 4 rows

void setup() {
  Serial.begin(9600);
  NodeMCU.begin(4800);
  
  lcd.init();          // Initialize LCD
  lcd.backlight();     // Turn on backlight
  lcd.setCursor(0, 0); // Set cursor to beginning of first line
   // Initial message on LCD
}

void loop() {
  if (NodeMCU.available()) {
    // Read data from NodeMCU
    String data = NodeMCU.readStringUntil('\n');
    Serial.println(data);   // Print received data to Serial monitor
    
    // Clear previous content on LCD
    
    
    // Determine which line to print on based on data type
    if (data.startsWith("Hum:")) {
      lcd.setCursor(0, 1);    // Set cursor to beginning of second line
      lcd.print(data);        // Print humidity data on LCD
    } else if (data.startsWith("Temp:")) {
      lcd.setCursor(-4, 2);    // Set cursor to beginning of third line
      lcd.print(data);        // Print temperature data on LCD
    } else if (data.startsWith("Occ:")) {
      lcd.setCursor(-4, 3);    // Set cursor to beginning of fourth line
      lcd.print(data);        // Print occupancy data on LCD
    }
  }
}
