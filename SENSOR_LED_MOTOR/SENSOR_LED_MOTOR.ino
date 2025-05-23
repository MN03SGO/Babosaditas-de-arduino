const int Trig_Sensor = 9;
const int Echo_Sensor = 10;
const int Motor = 8;

//BUZZ
const int BUZZ = 11;
//LED
const int LED = 13;


// SEBSOR
long duracion; 
int distancia;
int distancia_segura;


void setup() {
  pinMode(Trig_Sensor, OUTPUT);
  pinMode(Echo_Sensor,INPUT);
  pinMode(BUZZ, OUTPUT);
  pinMode(LED, OUTPUT);

  //Motor
  pinMode(Motor,OUTPUT);

  Serial.begin(9600);

}

void loop() {
  digitalWrite(Trig_Sensor,LOW);
  delayMicroseconds(2);



  // 
  digitalWrite(Trig_Sensor,HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig_Sensor,LOW);

  duracion = pulseIn(Echo_Sensor, HIGH);
  distancia = duracion*0.034/2;

  distancia_segura = distancia;
  if (distancia_segura <= 5){
    digitalWrite(BUZZ, HIGH);
    digitalWrite(LED, HIGH);
    digitalWrite(Motor,HIGH);

  }else{
    digitalWrite(BUZZ, LOW);
    digitalWrite(LED, LOW);
    digitalWrite(Motor,LOW);

  }
 

}
