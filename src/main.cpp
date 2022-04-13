#include <Arduino.h>
#include <Servo.h>
// Futuras librerias para migrar al chip PCA9685
// #include <Wire.h>
// #include <Adafruit_PWMServoDriver.h> 

const int derecho = 90; 
const int cruzado = 120;
const int led_cruzado = 7;
const int led_derecho = 6;
const int pul_servo1 = 8;
const int pul_servo2 = 4;
const int servopin1 = 9;
const int servopin2 = 9;
const long step_delay = 100;
unsigned long b1_previousMillis = 0;
unsigned long b2_previousMillis = 0;
int ALIGN_MAIN = 0;
int ALIGN_DIVERGENT = 90;



void setup() {

typedef struct TURNOUT_DEF {
  int pin;
  int pos_main;
  int pos_div;
} TURNOUT_DEF ;

typedef struct TURNOUT_DATA {
  TURNOUT_DEF data;
  bool is_moving;
  int alignment;
  int pos_now;
  int target_pos;
  unsigned long last_move;
} TURNOUT_DATA ;

Servo servos[2];

// Defino las entradas y salidas
pinMode(pul_servo1, INPUT);
pinMode(pul_servo2, INPUT);
pinMode(led_derecho, OUTPUT);
pinMode(led_cruzado, OUTPUT);
Serial.begin(9600);

TURNOUT_DATA turnouts[2];

turnouts[0] = {{servopin1, derecho, cruzado}, false, ALIGN_MAIN, 93, 93, 0};
turnouts[1] = {{servopin2, derecho, cruzado}, false, ALIGN_MAIN, 93, 93, 0};

void setTurnout(int id, int align){
   switch(align){
        case ALIGN_MAIN:
          turnouts[id].is_moving = true;
          turnouts[id].last_move = 0;
          turnouts[id].target_pos = turnout[id].data.pos_main;
          turnouts[id].alignment = ALIGN_MAIN;
          break;
        case ALIGN_DIVERGENT:
          turnouts[id].is_moving = true;
          turnouts[id].last_move = 0;
          turnouts[id].target_pos = turnout[id].data.pos_div;
          turnouts[id].alignment = ALIGN_DIVERGENT;
          break;
      }
}
// Defino el estado inicial de los led
digitalWrite(led_derecho, HIGH);
digitalWrite(led_cruzado, LOW);

// initialize turnouts

for(int i = 0; i < 2; i++){
  // create a SERVO instance and store it in the servos array
  servos[i] = new Servo; 
  servos[i]->attach(turnouts[i].data.pin);
  setTurnout(i, ALIGN_MAIN);  // set initial turnout position
  }
}


void loop() {
// get elapsed milliseconds at loop start

unsigned long currentMillis = millis();

  // Turnout Control
  for(int i = 0; i < 2; i++){
    if (turnouts[i].is_moving) {
      if ( (currentMillis - turnouts[i].last_move) >= step_delay ) {
        turnouts[i].last_move = currentMillis;
        // if the new angle is higher
        // and not already at destination
        if (turnouts[i].pos_now < turnouts[i].target_pos) { 
          // increment and write the new position
          servos[i]->write(++turnouts[i].pos_now);
        } else {  
           // otherwise the new angle is equal or lower
           // if not already at destination
          if (turnouts[i].pos_now != turnouts[i].target_pos) { 
            // decrement and write the new position
            servos[i]->write(--turnouts[i].pos_now);
          }
        }
      }
      // if target position is reached, turn motion off
      if (turnouts[i].pos_now == turnouts[i].target_pos) {
        turnouts[i].is_moving = false;
      }
    }
  }
}