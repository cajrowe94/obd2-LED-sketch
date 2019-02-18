#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>
#include <FastLED.h>
#include <fastled_config.h>
#include <fastled_delay.h>
#include <fastled_progmem.h>
#include <fastpin.h>
#include <fastspi.h>
#include <fastspi_bitbang.h>
#include <fastspi_dma.h>
#include <fastspi_nop.h>
#include <fastspi_ref.h>
#include <fastspi_types.h>
#include <hsv2rgb.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <noise.h>
#include <pixelset.h>
#include <pixeltypes.h>
#include <platforms.h>
#include <power_mgt.h>

#include <OBD2UART.h>

/*  -----FUNCTIONS-----
 
 * setBaudRate – set adapter serial baudrate
 * readPID – read specified OBD-II PID and return parsed value
 * clearDTC – clear diagnostic trouble code
 * getVoltage – measure car battery voltage
 * getVIN – retrieve Vehicle Identification Number
 * getTemperature – get device temperature
 * readAccel – read accelerometer X/Y/Z values
 * memsInit – initialize motion sensor
 * memsRead – read motion sensor data
 * memsOrientation – retrieve computed orientation data
 * Engine
 * 
 -----RETURNED VALUES-----
PID_RPM – Engine RPM (rpm)
PID_ENGINE_LOAD – Calculated engine load (%)
PID_COOLANT_TEMP – Engine coolant temperature (°C)
PID_ENGINE_LOAD – Calculated Engine load (%)
PID_ABSOLUTE_ENGINE_LOAD – Absolute Engine load (%)
PID_TIMING_ADVANCE – Ignition timing advance (°)
PID_ENGINE_OIL_TEMP – Engine oil temperature (°C)
PID_ENGINE_TORQUE_PERCENTAGE – Engine torque percentage (%)
PID_ENGINE_REF_TORQUE – Engine reference torque (Nm)

Intake/Exhaust
PID_INTAKE_TEMP – Intake temperature (°C)
PID_INTAKE_PRESSURE – Intake manifold absolute pressure (kPa)
PID_MAF_FLOW – MAF flow pressure (grams/s)
PID_BAROMETRIC – Barometric pressure (kPa)

Speed/Time
PID_SPEED – Vehicle speed (km/h)
PID_RUNTIME – Engine running time (second)
PID_DISTANCE – Vehicle running distance (km)

Driver
PID_THROTTLE – Throttle position (%)
PID_AMBIENT_TEMP – Ambient temperature (°C)
 */

#define LED_PIN     7 //which pin to run LED strip off of
#define NUM_LEDS    150 //how many LED's in entire strip
CRGB leds[NUM_LEDS]; //led array

COBD obd; //obd2 object to use

int carRpm; //holds cars live rpms
int carRpmReverse; //maps values downwards to 0, rather than to 255
int carSpeed; //holds cars live speed
int engineLoad; //holds cars live engine load percentage

int red;
int green;
int blue = 255;

int color;

void setup() { //runs once

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS); //setup LED strip
 
  obd.begin(); // start communication with OBD-II adapter
  
  while (!obd.init()); // initiate OBD-II connection until success
}

void loop() {

  if (obd.readPID(PID_RPM, carRpm)) { //reads RPM's, saves into carRpm
    map(carRpm, 0, 8000, 0, 255);
    map(carRpmReverse, 0, 8000, 255, 0);
  }
  
  if (obd.readPID(PID_SPEED, carSpeed)) { //reads speed of car, saves result into carSpeed
    map(carSpeed, 0, 100, 0, 255); //in KPH
  }

  if (carRpm > 6000){ //check if youre redlining the car
    redlineAnimation(); //if so, run the redline function
  }
  else if (carRpm > 1000 && carRpm < 6000){ //check if car is above idle and below redline
    for (int i = 0; i <= NUM_LEDS; i++) { //apply colors to all LED's
      leds[i] = CRGB(0, carRpm, blue); //the rpm's increase the green
    }
    FastLED.show(); //show LED's
  }
  else if (carRpm < 1000){ //check if car is idling
    color = random(0, 255);
    idleAnimation(); //if so, run the idle function
  }

}







void idleAnimation(){ //called when car is at idle
  //int color = random(0, 255); //get a new color every time function is called
  int stripLength = 5; //set how many leds you want running across strip at once
  
  for (int i = 0; i < NUM_LEDS; i++){ //run through LED array
    if (i >= stripLength){ //check if you are at your preferred led strip length
      leds[i-5] = CRGB(0,0,0); //if so, turn off leds behind it
      //FastLED.show();
      //delay(20);
    }
    leds[i] = CRGB(random(0, 255), random(0, 255), random(0, 255)); //light up the next LED
    FastLED.show();
    delay(20);
  }
}



void redlineAnimation(){ //when RPM's reach 6k+

//  FastLED.show();

  for (int i = 0; i <= NUM_LEDS; i++){ //turn all leds to red
    leds[i] = CRGB(255, 0, 0);
  }

  FastLED.show(); //turn them on
  delay(10);
  
  for (int i = 0; i <= NUM_LEDS; i++){ //turn them all to black
    leds[i] = CRGB(0, 0, 0);
  }

  FastLED.show(); //display it and repeat the process to achieve a blinking effect
  delay(10);

}











