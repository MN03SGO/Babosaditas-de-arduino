
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
const int LED_RED = 12;

void setup() {
  pinMode(LED_RED, OUTPUT);
  Serial.begin(115200);
  SerialBT.begin("ESP32_LED");
  Serial.println("El Bluetooth ha iniciado y está listo para emparejarse");
}

void loop() {
  if (SerialBT.available()) {
    char DatoRecibo = SerialBT.read();
    
    if (DatoRecibo == 'A') {
      digitalWrite(LED_RED, HIGH);
      Serial.println("LED Encendido");
    } else if (DatoRecibo == 'B') {
      digitalWrite(LED_RED, LOW);
      Serial.println("LED Apagado");
    }
  }
}
