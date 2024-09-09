
#define BLYNK_TEMPLATE_ID "TMPL65y8A3cRI"
#define BLYNK_TEMPLATE_NAME "Trial01"
#define BLYNK_AUTH_TOKEN "VwOzQKvwza3waItLJ4PgEHp4Xi8o6CwI"

// this is for the OLED........
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//OLED........................................

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>  
#include <BlynkSimpleEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN; // Token for the Blynk Template
char ssid[] = "mark_101";  // Wifi username
char pass[] = "donnamark123";  // Wifi password

// initializing the pins in the ESP8266
const int sensorPin = A0;
const int GLED = D0;
const int Buzz = D3;
const int BLED = D4;
const int YLED = D5;
const int RLED = D6;
const int echoPin = D7;
const int trigPin = D8;

// variables B and C are the max values for NORMAL AND WARNING status respectively
float B = 6.00;
float C = 9.00;

// variables need in gettting the value of the distance, filtering it and sending to the blynk
float distance;
float duration;
const float alpha = 0.1; //smoothing factor
float smoothedValue = 0;
float actualReading = 0;

// This function creates the timer object. It's part of Blynk library 
BlynkTimer timer;

// when the device was disconnected and is reconnected again, this function updates the latest values of the virtual pins
BLYNK_CONNECTED(){
  Blynk.syncVirtual(V0); // display of water level
  Blynk.syncVirtual(V1); // for the level H widget
  Blynk.syncVirtual(V2); // for the livestream widget
  Blynk.syncVirtual(V3); // for the status caption
  Blynk.syncVirtual(V4); // for the LED widget
  Blynk.syncAll();
}

// Warning Buzzer that activates once every 1 second.
void warningBuzzer(){
  digitalWrite(Buzz, HIGH);
  delay(1000);
  digitalWrite(Buzz, LOW);
  delay(1000);
}

// the virtual LED on the blynk app activates once every 20 milliseconds
void blinkVirtualLED(){
  Blynk.virtualWrite(V4, 0);
  delay(20);
  Blynk.virtualWrite(V4, 255);  
}

// the function used to set the different water level conditions
void myTimerNormal(){
  if(actualReading < B){ // condition when the water level is below 6 cm (NORMAL STATUS)
    // turns the green LED on and the other LEDs and BUZZER off
    digitalWrite(GLED, HIGH);
    digitalWrite(YLED, LOW);
    digitalWrite(RLED, LOW);
    digitalWrite(Buzz, LOW);    
    // displays "NORMAL" text on the OLED
    display.setCursor(50, 24);
    display.print("NORMAL");
    display.display();
    // sets the label widget color to green and displays the water level value
    Blynk.setProperty(V0, "color", "#02710D");
    Blynk.virtualWrite(V0, actualReading);
    // sets the level H widget color to green and shows the water level
    Blynk.setProperty(V1, "color", "#02710D");
    Blynk.virtualWrite(V1, actualReading);
    // displays the normal river GIF 
    Blynk.setProperty(V2, "url", "https://i.gifer.com/6jDk.gif");
    // sets the label widget color to green and display the "NORMAL" text
    Blynk.setProperty(V3, "color", "#02710D");
    Blynk.virtualWrite(V3, "NORMAL");
    // sets the virtual LED color to green and turns it "ON"
    Blynk.setProperty(V4, "color", "#02710D");
    Blynk.virtualWrite(V4, 255);
    // updates all the datastreams
    Blynk.syncAll();  
  }
  else if(actualReading > B && actualReading < C){ // condition when the water level is between 6 and 9 cm (WARNING STATUS)  
    // turns the yellow LED and BUZZER on and the other LEDs off
    digitalWrite(GLED, LOW);
    digitalWrite(YLED, HIGH);
    digitalWrite(RLED, LOW);    
    warningBuzzer();
    // displays "WARNING" text on the OLED 
    display.display(); 
    display.setCursor(50, 24);
    display.print("WARNING!!!");
    display.display();
    // sets the label widget color to orange and displays the water level value
    Blynk.setProperty(V0, "color", "#D5D405");
    Blynk.virtualWrite(V0, actualReading);
    // sets the level H widget color to orange and shows the water level
    Blynk.setProperty(V1, "color", "#D5D405");
    Blynk.virtualWrite(V1, actualReading);
    // displays the flooded river GIF    
    Blynk.setProperty(V2, "url", "https://s.yimg.com/ny/api/res/1.2/gPvbpTcuwXhYzioXYSstAw--/YXBwaWQ9aGlnaGxhbmRlcjt3PTk2MDtoPTU0MTtjZj13ZWJw/https://media.zenfs.com/en-us/accuweather_297/94f9c733a2e001bad0fd57b0f7e7d9f9");
    // sets the label widget color to orange and display the "WARNING" text
    Blynk.setProperty(V3, "color", "#D5D405");
    Blynk.virtualWrite(V3, "WARNING");
    // sets the virtual LED color to orange and activates the blinkVirtualLED function
    Blynk.setProperty(V4, "color", "#D5D405");
    blinkVirtualLED();
    // updates all the datastreams
    Blynk.syncAll();      
  }
  else{ // condition when the water level is beyond 9 cm (CRITICAL STATUS) 
    // turns the red LED and BUZZER on and the other LEDs off
    digitalWrite(GLED, LOW);
    digitalWrite(YLED, LOW);
    digitalWrite(RLED, HIGH);
    digitalWrite(Buzz, HIGH);
    // displays "CRITICAL" text on the OLED 
    display.setCursor(50, 24);
    display.print("CRITICAL!");
    display.display(); 
    // sets the label widget color to red and displays the water level value
    Blynk.setProperty(V0, "color", "#BD1002");
    Blynk.virtualWrite(V0, actualReading);
    // sets the level H widget color to red and shows the water level
    Blynk.setProperty(V1, "color", "#BD1002");
    Blynk.virtualWrite(V1, actualReading); 
    // displays the overflooded river GIF   
    Blynk.setProperty(V2, "url", "https://media.zenfs.com/en/video.storyful.com/7741f63f698ec5feef26775450601a7f"); 
    // sets the label widget color to red and display the "CRITICAL" text
    Blynk.setProperty(V3, "color", "#BD1002");
    Blynk.virtualWrite(V3, "CRITICAL"); 
    // sets the virtual LED color to red and activates the blinkVirtualLED function
    Blynk.setProperty(V4, "color", "#BD1002");
    blinkVirtualLED();
    // updates all the datastreams
    Blynk.syncAll();     
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // initializing and establishing connection from esp8266 and the blynk
  Blynk.begin(auth, ssid, pass, "blynk.cloud");
  // initializing the INPUT and OUTPUT pins
  pinMode(GLED, OUTPUT);
  pinMode(YLED, OUTPUT);
  pinMode(BLED, OUTPUT);
  pinMode(RLED, OUTPUT);
  pinMode(Buzz, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  // function which allows the device to send one data per 1 second to prevent the blynk cloud from crashing
  timer.setInterval(1000L, myTimerNormal);
   
  //this is for the OLEd set-up ........
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)){
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(F("Welcome Friends"));
  display.display();
  //OLED set-up .......................  
}

void loop() {
  // runs the blynk application
  Blynk.run(); 
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; 
  // apply 'LOW-PASS FILTER to make the sensor reading stable 
  smoothedValue = alpha * distance + (1 - alpha) * smoothedValue;
  // calibrates the measured water level since the ultrasonic sensor was positioned on top of the water surface
  actualReading = 16 - smoothedValue;
  // runs the timer function which activates all the widget in the blynk application  
  timer.run();
  // prints the water level value on the serial monitor
  Serial.println(actualReading);   
  
  //this is for OLEd display....
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("--JEM IoT Solutions--"));  
  display.setCursor(0, 12);
  display.print(F("Water Level: "));  
  display.setCursor(75, 12);
  display.print(actualReading);  
  display.setCursor(0, 24);
  display.print("Status: ");  
      
  delay(100);   
}
