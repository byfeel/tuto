#include <Arduino.h>
// Tuto ring NeoPixel
// Byfeel 2019
// bibliotheque adafruit neopixel
#include <Adafruit_NeoPixel.h>

// pin ou est branché la commande dataIn du ring
#define LED_PIN  D3

// Nombre de led
#define LED_COUNT  12

// NeoPixel luminosite, 0 (min) to 255 (max)
#define BRIGHTNESS 30

// Declare votre neopixel Ring
Adafruit_NeoPixel ring(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Nombre de Pixel dans le Ring
// Argument 2 = Wemos Pin
// Argument 3 = Pixel type
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Led cablé selon GRB (most NeoPixel products)
//   NEO_GRBW    Led cablé selon GRB + W (most NeoPixel products) + Led blanche
//   NEO_RGB     Led cablé selon RGB (v1 FLORA pixels, not v2)
//   NEO_RGBW   led cablé selon  RGBW (NeoPixel RGBW products)


// definition des couleurs
uint32_t Red=ring.Color(255,   0,   0);
uint32_t Blue=ring.Color(0,   0,   255);
uint32_t Green=ring.Color(0, 255,   0);
uint32_t Yellow=ring.Color(255, 255,   0);
uint32_t White=ring.Color(255, 255, 255);
uint32_t Orange = ring.gamma32(ring.ColorHSV(5462, 255, 255));
uint32_t Violet = ring.gamma32(ring.ColorHSV(52000, 255, 255));
uint32_t Pink = ring.gamma32(ring.ColorHSV(60080));

// gonction qui allume les leds unes apres les autres de la même couleur
void colorWipe(uint32_t color, int speed=50,bool single=false,int loops=1) {
for (int j=0;j<loops;j++) {
  for(int i=0; i<ring.numPixels(); i++) {
    if (single) ring.clear();
    ring.setPixelColor(i, color);
    ring.show();
    delay(speed);
  }
}
}


void rainbow(int rainbowLoops,int F=100,int speed=3, int S=255) {
  int fadeVal=0, fadeMax;
  fadeMax=constrain(F, 1, 100);
  for(uint32_t firstPixelHue = 0; firstPixelHue < rainbowLoops*65536;
    firstPixelHue += 256) {

    for(int i=0; i<ring.numPixels(); i++) {
      uint32_t pixelHue = firstPixelHue + (i * 65536L / ring.numPixels());
      ring.setPixelColor(i, ring.gamma32(ring.ColorHSV(pixelHue, S,255* fadeVal / fadeMax )));
    }

    ring.show();
    delay(speed);

    if(firstPixelHue < 65536) {                              // Premiere boucle,
      if(fadeVal < fadeMax) fadeVal++;                       // fade in
    } else if(firstPixelHue >= ((rainbowLoops-1) * 65536)) { // Derniere boucle loop,
      if(fadeVal > 0) fadeVal--;                             // fade out
    } else {
      fadeVal = fadeMax; // boucle intermediaire fade=fademax
    }
  }
}

void setup() {

ring.begin();           // INITIALIZE NeoPixel ring object (REQUIRED)
ring.setBrightness(BRIGHTNESS); // Set BRIGHTNESS to about 1/5 (max = 255)
ring.show();            // Turn OFF all pixels ASAP

}

void loop() {
  ring.fill(Orange,1,3);
  ring.fill(Violet,5,7);
  ring.fill(Pink,9,11);
  ring.show();
  delay(1000);
  ring.clear();
  colorWipe(Red); // Red
  colorWipe(Blue, 100); // Green
  colorWipe(Orange,60,true,2);
  rainbow(3,50);
  colorWipe(White,30,true,5);
  rainbow(5,100,10,200);

}
