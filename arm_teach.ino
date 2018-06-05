/*
 * ROBBOTIC ARM TEACH CODE
 * Firstly wire all motors and potentiometers , you dont have to use pins that i used , but make shure you use pins 2 and 3 for push buttons
 * REDbutton activates recording mode,by moving potenciometer arm real arm will move like it . You will have around 12 seconds until all arrays are filled and it starts executing recored program.
 * Pressing RED button again pauses recording mode.
 * GREEN button makes real arm mimick potenciometer arm until you press GREEN button again or press RED button to start recording.
 * pressing GREEN button pauses mimick mode.
 * To reset all arrays restart arduino, then you can teach it new movements but previous movements will be lost.
 */

#include<Servo.h>
#define Speed 75

Servo myservo1; // shoulder rotation servo
Servo myservo2; // shoulder (middle) servo
Servo myservo3; // shoulder (right)  servo
Servo myservo4; // elbow servo
Servo myservo5; // wrist servo
Servo myservo6; // gripper servo

const int REDbutton = 2; //  Button above red led
const int GREENbutton = 3; // Button above green led
const int REDled = 10; // RED led
const int GREENled = 9; // GREEN led

unsigned long previousMillis=0;
const long interval =75; // Invertval in wich we record one position of each servo and place it in corensponding array

int pot1 = A4; // shoulder rotation potentiometer
int pot2 = A1; // shoulder  potentiometer 
int pot3 = A0; // elbow potentiometer
int pot4 = A2; // wrist potentiometer
int pot5 = A3; // gripper potentiometer
int WristArray[140],ElbowArray[140],GripperArray[140],ShoulderArray[140],ShoulderRotArray[140]; // arrays for each joint


volatile boolean REDmode=false;
volatile boolean GREENmode=false;

int vals[4]; // array that temporary stores values for each joint ( total of 5 joints)
int cnt=0; // value in wich we store how many times we stored positions for each servo
int i;

void setup() {
 Serial.begin(9600);
 /// define leds as outputs and buttons as inputs
pinMode(GREENled, OUTPUT);
pinMode(REDbutton, INPUT);
pinMode(REDled, OUTPUT);
pinMode(GREENbutton, INPUT);
// attaching interupts for both buttons
attachInterrupt(0,REDpress,FALLING);
attachInterrupt(1,GREENpress,RISING);
//attaching servos to corensponding pins
myservo1.attach(12); // shoulder rotation
myservo2.attach(11); //shoulder middle
myservo3.attach(4); //shoulder right
myservo4.attach(7); // elbow
myservo5.attach(6); // wrist
myservo6.attach(5); // gripper
// turning leds off
digitalWrite(REDled, HIGH);
digitalWrite(GREENled, HIGH);
// placing servos at specific position , so that arm does not go to some janky position and breaks itself
myservo6.write(60); 
delay(100);
myservo2.write(70); 
myservo3.write(70); 
delay(100);
myservo4.write(25);
delay(100);
myservo1.write(70);
delay(100);
myservo5.write(85);
delay(100);
}

void loop() {
if(GREENmode == true ){ /// activates mimic mode
 REDmode= false;
 Wrist();Elbow(); Gripper(); Shoulder(); ShoulderRot();
 digitalWrite(GREENled,LOW);
 digitalWrite(REDled, HIGH);
}else{
   digitalWrite(GREENled,HIGH);
}


//Serial.print(REDmode);Serial.print("\n");
if(REDmode == true && cnt!=140 ){ // activates teach mode
GREENmode=false;
digitalWrite(REDled, LOW); // RED led turns on
digitalWrite(GREENled,HIGH);
 /// starts recording values every 75 ms and moving arm at the same time
unsigned long currentMillis = millis();
if (currentMillis - previousMillis >= interval) {
   Wrist();Elbow(); Gripper(); Shoulder(); ShoulderRot();
   values();
   previousMillis = currentMillis;
   cnt++;
   Serial.print("\n ");Serial.print(currentMillis - previousMillis); Serial.print("\t");Serial.print(cnt);
   }
}else{
  digitalWrite(REDled,HIGH);
}


 // when we reach 140 record cycles , arm starts to loop recorded actions, it can be interrupted by activating mimic mode
 if(cnt==140 && GREENmode==false){
   REDmode== false;
  if(i!=139){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    myservo5.write(WristArray[i]);

    myservo4.write(ElbowArray[i]); 

    myservo6.write(GripperArray[i]);

    myservo3.write(ShoulderArray[i]);

    myservo2.write(ShoulderArray[i]);

    myservo1.write(ShoulderRotArray[i]);

previousMillis = currentMillis;
i++;
  }}else{ i=1;
   }}}




void Decode(){
 
  Serial.print("\t");
  Serial.print(cnt);
  Serial.print("\n");
}

void Wrist(){
 vals[0] = analogRead(pot4);            
 vals[0] = map(vals[0], 900, 100, 0, 180);     ///wrist
 myservo5.write(vals[0]); 
 //Serial.print(vals[0]);
 //Serial.print("\t Wrist:");
}

void Elbow(){
vals[1] = analogRead(pot3);            
vals[1] = map(vals[1], 90, 1000, 0, 180);      ///Elbow
myservo4.write(vals[1]); 
//Serial.print(vals[1]);
///Serial.print("\t Elbow:");
}

void Gripper(){
vals[2] = analogRead(pot5);            
vals[2] = map(vals[2], 300, 0, 45, 95);     ///gripper
myservo6.write(vals[2]); 
 ///Serial.print(vals[2]);
//Serial.print("\t Gripper:");
}

void Shoulder(){
vals[3] = analogRead(pot2);        
vals[3] = map(vals[3], 90, 950, 0, 180);     ////shoulder 
myservo2.write(vals[3]);
myservo3.write(vals[3]);
//Serial.print(vals[3]);
//Serial.print("\t Shoulder:");
}

void ShoulderRot (){
vals[4] = analogRead(pot1);            
vals[4] = map(vals[4], 180, 852, 0, 188);     ////shoulder rotate
myservo1.write(vals[4]);
 //Serial.print(vals[4]);
//Serial.print("\t ShoulderR:");
} 

void values(){ // records values of joints
WristArray[cnt]=vals[0];
ElbowArray[cnt]=vals[1];
GripperArray[cnt]=vals[2];
ShoulderArray[cnt]=vals[3];
ShoulderRotArray[cnt]=vals[4];
 
}

void GREENpress(){
static unsigned long last_interrupt_time = 0;
 unsigned long interrupt_time = millis();
 // If interrupts come faster than 250ms, assume it's a bounce and ignore
 if (interrupt_time - last_interrupt_time > 250) 
 {

 if(GREENmode){
  
  GREENmode=false;
}else{
  
  GREENmode=true;
}
 }
 last_interrupt_time = interrupt_time;
}


void REDpress()
{
 static unsigned long last_interrupt_time = 0;
 unsigned long interrupt_time = millis();
 // If interrupts come faster than 250ms, assume it's a bounce and ignore
 if (interrupt_time - last_interrupt_time > 250) 
 {
 if(REDmode){
  R
  REDmode=false;
}else{

  REDmode=true;
}
 }
 last_interrupt_time = interrupt_time;
}
