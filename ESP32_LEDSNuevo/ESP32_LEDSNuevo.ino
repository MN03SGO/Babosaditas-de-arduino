#include <Arduino.h>

// Pines de los LEDs (Cuartos 1 a 7)
const int leds[] = {25, 26, 27, 14, 12, 13, 23}; 
const int motorPin = 22; // Pin del ventilador

void setup() {
  Serial.begin(115200);

  // Configurar LEDs
  for (int i = 0; i < 7; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW); // Apagados al inicio
  }

  // Configurar motor
  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, LOW);
}

void loop() {
  if (Serial.available()) {
    char comando = Serial.read();
    Serial.print("Comando recibido: ");
    Serial.println(comando);

    // Encender LEDs A-G (Cuartos 1-7)
    if (comando >= 'A' && comando <= 'G') {
      int index = comando - 'A';
      if (index >= 0 && index < 7) {
        digitalWrite(leds[index], HIGH);
      }
    } 
    // Apagar LEDs a-g
    else if (comando >= 'a' && comando <= 'g') {
      int index = comando - 'a';
      if (index >= 0 && index < 7) {
        digitalWrite(leds[index], LOW);
      }
    }
    // Encender motor (ventilador)
    else if (comando == 'V') {
      digitalWrite(motorPin, HIGH);
    }
    // Apagar motor
    else if (comando == 'v') {
      digitalWrite(motorPin, LOW);
    }
  }
}
