#define BLYNK_TEMPLATE_ID "TMPL3vv_4dy5R"
#define BLYNK_TEMPLATE_NAME "test"
#define BLYNK_AUTH_TOKEN "y_Kiuc-w0r1ihY7s-TvEyeu9kHtiK6wq"
#define BLYNK_PRINT Serial


#include <LiquidCrystal.h>
#include <WiFi.h>
#include "MQ7.h"
#include "MQ135.h"
#include <GP2Y1010AU0F.h>
#include <BlynkSimpleEsp32.h>

// For LED
LiquidCrystal lcd(5, 18, 19, 21, 22, 23); // RS, E, D4, D5, D6, D7
const int lcdContrast = 13;

// For Dust Sensor
int measurePin = 35;   // Connect dust sensor analog measure pin to Arduino A0 pin
int ledPin = 4;        // Connect dust sensor LED pin to Arduino pin 2
GP2Y1010AU0F dustSensor(ledPin, measurePin); // Construct dust sensor global object
float dustDensity = 0;

// For MQ7 Sensor
MQ7 mq7(32, 5.0);
int mq7Value = 0;

// For MQ135 Sensor
MQ135 mq135(33);
int mq135Value = 0;

// For WiFi
const char ssid[] = "Heo";     // Your WiFi SSID
const char password[] = "0123456789"; // Your WiFi password
const char auth[] = BLYNK_AUTH_TOKEN;


//For pizzo
const int buzzerPin = 12; // Change this to the actual pin where your buzzer is connected


void setup() {
  Serial.begin(9600);
  dustSensor.begin();
  lcd.begin(16, 2);
  pinMode(lcdContrast, OUTPUT);
  analogWrite(lcdContrast, 85);
  playOpening();
  welcome();
  delay(500);
  connectToWiFi();
  Blynk.begin(auth, ssid, password);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(String(ssid) + " Wifi Lost");
    delay(2000); // Adjust the delay to control how often you check the WiFi status
    connectToWiFi(); // Attempt to reconnect
  } else {
    // Example: Reading sensors and displaying data
    float dustDensity = dustSensor.read();
    int mq7Value = mq7.getPPM();
    int mq135Value = mq135.getPPM();
    sendBlink(mq7Value,mq135Value,dustDensity);
    // Display sensor data on the LCD
    lcd.clear();
    printLed(0, 0, "Dust Density:");
    printLed(0, 1, String(dustDensity, 2) + " ug/m3");
    delay(2000);
    lcd.clear();
    printLed(0, 0, "MQ7: " + String(mq7Value) + " ppm");
    printLed(0, 1, "MQ135: " + String(mq135Value) + " ppm");

    Serial.println("Dust Density: " + String(dustDensity) + " ug/m3");
    Serial.println("MQ7: " + String(mq7Value) + " ppm");
    Serial.println("MQ135: " + String(mq135Value) + " ppm");


    Blynk.run();


    delay(2000); // Adjust the delay between sensor readings

  }

}

void printLed(int col, int row, String str) {
  lcd.setCursor(col, row);
  if (str.length() <= 16) {
    lcd.print(str); // Print the text as is if it's 16 characters or less
  } else {
    // Scroll the text for strings longer than 16 characters
    int textLength = str.length();
    for (int i = 0; i <= textLength - 16; i++) {
      lcd.clear(); // Clear the LCD screen before printing the next portion
      lcd.setCursor(0, row);
      lcd.print(str.substring(i, i + 16));
      delay(500); // Adjust the delay to control the scrolling speed
    }
  }
}

void welcome() {
  String message = "Air Pollution Monitoring System";
  int messageLength = message.length();

  for (int i = 0; i <= messageLength - 16; i++) {
    printLed(0, 0, "Welcome to ...");
    printLed(0, 1, message.substring(i, i + 16));
    delay(500);  // Adjust the delay to control the scrolling speed
    lcd.clear();
  }
}

void animateWiFi() {
  const char wifiSymbols[] = { // Define WiFi animation symbols
    ' ', '.', ':', '|', ':', '.', ' '
  };

  static int frame = 0; // Variable to track the frame of the animation
  lcd.setCursor(15, 1); // Position the cursor at the end of the second row
  lcd.print(wifiSymbols[frame]);

  frame = (frame + 1) % (sizeof(wifiSymbols) / sizeof(wifiSymbols[0])); // Cycle through the animation frames
  delay(200); // Adjust the delay to control the animation speed
  lcd.setCursor(15, 1);
  lcd.print(" "); // Clear the previous symbol
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  lcd.clear();
  lcd.print("Connecting WiFi");
  lcd.setCursor(0, 1);
  lcd.print("Please wait...");
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    // delay(100);
    Serial.print(".");
    animateWiFi(); // Call the WiFi animation function while connecting
  }
  delay(1000);
  // Turn off the WiFi animation after a successful connection
  lcd.setCursor(15, 1);
  lcd.print(" "); // Clear the animation symbol

  Serial.println("\nConnected to WiFi");
  Serial.println("");
  Serial.println("╋╋╋╋╋╋╋┏━┓╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋┏┓╋╋╋╋╋┏┓\n╋╋╋╋╋╋╋┃┏┛╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋┏┛┗┓╋╋╋╋┃┃\n┏┓┏┓┏┳┳┛┗┳┓┏━━┳━━┳━┓┏━┓┏━━┳━┻┓┏╋━━┳━┛┃\n┃┗┛┗┛┣╋┓┏╋┫┃┏━┫┏┓┃┏┓┫┏┓┫┃━┫┏━┫┃┃┃━┫┏┓┃\n┗┓┏┓┏┫┃┃┃┃┃┃┗━┫┗┛┃┃┃┃┃┃┃┃━┫┗━┫┗┫┃━┫┗┛┃\n╋┗┛┗┛┗┛┗┛┗┛┗━━┻━━┻┛┗┻┛┗┻━━┻━━┻━┻━━┻━━┛\n");
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  // String conn = String(ssid) + " Connected";
  printLed(0, 0, String(ssid) + " Connected");
  printLed(0, 1, "Successfully ...");
  delay(4000);
}


void playOpening() {
  // Define the melody notes and durations with their corresponding frequencies
  int melody[] = {
    659, 659, 0, 659, 0, 523, 659, 0,
    784, 0, 0, 0, 392, 0, 0, 0,
    523, 0, 0, 392, 0, 0, 329, 0,
    0, 440, 0, 493, 0, 440, 392
  };

  int noteDurations[] = {
    8, 8, 4, 8, 4, 4, 8, 4,
    4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 8, 4,
    4, 8, 4, 8, 4, 8, 4
  };

  // Set the buzzer pin as an output
  pinMode(buzzerPin, OUTPUT);

  // Play the melody
  for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); i++) {
    int noteDuration = 500 / noteDurations[i];
    tone(buzzerPin, melody[i], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(buzzerPin);
  }

  // Wait before playing the melody again
  delay(2000);
}


void sendBlink(int mq7o,int mq135o,int dustdeno){
  Blynk.virtualWrite(V0, mq7o);
  Blynk.virtualWrite(V1, mq135o);
  Blynk.virtualWrite(V2, dustdeno);
}
