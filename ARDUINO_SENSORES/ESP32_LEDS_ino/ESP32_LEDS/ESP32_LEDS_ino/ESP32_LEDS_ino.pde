import processing.serial.*;

Serial esp32;
Boton[] botones = new Boton[8]; // 7 cuartos + 1 ventilador
String[] etiquetas = {
  "Cuarto 1", "Cuarto 2", "Cuarto 3", "Cuarto 4", 
  "Cuarto 5", "Cuarto 6", "Cuarto 7", "Ventilador"
};

void setup() {
  size(700, 350);
  println("Puertos disponibles:");
  printArray(Serial.list()); // Verifica puertos disponibles

  // Asegúrate de que este índice corresponde a tu ESP32
  esp32 = new Serial(this, Serial.list()[0], 115200); 

  for (int i = 0; i < 8; i++) {
    int x = 50 + (i % 4) * 150;
    int y = 80 + (i / 4) * 120;
    char cmdOn = (i < 7) ? (char)('A' + i) : 'V';
    char cmdOff = (i < 7) ? (char)('a' + i) : 'v';
    botones[i] = new Boton(x, y, etiquetas[i], cmdOn, cmdOff);
  }
}

void draw() {
  background(30);
  for (Boton b : botones) {
    b.mostrar();
  }
}

void mousePressed() {
  for (Boton b : botones) {
    if (b.presionado(mouseX, mouseY)) {
      b.estado = !b.estado;
      char comando = b.estado ? b.cmdEncender : b.cmdApagar;
      esp32.write(comando);
      println("Enviado: " + comando); // Mostrar el comando enviado
    }
  }
}

// Prueba manual con teclado
void keyPressed() {
  if (key == 'V' || key == 'v') {
    esp32.write(key);
    println("Manual envío: " + key);
  }
}

class Boton {
  int x, y, w = 130, h = 50;
  String etiqueta;
  char cmdEncender, cmdApagar;
  boolean estado = false;

  Boton(int x, int y, String etiqueta, char cmdEncender, char cmdApagar) {
    this.x = x;
    this.y = y;
    this.etiqueta = etiqueta;
    this.cmdEncender = cmdEncender;
    this.cmdApagar = cmdApagar;
  }

  void mostrar() {
    fill(estado ? color(0, 200, 0) : color(200, 0, 0));
    rect(x, y, w, h, 12);
    fill(255);
    textAlign(CENTER, CENTER);
    textSize(14);
    text(etiqueta + (estado ? " ON" : " OFF"), x + w / 2, y + h / 2);
  }

  boolean presionado(int mx, int my) {
    return (mx > x && mx < x + w && my > y && my < y + h);
  }
}
