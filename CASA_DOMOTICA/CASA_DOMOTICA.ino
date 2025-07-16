#include <Arduino.h>
#include <ESP32Servo.h>

// Pines ESP32
const int leds[]         = {25, 26, 27, 14, 12, 13}; // LEDs habitaciones
const int ventilador1    = 15;
const int ventilador2    = 5;
const int ledLuz         = 4;     // LED para fotorresistencia (indica oscuridad)
const int pinLDR         = 34;    // Foto celda analógica (ADC)
const int pinPIR         = 35;    // Sensor presencia PIR
const int ledPIR         = 18;    // LED PIR

// Garaje
const int trigPin        = 23;
const int echoPin        = 22;
const int servoPin       = 21;
const int ledGarageVerde = 33;
const int ledGarageRojo  = 32;

Servo servo;
const int anguloAbierto = 100;
const int anguloCerrado = 0;
int anguloActual = anguloCerrado;
bool puertaAbierta = false;
unsigned long tiempoUltimaDeteccion = 0;
const unsigned long tiempoEsperaCerrar = 5000;

// Variables generales
bool estados[8] = {false}; // 6 LEDs + 2 ventiladores
bool estadoSala = false;
String comando = "";

HardwareSerial ArduinoSerial(1); // UART1, para comunicación con Arduino Uno

// Temperatura y humedad recibidas
float tempArduino = 0.0;
float humArduino = 0.0;

void setup() {
  Serial.begin(115200);
  ArduinoSerial.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17

  for (int i = 0; i < 6; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }

  pinMode(ventilador1, OUTPUT);
  pinMode(ventilador2, OUTPUT);
  digitalWrite(ventilador1, LOW);
  digitalWrite(ventilador2, LOW);

  pinMode(ledLuz, OUTPUT);
  digitalWrite(ledLuz, LOW);

  pinMode(pinPIR, INPUT);
  pinMode(ledPIR, OUTPUT);
  digitalWrite(ledPIR, LOW);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  servo.attach(servoPin);
  servo.write(anguloCerrado);

  pinMode(ledGarageVerde, OUTPUT);
  pinMode(ledGarageRojo, OUTPUT);
  digitalWrite(ledGarageVerde, LOW);
  digitalWrite(ledGarageRojo, HIGH);

  Serial.println("✅ Sistema domótico ESP32 iniciado.");
}

void loop() {
  leerComandosSerial();
  leerDatosArduino();
  controlVentiladorPorTemperatura();
  manejarGaraje();
  manejarFotoCelda();
  manejarPIR();
  delay(200);
}

void leerComandosSerial() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      procesarComando(comando);
      comando = "";
    } else {
      comando += c;
    }
  }
}

void leerDatosArduino() {
  while (ArduinoSerial.available()) {
    String linea = ArduinoSerial.readStringUntil('\n');
    linea.trim();

    // Lectura de temperatura y humedad
    if (linea.startsWith("TEMP:")) {
      int posHum = linea.indexOf(",HUM:");
      if (posHum > 0) {
        String tempStr = linea.substring(5, posHum);
        String humStr = linea.substring(posHum + 5);
        tempArduino = tempStr.toFloat();
        humArduino = humStr.toFloat();
        Serial.printf("🌡️ Temp: %.1f °C, Hum: %.1f %%\n", tempArduino, humArduino);
      }
    }

    // Detección de palmada desde el Arduino Uno
    if (linea == "PALMADA") {
      estadoSala = !estadoSala;
      digitalWrite(leds[2], estadoSala ? HIGH : LOW); // D27
      estados[2] = estadoSala;
      Serial.println(estadoSala ? "🎵 Sala encendida (Arduino)" : "🎵 Sala apagada (Arduino)");
    }
  }
}

void controlVentiladorPorTemperatura() {
  if (tempArduino > 30.0) {
    digitalWrite(ventilador1, HIGH);
    estados[6] = true;
  } else {
    digitalWrite(ventilador1, LOW);
    estados[6] = false;
  }
}

void manejarGaraje() {
  long distancia = medirDistancia();
  if (!puertaAbierta && distancia > 0 && distancia < 20) {
    abrirPuerta();
    puertaAbierta = true;
    tiempoUltimaDeteccion = millis();
  }
  if (puertaAbierta && millis() - tiempoUltimaDeteccion >= tiempoEsperaCerrar) {
    cerrarPuerta();
    puertaAbierta = false;
  }
}

void manejarFotoCelda() {
  int valorLDR = analogRead(pinLDR); // 0 - 4095
  const int umbralLuz = 1200;

  Serial.printf("📷 LDR: %d\n", valorLDR); // Para depurar

  if (valorLDR < umbralLuz) {
    digitalWrite(ledLuz, HIGH); // Oscuro
  } else {
    digitalWrite(ledLuz, LOW);
  }
}

void manejarPIR() {
  bool presencia = digitalRead(pinPIR);
  digitalWrite(ledPIR, presencia ? HIGH : LOW);
}

void procesarComando(String cmd) {
  cmd.trim();
  Serial.print("📥 Comando recibido: ");
  Serial.println(cmd);

  if (cmd == "A") {
    for (int i = 0; i < 6; i++) {
      estados[i] = true;
      digitalWrite(leds[i], HIGH);
    }
    digitalWrite(ventilador1, HIGH);
    digitalWrite(ventilador2, HIGH);
    estados[6] = true;
    estados[7] = true;
    return;
  }

  if (cmd == "B") {
    for (int i = 0; i < 6; i++) {
      estados[i] = false;
      digitalWrite(leds[i], LOW);
    }
    digitalWrite(ventilador1, LOW);
    digitalWrite(ventilador2, LOW);
    estados[6] = false;
    estados[7] = false;
    return;
  }

  int coma = cmd.indexOf(',');
  if (coma > 0 && coma < cmd.length() - 1) {
    int id = cmd.substring(0, coma).toInt();
    int valor = cmd.substring(coma + 1).toInt();

    if (id >= 1 && id <= 6) {
      int i = id - 1;
      estados[i] = (valor == 1);
      digitalWrite(leds[i], estados[i] ? HIGH : LOW);
      Serial.printf("🔆 LED %d %s\n", id, estados[i] ? "encendido" : "apagado");
    }
    if (id == 7) {
      digitalWrite(ventilador1, valor ? HIGH : LOW);
      Serial.println(valor ? "🌀 Ventilador 1 encendido" : "🌀 Ventilador 1 apagado");
      estados[6] = valor;
    }
    if (id == 8) {
      digitalWrite(ventilador2, valor ? HIGH : LOW);
      Serial.println(valor ? "🌀 Ventilador 2 encendido" : "🌀 Ventilador 2 apagado");
      estados[7] = valor;
    }
  }
}

long medirDistancia() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duracion = pulseIn(echoPin, HIGH, 30000);
  if (duracion == 0) return -1;
  return duracion * 0.034 / 2;
}

void abrirPuerta() {
  Serial.println("🚗 Abriendo garaje...");
  for (int ang = anguloActual; ang <= anguloAbierto; ang += 5) {
    servo.write(ang);
    delay(10);
  }
  anguloActual = anguloAbierto;
  digitalWrite(ledGarageVerde, HIGH);
  digitalWrite(ledGarageRojo, LOW);
  Serial.println("✅ Garaje abierto");
}

void cerrarPuerta() {
  Serial.println("🚪 Cerrando garaje...");
  for (int ang = anguloActual; ang >= anguloCerrado; ang -= 5) {
    servo.write(ang);
    delay(10);
  }
  anguloActual = anguloCerrado;
  digitalWrite(ledGarageVerde, LOW);
  digitalWrite(ledGarageRojo, HIGH);
  Serial.println("✅ Garaje cerrado");
  }