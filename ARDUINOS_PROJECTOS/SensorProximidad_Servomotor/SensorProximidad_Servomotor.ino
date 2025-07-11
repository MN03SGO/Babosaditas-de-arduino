#include<Servo.h>
Servo myServo;
int TRIG = 8;
int ECO = 9;
int tiempo;
int distancia;
void setup() {
   pinMode(2,OUTPUT);
   pinMode(3,OUTPUT);
   pinMode(TRIG,OUTPUT);e
   pinMode(ECO,INPUT);
   myServo.attach(5);
   onOffLeds();
   Serial.begin(9600);
}
void loop() {
 leerDistancia();
 if(distancia<10){
  onOffLeds();
  abrirPuerta();
  delay(1000);
  cerrarPuerta();d
  delay(4000);
  }
}
void leerDistancia(){
  digitalWrite(TRIG,HIGH);
  delay(1);
  digitalWrite(TRIG,LOW);
  tiempo = pulseIn(ECO,HIGH);
  distancia = tiempo / 58.2;  
  } 
void onOffLeds(){
  digitalWrite(2,HIGH);
  digitalWrite(3,HIGH);
  delay(100);
  digitalWrite(2,LOW);
  digitalWrite(3,LOW);
  delay(100);
  digitalWrite(2,HIGH);
  digitalWrite(3,HIGH);
  delay(100);
  digitalWrite(2,LOW);
  digitalWrite(3,LOW);
  }
void cerrarPuerta(){
  digitalWrite(2,LOW);
  digitalWrite(3,HIGH);
  int angulo=0;
  do{
    angulo=angulo+1;
    myServo.write(angulo);
    delay(50);
    }while(angulo<90);
  digitalWrite(2,LOW);
  digitalWrite(3,LOW);    
}
void abrirPuerta(){
  digitalWrite(2,HIGH);
  digitalWrite(3,LOW);
  int angulo =90;
  do{
    angulo=angulo-1;
    myServo.write(angulo);
    delay(50);
    }while(angulo>0);  
  digitalWrite(2,LOW);
  digitalWrite(3,LOW);
  }