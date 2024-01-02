#include "forklift.h"

int height_limit = 5001;
int height_live;
int set_limit = 1;
int step_revo = 0;
int stepPot_manual = 0;
int stepPot = 0;
int stepPot2 = 0;
long distance;
int i;

//************************ BASIC MOVE ************************

int ledState = HIGH;
unsigned long previousMillis = 0;
long rememberTime=0;         
long onDuration; 
long offDuration;

//************************ RFID *******************************
String tags_A = "02008AC5FCB1";
String tags_B = "02008A80F8F0";
String tags_C = "020089C55719";
String tags_D = "020089AA80A1";
String hasil;
String command;

const int BUFFER_SIZE = 16;
char buf[BUFFER_SIZE];


void setup() {
  // put your setup code here, to run once:
  //ps2x.read_gamepad(false, vibrate);
  Serial.begin(115200);
  Serial2.begin(115200);
  //Serial.println("==================== FORKLIFT NEW PROJECT ====================");
  pinMode(PWM, OUTPUT);
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);
  pinMode(EN, OUTPUT);

  // Relay For Move Forward and Backward
  pinMode(relay_F, OUTPUT);
  pinMode(relay_B, OUTPUT);

  // LED Set Up
  pinMode(w_led, OUTPUT);
  pinMode(b_led, OUTPUT);
  pinMode(g_led, OUTPUT);
  pinMode(r_led, OUTPUT);
  pinMode(y_led, OUTPUT);

  // Stepper Motor Set Up
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT);

  //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  pot.Reset();
  pot2.Reset();
  

  if(error == 0){
    Serial.print("Controller is Found, configured successful");
  }

  type = ps2x.readType();
  switch (type) {
    case 0:
      Serial.print("Unknown Controller type found ");
      break;
    case 1:
      Serial.print("DualShock Controller found ");
      break;
    case 2:
      Serial.print("GuitarHero Controller found ");
      break;
    case 3:
      Serial.print("Wireless Sony DualShock Controller found ");
      break;
  }
  my_servo.attach(servoPin);
  myStepper.setSpeed(motSpeed);

  // Calibrating Gyroscope
  byte status = mpu.begin();
  while(status != 0){
    delay(1000);
  }
  mpu.calcOffsets();

  for(int i = 0; i < samples; i++){
    mpu.update();
    xsample += mpu.getAngleX();
    ysample += mpu.getAngleY();
    zsample += mpu.getAngleZ();
  }

  xsample /= samples;
  ysample /= samples;
  zsample /= samples;  
  Serial.println("Set Up is Done");
}

void loop() {
  ps2x.read_gamepad(false, vibrate);
  const auto left = T_B;
  const auto right = T_A;
  // put your main code here, to run repeatedly:

  user_mode();

  // switch(choices){
  //   case 0:
  //     blink_no_delay(w_led, "counter");
  //   case 1:
  //     ps2x.read_gamepad(false, vibrate);
  //     switch(mode_user){
  //       case 1:
  //         automated_scan();
  //       case 2:
  //         relay();
  //         fork_manual();
  //         manual_mode();       
  //     }
  // }


}

void mundur(){
  ps2x.read_gamepad(false, vibrate);

  auto& left = T_B;
  auto& right = T_A;
  digitalWrite(relay_F, HIGH);
  digitalWrite(relay_B, LOW);
  line_mundur();
}

void H_up(){
  for(int i=0; i<40; i++){
    int stepPot2 = stepPot2 - 3;
    pot2.JumpToStep(stepPot2);
    if(stepPot2 <= 20){
      pot2.JumpToStep(20);
    }
    delay(20);
  }
}

void H_n(){
  for(int ix=40; ix>0; ix--){
    int stepPot2 = stepPot2 + 3;
    pot2.JumpToStep(stepPot2);
    if(stepPot2 >= 45){
      pot2.JumpToStep(45);
    }
    delay(5);
  }
}

void T_N(){

  digitalWrite(y_led,LOW);
  digitalWrite(g_led,LOW);
    
  analogWrite(INA,0);
  analogWrite(INB,0);
  analogWrite(EN,255);
  analogWrite(PWM,155);
  
}

void T_B(){
  ps2x.read_gamepad(false, vibrate);

  analogWrite(INA, 255);
  analogWrite(INB, 0);
  analogWrite(EN, 255);
  analogWrite(PWM, 255);
  digitalWrite(g_led, HIGH);
  pot.JumpToStep(100);

  value1 = mpu.getAngleX();
  value2 = mpu.getAngleY();
  value3 = mpu.getAngleZ();

  xValue = xsample-value1;
  yValue = ysample-value2;
  zValue = zsample-value3;



  if(xValue < minVal || xValue > maxVal || yValue < minVal || yValue > maxVal || zValue < minVal || zValue > maxVal){
    analogWrite(INA, 0);
    analogWrite(INB, 255);
    analogWrite(EN, 255);
    analogWrite(PWM, 255);
    delay(4200);
    T_N();
  }
}

void T_A(){
  analogWrite(INA, 0);
  analogWrite(INB, 255);
  analogWrite(EN, 255);
  analogWrite(PWM, 255);
  digitalWrite(y_led, HIGH);
  //myStepper.step(1764);
}

void line_normal(){
  auto& left = T_A;
  auto& right = T_B;

  for(int iy=0; iy<100; iy++){
    stepPot = stepPot + 3;
    pot.JumpToStep(stepPot);
    if(stepPot >= 100){
      pot.JumpToStep(100);
    } 
    delay(20);    
  }
  
}

void line_mundur(){
  auto& left = T_B;
  auto& right = T_A;
  digitalWrite(relay_F, HIGH);
  digitalWrite(relay_B, LOW);  
  for(int iu=0; iu<100; iu++){
    stepPot = stepPot + 3;
    pot.JumpToStep(stepPot);
    if(stepPot >= 100){
      pot.JumpToStep(100);
    }
    delay(20);
  }
}

void stop(){
  ps2x.read_gamepad(false, vibrate);
  for(int ii=100; ii>0; ii--){
    stepPot = stepPot - 3;
    pot.JumpToStep(stepPot);
    if(stepPot <= 40){
      pot.JumpToStep(40);
    }   
    delay(5);
  }
}

void straight_turn(){
  ps2x.read_gamepad(false, vibrate);
  pot.JumpToStep(100);
}

void straight(){
  ps2x.read_gamepad(false, vibrate);
  T_N();

  auto& left = T_A;
  auto& right = T_B;
  digitalWrite(relay_F, HIGH);
  digitalWrite(relay_B, HIGH);
  line_normal();

}

void automated_scan(){
  distance = us_reader();   
  if(Serial2.available()){
    hasil = "";
    int rlen = Serial2.readBytes(buf, BUFFER_SIZE);
    for(int ia = 1; ia < 13; ia++){
      hasil += buf[i];      
    }
    Serial.println(hasil);    
  }
  if(hasil==tags_B){
    H_up();
    myStepper.step(1024);
    while(Serial.available() == 0){

    }
    command = Serial.readStringUntil('\r'); 

    if(command == "Straight"){
      mundur();      
    }
    else if(command == "Left"){
      digitalWrite(relay_F, HIGH);
      digitalWrite(relay_B, LOW);
      T_B();
      delay(1000);
    }
    else if(command == "Right"){
      digitalWrite(relay_F, HIGH);
      digitalWrite(relay_B, LOW);
      T_A();
      delay(1000);
    }
  }
  while(Serial.available() == 0){
    
  }
  command = Serial.readStringUntil('\r');
  if(command=="Straight"){
    straight();
  }
  else if(command == "Left"){
    T_A();
    delay(1000);
    //straight();
  }
  else if(command=="Right"){ 
    T_B();
    delay(1000);
    //straight();
  }

  else if(command=="Stop" && distance <= 40){
    stop();
  }    
}

void blink_no_delay(int ledPin, String blink_speed){
  if(blink_speed == "counter"){
    onDuration = 80;
    offDuration = 3000;
  }
  else if(blink_speed == "slow"){
    onDuration = 1000;
    offDuration = 4000;    
  }
  else if(blink_speed == "fast"){
    onDuration = 80;
    offDuration = 50;
  }  
  if(ledState == HIGH){
    if((millis() - rememberTime) >= onDuration){
      ledState = LOW;
      rememberTime = millis();
    }
  }
  else{
    if((millis() - rememberTime) >= onDuration){
      ledState = LOW;
      rememberTime = millis();
    }
  }
  digitalWrite(ledPin, ledState);  
}

void user_mode(){
  if(ps2x.ButtonPressed(PSB_L2)){
    digitalWrite(r_led, HIGH);
    digitalWrite(b_led, LOW);
    digitalWrite(w_led, LOW);
    choices = 0;
    mode_user = 1;
    Serial.println("L2 pressed");
  }
  if(ps2x.ButtonPressed(PSB_R2)){
    digitalWrite(r_led, LOW);
    digitalWrite(b_led, HIGH);
    digitalWrite(w_led, LOW);
    choices = 0;
    mode_user = 2;
    Serial.println("R2 pressed");
  }

  if(ps2x.ButtonPressed(PSB_START)){
    ps2x.read_gamepad(false, vibrate);
    digitalWrite(b_led,LOW);
    digitalWrite(r_led,LOW);
    digitalWrite(w_led,LOW);
    choices = 1;
    Serial.println("Start pressed");
  }

  if(choices == 1){
    
      ps2x.read_gamepad(false, vibrate);
      if (mode_user == 1){    
        automated_scan();

        }
      else if(mode_user == 2 ){
        relay();
        run_manual();
        fork_manual();
        manual_mode();
      }
  }
  else if(choices == 0){
    blink_no_delay(w_led,"counter");
  }  

}

void relay(){
  ps2x.read_gamepad(false, vibrate);
  if(ps2x.Button(PSB_PAD_UP)){
    digitalWrite(relay_F, HIGH);
    relay_f = true;
    Serial.println("Up Button is held");
  }
  else if(ps2x.Button(PSB_PAD_DOWN)){
    digitalWrite(relay_F, LOW);
    relay_f = false;
    Serial.println("Down Button is held");
  }
  else if(ps2x.Button(PSB_PAD_RIGHT)){
    digitalWrite(relay_B, HIGH);
    relay_b = true;
    Serial.println("Right Button is held");
  }
  else if(ps2x.Button(PSB_PAD_LEFT)){
    digitalWrite(relay_B, LOW);
    relay_b = false;
    Serial.println("Left Button is held");
  }
}

void fork_manual(){
  if(ps2x.Button(PSB_GREEN)){
    Serial.println("FORK GOES UP");
    H_up();
  }
  else if(ps2x.Button(PSB_PINK)){
    Serial.println("FORK GOES NEUTRAL");
    H_n();
  }
}

void run_manual(){
  if(ps2x.Button(PSB_BLUE)){
    Serial.println("X is Pressed");
    go_run();
  }

  else if(ps2x.Button(PSB_RED)){
    Serial.println("Circle is Pressed");
    stop_run();
  }
}

void manual_mode(){
  const auto left = T_A;
  const auto right = T_B;
  if(relay_f == true && relay_b == false){
    auto& left = T_A;
    auto& right = T_B;
    //Serial.println("Change A");
  }
  else if(relay_f == true && relay_b ==true){
    auto& left = T_B;
    auto& right = T_A;
    //Serial.println("Change B");
  }

  if(ps2x.Button(PSB_L1)){
    LY = ps2x.Analog(PSS_LY);
    LX = ps2x.Analog(PSS_RX);
  }

  if(ps2x.Button(PSB_R1)){
    RY = ps2x.Analog(PSS_RY);
    RX = ps2x.Analog(PSS_RX);    
  }

  if(LY < 100){
    digitalWrite(b_led, HIGH);
    T_N();
  }

  if(RY < 100){
    blink_no_delay(w_led, "fast");
  }

  if(LX < 100 || RX < 100){
    if(relay_f == true && relay_b == false){
      T_A();      
    }
    else if(relay_f == true && relay_b == true){
      T_B();      
    }
    else{
      T_A();      
    }
  }

  if(LX > 200 || RX > 200){
    if(relay_f == true && relay_b == false){
      T_B();
    }
    else if(relay_f == true && relay_b == true){
      T_A();
    }
    else{
      T_B();
    }
  }

  if(LX == 128 && LY == 128 && RX ==  128 && RY == 128){
    waithere();
  }

  LY = LX = 128;
  RY = RX = 128;

}

void turning_direction_f(){
  
  if(relay_f == true && relay_b == false){
      T_A();
    }
  else if(relay_f == true && relay_b == true){
      T_B();
    }
  
}

void go_run(){
  //ps2x.read_gamepad(false, vibrate);
  for(int i=0; i<40; i++){
    stepPot_manual = stepPot_manual - 3;
    pot.JumpToStep(stepPot_manual);
    if(stepPot_manual <= 20){
      pot.JumpToStep(20);
    }
    delay(20);
  }
  //pot.JumpToStep(stepPot_manual);
  digitalWrite(b_led, HIGH);
}

void stop_run(){
  //ps2x.read_gamepad(false, vibrate);
  for(int ix=40; ix>0; ix--){
    stepPot_manual = stepPot_manual + 3;
    pot.JumpToStep(stepPot_manual);
    if(stepPot_manual >= 45){
      pot.JumpToStep(45);
    }
    delay(5);
  }
  //pot.JumpToStep(stepPot_manual);
  digitalWrite(b_led, LOW);
}

void waithere(){ 
  ps2x.read_gamepad(false, vibrate);
  //Serial.print("stop");
  digitalWrite(g_led, LOW);
  digitalWrite(w_led, LOW);
  digitalWrite(y_led, LOW);
  digitalWrite(r_led, LOW);
  digitalWrite(b_led, LOW);
  analogWrite(INA,0);
  analogWrite(INB,0);
  analogWrite(EN,255);
  analogWrite(PWM,155);
//  servo_down();
//  H_n();
//    go_n();
}

long us_reader(){
  
  long duration, inches, cm;
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);

  cm = microsecondsToCentimeters(duration);
  //Serial.print(cm);
  //Serial.print("cm");
  //Serial.println();

  delay(100);
  return cm; 
  
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}