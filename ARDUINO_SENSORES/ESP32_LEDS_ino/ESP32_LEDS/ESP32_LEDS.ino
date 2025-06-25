const int leds[] = {25, 26, 27, 14, 12, 33, 32}; // LEDs para cuartos 1 a 7
const int motorPin = 4; // Pin para ventilador

void setup() {
  Serial.begin(115200); // Velocidad serial (debe coincidir con Processing)
  for (int i = 0; i < 7; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }
  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, LOW);
}

void loop() {
  if (Serial.available()) {
    char comando = Serial.read();
    Serial.print("Comando recibido: ");
    Serial.println(comando); // Para depurar desde el Monitor Serial

    if (comando >= 'A' && comando <= 'G') {
      int index = comando - 'A';
      digitalWrite(leds[index], HIGH);
    } 
    else if (comando >= 'a' && comando <= 'g') {
      int index = comando - 'a';
      digitalWrite(leds[index], LOW);
    }
    else if (comando == 'V') {
      digitalWrite(motorPin, HIGH);
    } 
    else if (comando == 'v') {
      digitalWrite(motorPin, LOW);
    }
  }
}
