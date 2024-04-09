#include <Adafruit_NeoPixel.h>

#define SHORTCUT_SLIDE_PIN  4
#define SLIDE_LIGHTS_PIN    8
#define DEBUG_LED_PIN       13

#define NUM_SLIDE_LEDS      17
Adafruit_NeoPixel SlideLights(NUM_SLIDE_LEDS, SLIDE_LIGHTS_PIN, NEO_GRB + NEO_KHZ800);

bool ballInHole = 0;
bool ballInSlide = 0;

void setup() {
  pinMode(DEBUG_LED_PIN, OUTPUT);
  digitalWrite(DEBUG_LED_PIN, LOW);
  pinMode(SHORTCUT_SLIDE_PIN, INPUT_PULLUP);
  SlideLights.begin();
  startObstacles();
  Serial.begin(9600);
  Serial.println("<Lanka Rescue Ready>");
}

void loop() {
  ballInHole = 0;
  ballInSlide = !digitalRead(SHORTCUT_SLIDE_PIN);
  if (ballInSlide) {
    animateSlide();
  }
  //if (ballInHole) {
  //}
}

void handleShortcut() {
  // TO DO
  digitalWrite(DEBUG_LED_PIN, HIGH);
  delay(300);
  digitalWrite(DEBUG_LED_PIN, LOW);
}

#define SLIDE_LIGHT_ANIMATION_DELAY      60
void animateSlide() {
  SlideLights.clear();
  SlideLights.show();
  for (int pixel = 0; pixel < NUM_SLIDE_LEDS; pixel++) {
    if ((pixel-3) >= 0) {
      SlideLights.setPixelColor(pixel-3, SlideLights.Color(0, 0, 0));
    }
    if ((pixel-2) >= 0) {
      SlideLights.setPixelColor(pixel-2, SlideLights.Color(0, 0, 30));
    }
    if ((pixel-1) >= 0) {
      SlideLights.setPixelColor(pixel-1, SlideLights.Color(0, 0, 60));
    }
    SlideLights.setPixelColor(pixel, SlideLights.Color(0, 0, 255));
    SlideLights.show();
    delay(SLIDE_LIGHT_ANIMATION_DELAY);
  }
  SlideLights.clear();
  SlideLights.show();
}

void startObstacles() {
  SlideLights.clear();
  SlideLights.show();
}
