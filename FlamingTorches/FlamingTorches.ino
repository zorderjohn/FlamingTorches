/*
 * FlamingTorches.ino - Version 1.0
 *
 * Written by Jonatan Martinez - January 2017
 *
 * This sketch generates pwm values to simulate the randomness intensity of 
 * fire. 
 * 
 * It has been tested with Teensy 2.0, although it should work with any 
 * Arduino compatible board. 
 * 
 * ----- INSTRUCTIONS -----
 * - Configure N_OUTS to the number of outputs you wish to have. 
 * - Check outs[] and write the output pins in order
 * - Configure MIN_FLAME_VALUE and FLAME_STEP to modify the fire appearance
 * 
 * ----- LICENSE -----
 * See LICENSE file. 
 */
  

#include <avr/sleep.h>

// Number of output channels
#define N_OUTS 7  

// Maximum and minimum values of the flames
#define MAX_FLAME_VALUE 1000
#define MIN_FLAME_VALUE 300

// Amount of random change every frame (relative to MAX_FLAME_VALUE)
#define FLAME_STEP 20

// Milliseconds of delay each frame
#define FLAME_DELAY 1

// Number of seconds for the automatic shutdown
#define SHUTDOWN_SECONDS (3600*4)

// Definition of the output pins
const int outs[N_OUTS] = {15, 14, 12, 4, 5, 10, 9};

// Other variables, not configurable

// Flame values
int16_t flames[N_OUTS] = {0, 0, 0, 0, 0, 0, 0};

// Number of elapsed seconds
uint32_t secs = 0;
unsigned long lastMillis = 0;

void setup()
{
  // Disable ADC to save energy
  ADCSRA = 0;
  
  // Disable Serial comm to save energy
  Serial.end();
  Serial1.end();

  // Configure outputs
  for(int i=0; i<N_OUTS; i++) {
    pinMode(outs[i], OUTPUT);
  }

  // Setup shutdown counter
  lastMillis = millis();
}

void loop() 
{
  // Update flame intensity
  for(uint8_t i=0; i<N_OUTS; i++) {
    flames[i] += random(-FLAME_STEP, FLAME_STEP + 1);
    flames[i] = constrain(flames[i], MIN_FLAME_VALUE, MAX_FLAME_VALUE);
    updateFlame(i, flames[i]);
  }
  delay(FLAME_DELAY);

  // Increase shutdown counter
  if(millis() - lastMillis > 1000) {
    secs++;
    lastMillis = millis();
  }

  // Call shutdown routine
  if(secs > SHUTDOWN_SECONDS) {
    powerdown();
  }
}

// Test loop
void Testloop() 
{
  for(uint8_t i=0; i<N_OUTS; i++) {
    flames[i] = (flames[i] + 1) % MAX_FLAME_VALUE;
    updateFlame(i, flames[i]);
  }
  delay(FLAME_DELAY);
}


void powerdown() {
  for(uint8_t i=0; i<N_OUTS; i++)
  {
    digitalWrite(outs[i], 0);
  }
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  noInterrupts();
  sleep_enable();
  interrupts();
  sleep_cpu();
  sleep_disable();
}



void updateFlame(uint8_t id, int16_t value) 
{
  float normValue = value / float(MAX_FLAME_VALUE);

  // Use a cuadratic function to make light intensity more linear
  normValue = normValue * normValue;

  // Write output
  uint8_t pwmValue = normValue * 255;
  analogWrite(outs[id], pwmValue);
}

