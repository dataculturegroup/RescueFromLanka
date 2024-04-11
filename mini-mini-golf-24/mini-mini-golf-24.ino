#include <Adafruit_NeoPixel.h>
#include "SCMD.h"
#include "SCMD_config.h" //Contains #defines for common SCMD register names and values
#include "Wire.h"

#define FINAL_HOLE_PIN      5
#define PALACE_LIGHTS_PIN   9
#define DEBUG_LED_PIN       13
#define SHORTCUT_SLIDE_PIN  4
#define SLIDE_LIGHTS_PIN    8

SCMD obstacleMotors;
unsigned long timeToChangeLankaRocks = 0;
#define LANKA_ROCKS_MOTOR   1
#define LANKA_ROCKS_CW        0
#define LANKA_ROCKS_SLEEP_1   1
#define LANKA_ROCKS_CCW       2
#define LANKA_ROCKS_SLEEP_2   3
int lankaRocksState = LANKA_ROCKS_SLEEP_2;

#define NUM_PALACE_LEDS     44
uint32_t palaceFireColors[NUM_PALACE_LEDS];
Adafruit_NeoPixel PalaceLights(NUM_PALACE_LEDS, PALACE_LIGHTS_PIN);
unsigned long timeToStopCelebrating = 0;
bool celebrating = true;

#define NUM_SLIDE_LEDS      17
Adafruit_NeoPixel SlideLights(NUM_SLIDE_LEDS, SLIDE_LIGHTS_PIN);

bool ballInHole = 0;
bool ballInSlide = 0;

void setup() {
  // debug setup
  pinMode(DEBUG_LED_PIN, OUTPUT);
  digitalWrite(DEBUG_LED_PIN, LOW);
  Serial.begin(9600);
  // slide setup
  pinMode(SHORTCUT_SLIDE_PIN, INPUT_PULLUP);
  pinMode(SLIDE_LIGHTS_PIN, OUTPUT);
  SlideLights.begin();
  SlideLights.clear();
  SlideLights.show();
  // lanka setup
  pinMode(FINAL_HOLE_PIN, INPUT_PULLUP);
  pinMode(PALACE_LIGHTS_PIN, OUTPUT);
  PalaceLights.begin();
  PalaceLights.clear();
  PalaceLights.show();
  // obstacle motors on Lanka island
  obstacleMotors.settings.I2CAddress = 0x5D;
  obstacleMotors.settings.chipSelectPin = 10;
  while ( obstacleMotors.begin() != 0xA9 ){  //Wait until a valid ID word is returned
    delay(500);
  }
  Serial.println( " - motor ID matches 0xA9" );
  while ( obstacleMotors.ready() == false );
  while ( obstacleMotors.busy() );
  obstacleMotors.enable(); 
  // ready for action
  Serial.println("<Lanka Rescue Ready>");
}

void loop() {
  ballInHole = !digitalRead(FINAL_HOLE_PIN);
  ballInSlide = !digitalRead(SHORTCUT_SLIDE_PIN);
  if (millis() >= timeToChangeLankaRocks) {
    lankaRockStateMachine();
  }
  if (ballInSlide) {
    animateSlide();
  }
  if (ballInHole && !celebrating) {
    startCelebrating();
  }
  if (celebrating) {
    updateCelebration();
  }
}

uint32_t palaceFireColorOptions[6] = {
  PalaceLights.Color(195, 10, 2),
  PalaceLights.Color(235, 22, 7),
  PalaceLights.Color(185, 26, 12),
  PalaceLights.Color(135, 7, 2),
  PalaceLights.Color(225, 2, 11),
  PalaceLights.Color(205, 4, 2)
};


#define CELEBRATION_DURATION_MS  4000
void startCelebrating() {
  celebrating = true;
   // randomize a little for effects
  for (int p=0;p<NUM_PALACE_LEDS;p++) {
    int colorIdx = random(0, 6);
    palaceFireColors[p] = palaceFireColorOptions[colorIdx];
  }
  // track when to stop
  timeToStopCelebrating = millis() + CELEBRATION_DURATION_MS;
}

void updateCelebration() {
  if (millis() >= timeToStopCelebrating) {
    PalaceLights.clear();
    PalaceLights.show();
    celebrating = false;
    return;
  }
  for(int p=0; p < NUM_PALACE_LEDS; p++) {
    uint32_t targetColor = palaceFireColors[p];
    int currentTime = timeToStopCelebrating - millis();
    float currentStepPct = (float) currentTime / (float) CELEBRATION_DURATION_MS;
    float randomFade = (float) random(0,20) / (float) 20;
    uint32_t stepColor = colorFade(targetColor, randomFade);
    PalaceLights.setPixelColor(p, stepColor);
  }
  PalaceLights.show();
}

uint32_t colorFade(uint32_t targetColor, float fadePct) {
    // Extract RGB components from the targetColor
    uint8_t targetRed = (targetColor >> 16) & 0xFF;
    uint8_t targetGreen = (targetColor >> 8) & 0xFF;
    uint8_t targetBlue = targetColor & 0xFF;

    // Calculate faded colors
    uint8_t fadedRed = (uint8_t)(targetRed * fadePct);
    uint8_t fadedGreen = (uint8_t)(targetGreen * fadePct);
    uint8_t fadedBlue = (uint8_t)(targetBlue * fadePct);

    // Compose the faded color
    uint32_t fadedColor = ((uint32_t)fadedRed << 16) | ((uint32_t)fadedGreen << 8) | fadedBlue;

    return fadedColor;
}

void lankaRockStateMachine() {
  unsigned long now = millis();
  if (lankaRocksState == LANKA_ROCKS_CW) {
    lankaRocksState = LANKA_ROCKS_SLEEP_1;
    obstacleMotors.setDrive(LANKA_ROCKS_MOTOR, 0, 0);
    timeToChangeLankaRocks = now + 1000;
  } else if (lankaRocksState == LANKA_ROCKS_SLEEP_1) {
    lankaRocksState = LANKA_ROCKS_CCW;
    obstacleMotors.setDrive(LANKA_ROCKS_MOTOR, 1, 200);
    timeToChangeLankaRocks = now + 3000;
  } else if (lankaRocksState == LANKA_ROCKS_CCW) {
    lankaRocksState = LANKA_ROCKS_SLEEP_2;
    obstacleMotors.setDrive(LANKA_ROCKS_MOTOR, 0, 0);
    timeToChangeLankaRocks = now + 1000;
  } else if (lankaRocksState == LANKA_ROCKS_SLEEP_2) {
    lankaRocksState = LANKA_ROCKS_CW;
    obstacleMotors.setDrive(LANKA_ROCKS_MOTOR, 0, 200);
    timeToChangeLankaRocks = now + 3000;
  }
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
