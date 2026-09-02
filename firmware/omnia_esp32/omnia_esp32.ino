/*
  OMNIA — Firmware ESP32
  ----------------------------------------------------
  Estado: IMPLEMENTADO Y PROBADO EN HARDWARE REAL.
  Controla el movimiento del robot (avance, retroceso,
  giro izquierda, giro derecha, detener) mediante un
  driver L298N, recibiendo comandos por HTTP en la
  red local (Wi-Fi).

  WRO Future Innovators 2026 — Proyecto OMNIA
  Equipo: Fabricio Fabian Cabrera Cespedes / Fabricio Rueda Zapata
  Universidad de Ingeniería y Tecnología (UTEC) — Lima, Perú
*/

#include <WiFi.h>
#include <WebServer.h>

// ----------------- CONFIGURACIÓN WIFI -----------------
const char* WIFI_SSID     = "TU_RED_WIFI";
const char* WIFI_PASSWORD = "TU_CONTRASENA";

// ----------------- PINES DEL DRIVER L298N -----------------
// Lado izquierdo
const int IN1 = 26;
const int IN2 = 27;
const int ENA = 14;   // PWM velocidad motor izquierdo

// Lado derecho
const int IN3 = 25;
const int IN4 = 33;
const int ENB = 32;   // PWM velocidad motor derecho

// Canales PWM (LEDC) del ESP32
const int PWM_CHANNEL_A = 0;
const int PWM_CHANNEL_B = 1;
const int PWM_FREQ      = 1000;
const int PWM_RES_BITS  = 8;      // 0-255
int currentSpeed        = 200;    // velocidad por defecto (0-255)

WebServer server(80);

// ----------------- FUNCIONES DE MOVIMIENTO -----------------
void setMotorSpeeds(int leftSpeed, int rightSpeed) {
  ledcWrite(PWM_CHANNEL_A, abs(leftSpeed));
  ledcWrite(PWM_CHANNEL_B, abs(rightSpeed));
}

void motorsStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  setMotorSpeeds(0, 0);
}

void moveForward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  setMotorSpeeds(currentSpeed, currentSpeed);
}

void moveBackward() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  setMotorSpeeds(currentSpeed, currentSpeed);
}

void turnLeft() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  setMotorSpeeds(currentSpeed, currentSpeed);
}

void turnRight() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  setMotorSpeeds(currentSpeed, currentSpeed);
}

// ----------------- ENDPOINTS HTTP -----------------
void handleMove() {
  if (!server.hasArg("dir")) {
    server.send(400, "text/plain", "Falta parametro 'dir'");
    return;
  }
  String dir = server.arg("dir");

  if (dir == "forward")       moveForward();
  else if (dir == "backward") moveBackward();
  else if (dir == "left")     turnLeft();
  else if (dir == "right")    turnRight();
  else if (dir == "stop")     motorsStop();
  else {
    server.send(400, "text/plain", "Comando no reconocido: " + dir);
    return;
  }

  server.send(200, "application/json", "{\"status\":\"ok\",\"dir\":\"" + dir + "\"}");
}

void handleSpeed() {
  if (server.hasArg("value")) {
    currentSpeed = constrain(server.arg("value").toInt(), 0, 255);
  }
  server.send(200, "application/json", "{\"status\":\"ok\",\"speed\":" + String(currentSpeed) + "}");
}

void handleStatus() {
  String json = "{\"status\":\"online\",\"ip\":\"" + WiFi.localIP().toString() + "\",\"speed\":" + String(currentSpeed) + "}";
  server.send(200, "application/json", json);
}

void handleNotFound() {
  server.send(404, "text/plain", "Ruta no encontrada");
}

// ----------------- SETUP -----------------
void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  ledcSetup(PWM_CHANNEL_A, PWM_FREQ, PWM_RES_BITS);
  ledcSetup(PWM_CHANNEL_B, PWM_FREQ, PWM_RES_BITS);
  ledcAttachPin(ENA, PWM_CHANNEL_A);
  ledcAttachPin(ENB, PWM_CHANNEL_B);

  motorsStop();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Conectado. IP del robot: ");
  Serial.println(WiFi.localIP());

  server.on("/move", HTTP_GET, handleMove);
  server.on("/speed", HTTP_GET, handleSpeed);
  server.on("/status", HTTP_GET, handleStatus);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Servidor HTTP del robot iniciado.");
}

// ----------------- LOOP -----------------
void loop() {
  server.handleClient();
}
