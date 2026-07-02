#include <FastLED.h>
#include <Arduino.h>

#define NUM_LEDS 147 // Number of LEDs in your strip
#define DATA_PIN 6 // Pin which receives data

CRGB leds[NUM_LEDS];
String inputString = "";  // A string to hold incoming data
boolean stringComplete = false;  // Whether the string is complete
int currentPixel;
char currentMode[20];
int red, green, blue, delayTime;
int rgbArr[][3] = {

};

void setup() {
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS); // Use appropriate LED type
  Serial.begin(115200); // Initialize Serial communication
  inputString.reserve(200); // Reserve some memory for the inputString
  currentPixel = 0;
  Serial.println("Locked and loaded");
}

void parseComand () {
  
}

void loop() {
  if (stringComplete) {
    char mode[20];
    // rework this such that first arg is int for mode and str of 25char for start and parse it differently depending on what mode it is :)
    sscanf(inputString.c_str(), "%d,%d,%d,%d,%19s", &red, &green, &blue, &delayTime, mode);
      // print over serial for debugging, brackets is for ability to collapse this shit 
    {
      Serial.print("Red: ");
      Serial.println(red);
      Serial.print("Green: ");
      Serial.println(green);
      Serial.print("Blue: ");
      Serial.println(blue);
      Serial.print("Delay: ");
      Serial.println(delayTime);
      Serial.print("Mode: ");
      Serial.println(mode);
    }
    strcpy(currentMode, mode);
    // Clear the string
    inputString = "";
    stringComplete = false;
  }

  if (strcmp(currentMode, "solid") == 0) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(red, green, blue);
    }
  }

  // Update LEDs
  FastLED.show();
  if (strcmp(currentMode, "pixelrun") == 0) {
    for (int i = 0; i < NUM_LEDS; i++) {
      int isThisPixel = 
        i == currentPixel ||
        i%(NUM_LEDS/2) == currentPixel%(NUM_LEDS/2);

      leds[i] = isThisPixel ? CRGB(red, green, blue) : CRGB(0, 0, 0);

      // If this is the pixel, also light up the next and previous two pixels 
      if (isThisPixel) {
        leds[(i+1)%NUM_LEDS] = CRGB(red, green, blue);
        leds[(i+2)%NUM_LEDS] = CRGB(red, green, blue);

        leds[i-1 < 0 ? NUM_LEDS-1 : i-1] = CRGB(red, green, blue);
        leds[i-2 < 0 ? NUM_LEDS-2 : i-2] = CRGB(red, green, blue);

      }
      
    }
    FastLED.show();
    currentPixel = (currentPixel+1) % NUM_LEDS;
    delay(delayTime);
  }
}

// SerialEvent occurs whenever a new data comes in the hardware serial RX. This
// function is called between each time loop() runs, and is a good place to
// read incoming data into a string.
void serialEvent() {
  while (Serial.available()) {
    // Get the new byte:
    char inChar = (char)Serial.read();
    // Add it to the inputString:
    inputString += inChar;
    // If the incoming character is a newline, set a flag so the main loop can
    // process the string:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
