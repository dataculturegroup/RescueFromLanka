#include <Adafruit_NeoPixel.h>
#include "SCMD.h"
#include "SCMD_config.h" //Contains #defines for common SCMD register names and values
#include "Wire.h"

#define SHORTCUT_SLIDE_PIN  4
#define SLIDE_LIGHTS_PIN    8
#define DEBUG_LED_PIN       13

SCMD obstacleMotors;
unsigned long timeToChangeLankaRocks = 0;
#define LANKA_ROCKS_MOTOR   1
#define LANKA_ROCKS_CW        0
#define LANKA_ROCKS_SLEEP_1   1
#define LANKA_ROCKS_CCW       2
#define LANKA_ROCKS_SLEEP_2   3
int lankaRocksState = LANKA_ROCKS_SLEEP_2;

#define NUM_SLIDE_LEDS      17
Adafruit_NeoPixel SlideLights(NUM_SLIDE_LEDS, SLIDE_LIGHTS_PIN, NEO_GRB + NEO_KHZ800);

bool ballInHole = 0;
bool ballInSlide = 0;

void setup() {
  // debug setup
  pinMode(DEBUG_LED_PIN, OUTPUT);
  digitalWrite(DEBUG_LED_PIN, LOW);
  Serial.begin(9600);
  // slide setup
  pinMode(SHORTCUT_SLIDE_PIN, INPUT_PULLUP);
  SlideLights.begin();
  SlideLights.clear();
  SlideLights.show();
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
  ballInHole = 0;
  ballInSlide = !digitalRead(SHORTCUT_SLIDE_PIN);
  if (millis() >= timeToChangeLankaRocks) {
    lankaRockStateMachine();
  }
  if (ballInSlide) {
    animateSlide();
  }
  if (ballInHole) {
    celebrateVictory();
  }
}

void celebrateVictory() {
  // to do 
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
