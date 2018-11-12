#include <FastLED.h>

// How many leds in your strip?
#define NUM_LEDS 12

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 5
#define CLOCK_PIN 13
#define ITERATE 1
//step delay = milliseconds between every step
#define STEP_DELAY 10
// end delay = wait for how many milli seconds before fading back
#define END_DELAY 1000

// Define the array of leds
CRGB leds[NUM_LEDS];
int fromcolor[] = {0, 0, 255};
int tocolor[] = {255, 0, 0};
int currentcolor[] = {0,0,0};
int maxdistance = 0;
float factor[3]; 
int coloriterator = 0;
int positionLED = 0;
int halfdistance = NUM_LEDS/2 -1;
int currentPosition = 0;


void setup() { 
      Serial.begin(9600);
      //delay for 2 seconds because else the first serial output will get lost.
      delay(2000);
      // Uncomment/edit one of the following lines for your leds arrangement.
      // FastLED.addLeds<TM1803, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<TM1804, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<TM1809, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
  	  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
      // FastLED.addLeds<APA104, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<UCS1903, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<UCS1903B, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<GW6205, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<GW6205_400, DATA_PIN, RGB>(leds, NUM_LEDS);
      
      // FastLED.addLeds<WS2801, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<SM16716, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<LPD8806, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<P9813, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<APA102, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<DOTSTAR, RGB>(leds, NUM_LEDS);

      // FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<SM16716, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<P9813, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
 //determine maximal color distance 
 for (int i=0;i<3;i++) {
  if (abs(fromcolor[i] - tocolor[i]) > maxdistance) {
    maxdistance = abs(fromcolor[i] - tocolor[i]);
  }
 }
 //determine change factor for each color
  for (int i=0;i<3;i++) {
    factor[i] = maxdistance / abs(float(fromcolor[i])-float(tocolor[i]));
    currentcolor[i] = fromcolor[i];
  }
  
//set LEDs to initial color  
  leds[0] = CRGB(fromcolor[0], fromcolor[1], fromcolor[2]);
  
  FastLED.show();
}

//add a number to a led position and figure out which LED it is. start from the beginning if you go past the maximum number of LEDs
int ringposition(int currentPosition, int distance) {
  if (currentPosition + distance > NUM_LEDS-1) {
    return currentPosition+distance-NUM_LEDS;
  }
  else {
    return currentPosition+distance;
  }
}

//changes the color depending on the iterator and how many iterator steps it needs for one color value
int iterate(int fromcolor, int currentcolor, int tocolor, int coloriterator) {
  if (abs(currentcolor - fromcolor) < int(abs(coloriterator)) && currentcolor != tocolor) {
    if (tocolor - fromcolor > 0) {
      currentcolor++;
    }
    else {
      currentcolor--;
    }
  }
  return currentcolor;
}

int printout(CRGB leds[NUM_LEDS]) {
  Serial.print("LEDS = ");
  for(int i = 0;i<NUM_LEDS;i++) {
    Serial.print(leds[i].red);
    Serial.print(", ");
  }
  Serial.println("");
  
}

void loop() { 
// switch color directions
  if (coloriterator == maxdistance)  {
    
    leds[0] = CRGB(tocolor[0], tocolor[1], tocolor[2]);
    FastLED.show();
    //switch fromcolor and tocolor
    memcpy(currentcolor, tocolor, sizeof(tocolor));
    memcpy(tocolor, fromcolor, sizeof(fromcolor));
    memcpy(fromcolor, currentcolor, sizeof(fromcolor));
    //reset iterator
    coloriterator = 0;
    //wait for n amount of milliseconds before starting the next fade
    delay(END_DELAY);
  }
  else {
    //for each color
    for (int j=0;j<3;j++) {
      // iterate the current color
      currentcolor[j] = iterate(fromcolor[j], currentcolor[j], tocolor[j], coloriterator);
    }
    //increment the iterator
    coloriterator+= ITERATE;
    
    //go through all LEDS
    //Change num_leds/2 leds forward
    for (int i=halfdistance; i>-1;i--) {
      //print out the led values to serial
      //printout(leds);
      //the position of the LED to change is the iterator plus the current position (on the ring)
      currentPosition = ringposition(positionLED, i);
      //copy the value of the leds forward by two leds)
      leds[ringposition(currentPosition, 2)] = leds[currentPosition];
    }
    //set the current position
    positionLED = coloriterator % NUM_LEDS;   
    //change the value of the "frontmost led" to its new value
    leds[positionLED] = CRGB(currentcolor[0],currentcolor[1],currentcolor[2]);
    FastLED.show();
    delay(STEP_DELAY);
  }
}
