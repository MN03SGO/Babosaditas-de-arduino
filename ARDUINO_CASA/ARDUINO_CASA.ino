const int pinSonido = 7;
bool estadoAnterior = LOW;

void setup() {
  Serial.begin(9600);
  pinMode(pinSonido, INPUT);
}

void loop() {
  bool estadoActual = digitalRead(pinSonido);

  if (estadoActual == HIGH && estadoAnterior == LOW) {
    Serial.println("PALMADA");
    delay(100);
  }

  estadoAnterior = estadoActual;
}
