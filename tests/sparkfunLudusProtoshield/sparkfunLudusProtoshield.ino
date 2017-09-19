/* 
Control of the position of a servo motor and the speed of a DC motor using the Sparkfun Ludus Protoshield with a slider potentiometer and a rotary potentiometer, respectively.
*/
#include <Servo.h>

Servo servo_one;

static const int SERVO_ONE = 9;
static const int SERVO_ONE_LOWER = 1;
static const int SERVO_ONE_UPPER = 145;

static const int POT_ONE = A5;
static const int POT_ONE_LOWER = 10;
static const int POT_ONE_UPPER = 860;

static const int MOTOR_ONE_PWM = 3;
static const int MOTOR_ONE_DIR_ONE = 4;
static const int MOTOR_ONE_DIR_TWO = 5;

static const int POT_TWO = A4;
static const int POT_TWO_LOWER = 10;
static const int POT_TWO_UPPER = 860;
static const int POT_TWO_MID = (POT_TWO_LOWER + POT_TWO_UPPER) / 2;
static const int POT_TWO_MID_MARGIN = 40;
int val;    // variable to read the value from the analog pin

void setup() {
  Serial.begin(9600);
  
  pinMode(POT_ONE, INPUT);
  
  servo_one.attach(SERVO_ONE);
  servo_one.write(SERVO_ONE_LOWER);

  pinMode(POT_TWO, INPUT);

  pinMode(MOTOR_ONE_PWM, OUTPUT);
  pinMode(MOTOR_ONE_DIR_ONE, OUTPUT);
  pinMode(MOTOR_ONE_DIR_TWO, OUTPUT);
  
  delay(5000);
}

void loop() {
  int pot_one_value = analogRead(POT_ONE);
  servo_one.write(map(pot_one_value, POT_ONE_LOWER, POT_ONE_UPPER, SERVO_ONE_LOWER, SERVO_ONE_UPPER));
  int pot_two_value = analogRead(POT_TWO);
  if (pot_two_value >= POT_TWO_MID + POT_TWO_MID_MARGIN) {
    left(map(pot_two_value, POT_TWO_MID + POT_TWO_MID_MARGIN, POT_TWO_UPPER, 0, 255));
  } else if (pot_two_value <= POT_TWO_MID - POT_TWO_MID_MARGIN) {
    right(map(pot_two_value, POT_TWO_LOWER, POT_TWO_MID - POT_TWO_MID_MARGIN, 255, 0));
  } else {
    brake();
  }
}

void left(int speed) { 
  digitalWrite(MOTOR_ONE_DIR_ONE, LOW);
  digitalWrite(MOTOR_ONE_DIR_TWO, HIGH);

  analogWrite(MOTOR_ONE_PWM, constrain(speed, 0, 255));
}

void right(int speed) { 
  digitalWrite(MOTOR_ONE_DIR_ONE, HIGH);
  digitalWrite(MOTOR_ONE_DIR_TWO, LOW);

  analogWrite(MOTOR_ONE_PWM, constrain(speed, 0, 255));
}

void brake() {
  digitalWrite(MOTOR_ONE_DIR_ONE, HIGH);
  digitalWrite(MOTOR_ONE_DIR_TWO, HIGH);
  
  analogWrite(MOTOR_ONE_PWM, 0);
}

