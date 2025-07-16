import processing.serial.*;
import controlP5.*;

ControlP5 cp5;
Serial espPort, arduinoPort;

PFont font;

boolean[] leds = new boolean[6];
boolean[] ventiladores = new boolean[2];

float temperatura = 0.0;
boolean garajeAbierto = false;

void setup() {
  size(1000, 600);
  surface.setTitle("Dashboard Domótico - ESP32 + Arduino Uno");

  font = createFont("Arial", 16);
  textFont(font);
  cp5 = new ControlP5(this);

  println(Serial.list());

  // Asegúrate de que estos puertos sean correctos en tu sistema
  espPort = new Serial(this, "/dev/ttyUSB0", 115200);
  arduinoPort = new Serial(this, "/dev/ttyACM0", 9600);
  espPort.bufferUntil('\n');
  arduinoPort.bufferUntil('\n');

  cp5.addButton("btnEncenderTodos").setLabel("Encender LEDs").setPosition(40, 40).setSize(160, 40).setColorBackground(color(0, 180, 0));
  cp5.addButton("btnApagarTodos").setLabel("Apagar LEDs").setPosition(220, 40).setSize(160, 40).setColorBackground(color(180, 0, 0));

  cp5.addButton("btnEncenderVentiladores").setLabel("Encender Ventiladores").setPosition(500, 40).setSize(200, 40).setColorBackground(color(0, 180, 0));
  cp5.addButton("btnApagarVentiladores").setLabel("Apagar Ventiladores").setPosition(720, 40).setSize(200, 40).setColorBackground(color(180, 0, 0));
}

void draw() {
  background(30);
  fill(255);
  textSize(20);
  text("📊 Panel Domótico ESP32 + Arduino Uno", 30, 20);

  drawPanelLeds();
  drawPanelVentiladores();

  textSize(16);
  fill(255);
  text("🌡️ Temp: " + nf(temperatura, 1, 1) + " °C", 30, height - 60);

  text("🚗 Garaje: ", 300, height - 60);
  fill(garajeAbierto ? color(0, 200, 0) : color(200, 0, 0));
  text(garajeAbierto ? "ABIERTO" : "CERRADO", 380, height - 60);
}

void drawPanelLeds() {
  String[] nombres = {"Cuarto 1", "Cuarto 2", "Sala", "Cuarto 3", "Patio", "Baño"};
  int x = 40, y = 120, w = 180, h = 40, gap = 50;
  for (int i = 0; i < leds.length; i++) {
    fill(leds[i] ? color(0, 180, 0) : color(180, 0, 0));
    rect(x, y + i * gap, w, h, 10);
    fill(255);
    textAlign(CENTER, CENTER);
    text(nombres[i], x + w / 2, y + i * gap + h / 2);
  }
}

void drawPanelVentiladores() {
  String[] nombres = {"Ventilador 1 (D15)", "Ventilador 2 (D5)"};
  int x = 500, y = 120, w = 200, h = 40, gap = 50;
  for (int i = 0; i < ventiladores.length; i++) {
    fill(ventiladores[i] ? color(0, 180, 0) : color(180, 0, 0));
    rect(x, y + i * gap, w, h, 10);
    fill(255);
    textAlign(CENTER, CENTER);
    text(nombres[i], x + w / 2, y + i * gap + h / 2);
  }
}

void mousePressed() {
  for (int i = 0; i < leds.length; i++) {
    int x = 40, y = 120 + i * 50, w = 180, h = 40;
    if (mouseX > x && mouseX < x + w && mouseY > y && mouseY < y + h) {
      leds[i] = !leds[i];
      enviarComandoESP(i + 1, leds[i] ? 1 : 0);
    }
  }
  for (int i = 0; i < ventiladores.length; i++) {
    int x = 500, y = 120 + i * 50, w = 200, h = 40;
    if (mouseX > x && mouseX < x + w && mouseY > y && mouseY < y + h) {
      ventiladores[i] = !ventiladores[i];
      enviarComandoESP(i + 7, ventiladores[i] ? 1 : 0);
    }
  }
}

void serialEvent(Serial p) {
  String linea = p.readStringUntil('\n');
  if (linea != null) {
    linea = linea.trim();

    if (linea.startsWith("🌡️ Temp:")) {
      int i1 = linea.indexOf(":");
      int i2 = linea.indexOf("°");
      if (i1 > 0 && i2 > i1) {
        String tempStr = linea.substring(i1 + 1, i2).trim();
        temperatura = float(tempStr);
      }
    }
    if (linea.equals("✅ Garaje abierto")) {
      garajeAbierto = true;
    } else if (linea.equals("✅ Garaje cerrado")) {
      garajeAbierto = false;
    }
  }
}

void btnEncenderTodos() {
  for (int i = 0; i < leds.length; i++) {
    leds[i] = true;
    enviarComandoESP(i + 1, 1);
  }
}

void btnApagarTodos() {
  for (int i = 0; i < leds.length; i++) {
    leds[i] = false;
    enviarComandoESP(i + 1, 0);
  }
}

void btnEncenderVentiladores() {
  for (int i = 0; i < ventiladores.length; i++) {
    ventiladores[i] = true;
    enviarComandoESP(i + 7, 1);
  }
}

void btnApagarVentiladores() {
  for (int i = 0; i < ventiladores.length; i++) {
    ventiladores[i] = false;
    enviarComandoESP(i + 7, 0);
  }
}

void enviarComandoESP(int id, int valor) {
  String cmd = id + "," + valor + "\n";
  espPort.write(cmd);
  println("🛰️ Enviando a ESP32: " + cmd.trim());
}
