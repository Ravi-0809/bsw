#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <Servo.h>

#define SS_PIN D4
#define RST_PIN D3

const char* ssid = "ARC";
const char* password = "bphc@arc";

byte commands[4]={0,0,0,0};

int lmotor1 = D1;
int lmotor2 = D2;
int rmotor1 = D8;
int rmotor2 = D6;

int NEUTRAL = 1000;
int LEFT = 999;
int RIGHT = 1001;

int MOV_NEUTRAL = 10;
int FORWARD = 11;
int BACKWARD = 9;

int corr_factor = 2;

void connectToWiFi()
{
  WiFi.begin(ssid, password);
  Serial.println("Contacting Host");
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
}

void setup()
{
  //initialisations
  pinMode(lmotor1, OUTPUT);
  pinMode(lmotor2, OUTPUT);
  pinMode(rmotor1, OUTPUT);
  pinMode(rmotor2, OUTPUT);
  analogWrite(lmotor1, 0);
  analogWrite(lmotor2, 0);
  analogWrite(rmotor1, 0);
  analogWrite(rmotor2, 0); 
  
  SPI.begin();
  rfid.PCD_Init();
  Serial.begin(9600);
  delay(1000);
  Serial.println("Serial Initialized");

  connectToWiFi();
  
  Serial.println("\nConnected to ");
  Serial.println(WiFi.gatewayIP());
  Serial.println("My IP is:");
  Serial.println(WiFi.localIP());
  Serial.println("OUT");
  // positon gripper
  //pinMode(D0, OUTPUT);
  //gripper.attach(D0);
  //gripper.write(100);
}

void loop(){
  // put your main code here, to run repeatedly:
  if(Serial.available()>4)
  {
     // if the first byte read is not the first byte sent from the bluetooth skip it and read the next byte
    while(1)
    {
      commands[0] = Serial.read();
      // check if the first byte value corresponds to those sent by the app over bluetooth
      if(commands[0]==241 || commands[0]==242 || commands[0]==243)
        break;
    }
  // read the next 3 bytes as well
    commands[1] = Serial.read();  //Speed
    commands[2] = Serial.read();  //Angle
    commands[3] = Serial.read();
//  Serial.print("0 ");
//  Serial.print(commands[0]);
//  Serial.print(" 1 ");
//  Serial.print(commands[1]);
//  Serial.print(" 2 ");
//  Serial.print(commands[2]);
//  Serial.print(" 3 ");
//  Serial.println(commands[3]);

    int movDir = MOV_NEUTRAL, turnDir = NEUTRAL, scaledTurnSpeed = 0, scaledSpeed = 0;
    int recSpeed = commands[1];
    scaledSpeed = map(recSpeed, 0, 255, 0, 1023);
    int recTurnSpeed = commands[2];
    //Serial.print("recTurnSpeed\t");
    //Serial.println(recTurnSpeed);
    if(recTurnSpeed > 88){
      scaledTurnSpeed = map(recTurnSpeed - 89, 0, 21, 0, 1010);
      turnDir = RIGHT;
    }
    else if(recTurnSpeed < 88){
      scaledTurnSpeed = map(87 - recTurnSpeed, 0, 22, 0, 1010);
      turnDir = LEFT;
    }
    else{turnDir = NEUTRAL;}
    
    if(commands[0] == 241){
      movDir = FORWARD;
    }
    else if(commands[0] == 242){
      movDir = BACKWARD;
    }
    else if(commands[0] == 243){
      movDir = MOV_NEUTRAL;
    }
    leftWheel(movDir, scaledSpeed, turnDir, scaledTurnSpeed);
    rightWheel(movDir, scaledSpeed, turnDir, scaledTurnSpeed);
  }
}

void leftWheel(int mov_dir, int scaledSpeed, int turn_dir, int scaledTurn){
  if(turn_dir == NEUTRAL){
    if(mov_dir == FORWARD){
      analogWrite(lmotor1, scaledSpeed);
      analogWrite(lmotor2, 0);
    }
    else if(mov_dir == BACKWARD){
      analogWrite(lmotor2, scaledSpeed);
      analogWrite(lmotor1, 0);
    }
    else
    {
      analogWrite(lmotor2, 0);
      analogWrite(lmotor1, 0);
    }
  }
  else if(turn_dir == RIGHT){
    if(mov_dir == FORWARD){
      analogWrite(lmotor1, scaledTurn);
      analogWrite(lmotor2, 0);
    }
    else
    if(mov_dir == BACKWARD){
      analogWrite(lmotor2, scaledTurn);
      analogWrite(lmotor1, 0);
    }
  }
  else if(turn_dir == LEFT){
    if(mov_dir == FORWARD){
      analogWrite(lmotor1, scaledTurn/corr_factor);
      analogWrite(lmotor2, 0);
    }
    else
  if(mov_dir == BACKWARD){
      analogWrite(lmotor2, scaledTurn/corr_factor);
      analogWrite(lmotor1, 0);
    }
  }
}

void rightWheel(int mov_dir, int scaledSpeed, int turn_dir, int scaledTurn){
  if(turn_dir == NEUTRAL){
    if(mov_dir == FORWARD){
      analogWrite(rmotor1, scaledSpeed);
      analogWrite(rmotor2, 0); 
    }
    else if(mov_dir == BACKWARD){
      analogWrite(rmotor2, scaledSpeed);
      analogWrite(rmotor1, 0);
    }
    else
    {
      analogWrite(rmotor2, 0);
      analogWrite(rmotor1, 0);
    }
  }
  else if(turn_dir == LEFT){
    if(mov_dir == FORWARD){
      analogWrite(rmotor1, scaledTurn);
      analogWrite(rmotor2, 0);
    }
    else if(mov_dir == BACKWARD){
      analogWrite(rmotor2, scaledTurn);
      analogWrite(rmotor1, 0);
    }
    else
    {
      analogWrite(rmotor2, 0);
      analogWrite(rmotor1, 0);
    }
  }
  else if(turn_dir == RIGHT){
    if(mov_dir == FORWARD){
      analogWrite(rmotor1, scaledTurn/corr_factor);
      analogWrite(rmotor2, 0);
    }
    else if(mov_dir == BACKWARD){
      analogWrite(rmotor2, scaledTurn/corr_factor);
      analogWrite(rmotor1, 0);
    }
    else
    {
      analogWrite(rmotor2, 0);
      analogWrite(rmotor1, 0);
    }
  }
}
