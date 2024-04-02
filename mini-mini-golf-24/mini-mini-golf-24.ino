#include <Adafruit_NeoPixel.h>

#define PALACE_LIGHTS_PIN   8
#define NUM_PALACE_LIGHTS   33

Adafruit_NeoPixel PalaceLights(NUM_PALACE_LIGHTS, PALACE_LIGHTS_PIN, NEO_GRB + NEO_KHZ800);
bool ballInHole = 0;
bool ballInShortcut = 0;

void setup() {
  PalaceLights.begin();
  startObstacles();
}

void loop() {
  ballInHole = 0;
  ballInShortcut = 0;
  if (ballInShortcut) {
    handleShortcut();
  }
  if (ballInHole) {
    celebrateVictory();
  }
}

void handleShortcut() {
  // TO DO
}

#define DELAY_INTERVAL      100
void celebrateVictory() {
  PalaceLights.clear();
  PalaceLights.show();
  for (int pixel = 0; pixel < NUM_PALACE_LIGHTS; pixel++) {
    PalaceLights.setPixelColor(pixel, PalaceLights.Color(0, 255, 0));
    PalaceLights.show();
    delay(DELAY_INTERVAL);
  }
  for (int pixel = NUM_PALACE_LIGHTS-1; pixel >= 0; pixel--) {
    PalaceLights.setPixelColor(pixel, PalaceLights.Color(0, 0, 0));
    PalaceLights.show();
    delay(DELAY_INTERVAL);
  }
  PalaceLights.clear();
  PalaceLights.show();
}

void startObstacles() {
  PalaceLights.clear();
  PalaceLights.show();
}