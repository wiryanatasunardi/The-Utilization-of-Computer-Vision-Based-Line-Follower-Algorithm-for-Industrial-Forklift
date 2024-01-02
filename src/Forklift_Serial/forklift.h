#ifndef FORKLIFT_H
#define FORKLIFT_H

#include "ServoEaser.h"
#include "Wire.h"
#include <PS2X_lib.h>
#include <LapX9C10X.h>
#include <DigiPotX9Cxxx.h>
#include <FastX9CXXX.h>

#include <Servo.h>
#include <Stepper.h>
#include <MPU6050_light.h>
//include <SharpDistSensor.h>

//====================
#define pressures   true
#define rumble      true
#define samples 50
#define maxVal 50
#define minVal -50

#define PS2_DAT 51
#define PS2_CMD 50
#define PS2_SEL 53
#define PS2_CLK 52

// Digital Potentiometer Pin
#define X9_CS_PIN 1
#define X9_UD_PIN 2
#define X9_INC_PIN 3

#define X9_CS_PIN_2 4
#define X9_UD_PIN_2 5
#define X9_INC_PIN_2 6

// Ultrasonic Paralax Pin
int pingPin = 11;

// Stepper Motor Pin
int pinA = 14;
int pinB = 15;
int pinC = 16;
int pinD = 17;
int stepsPerRevolution = 2048;
int motSpeed = 5;

int error = 0;
byte type = 0;
byte vibrate = 0;

int LX = 0;
int LY = 0;
int RX = 0;
int RY = 0;

PS2X ps2x;
Servo my_servo;
FastX9C103 pot(X9_CS_PIN, X9_UD_PIN, X9_INC_PIN);
FastX9C103 pot2(X9_CS_PIN_2, X9_UD_PIN_2, X9_INC_PIN_2);
Stepper myStepper = Stepper(stepsPerRevolution, pinA, pinB, pinC, pinD);
MPU6050 mpu(Wire);


// Gyroscope Module Variable
int xsample = 0;
int ysample = 0;
int zsample = 0;
int value1;
int value2;
int value3;
int xValue = 0;
int yValue = 0;
int zValue = 0;

//====================
int counter;
float resistance;

#define PWM 7
#define INA A0
#define INB A1
#define EN A2

int servoPin  = 14;
int relay_F = 23;
int relay_B = 25;

int w_led = 39;
int g_led = 41;
int b_led = 43;
int y_led = 45;
int r_led = 47;

bool relay_f = false;
bool relay_b = false;

int choices = 0;
int mode_user = 0;
//int button_mode = 0;
//long int select_counter = 0;

#endif