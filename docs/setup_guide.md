# Guía de instalación — OMNIA

## Requisitos previos
- Arduino IDE (o PlatformIO) con el paquete de placas ESP32 instalado.
- Python 3.10+ y `pip`.
- Una cuenta gratuita de [ngrok](https://ngrok.com/) (para teleoperación fuera de la red local).
- Robot armado: ESP32, driver L298N, motores DC, batería (ver `/hardware/BOM.md`).

## 1. Cablear el driver L298N al ESP32

| L298N | ESP32 (pin sugerido) |
|---|---|
| IN1 | GPIO 26 |
| IN2 | GPIO 27 |
| ENA | GPIO 14 (PWM) |
| IN3 | GPIO 25 |
| IN4 | GPIO 33 |
| ENB | GPIO 32 (PWM) |
| GND | GND común con ESP32 y batería |

> Ajusta los números de pin en `firmware/omnia_esp32/omnia_esp32.ino` si tu cableado es diferente.

## 2. Cargar el firmware
1. Instala la librería de placas ESP32 en el Arduino IDE (Preferencias → URLs adicionales de gestor de tarjetas → agregar el índice oficial de Espressif).
2. Abre `firmware/omnia_esp32/omnia_esp32.ino`.
3. Reemplaza `WIFI_SSID` y `WIFI_PASSWORD` por los de tu red.
4. Selecciona la placa correcta (ej. "ESP32 Dev Module") y el puerto COM.
5. Sube el código.
6. Abre el Monitor Serial (115200 baudios) y copia la IP que el ESP32 imprime al conectarse — la necesitarás en el siguiente paso.

## 3. Configurar y ejecutar el backend
```bash
cd backend
python -m venv venv
source venv/bin/activate      # En Windows: venv\Scripts\activate
pip install -r requirements.txt
```

Edita `app.py` y reemplaza:
```python
ROBOT_IP = "192.168.1.50"
```
por la IP real que imprimió el ESP32.

Ejecuta el servidor:
```bash
python app.py
```

Abre `http://localhost:5000` en tu navegador — deberías ver el panel de control de OMNIA.

## 4. Probar el control remoto (fuera de tu red local)
```bash
ngrok http 5000
```
ngrok generará una URL pública (ej. `https://xxxx.ngrok-free.app`). Compártela para controlar el robot desde cualquier lugar con conexión a Internet.

## 5. Solución de problemas comunes

| Problema | Causa probable | Solución |
|---|---|---|
| El robot no responde a los comandos | IP del ESP32 mal configurada en `app.py` | Verifica la IP en el Monitor Serial y actualízala |
| El servidor se detiene al presionar un botón | Flask no está corriendo en modo persistente | Verificar que `app.run(..., threaded=True)` esté presente |
| El video no carga | La cámara ESP32-CAM aún no está calibrada/conectada | Se muestra automáticamente un placeholder; es una integración en desarrollo |
| ngrok se desconecta tras un tiempo | Límite de sesión de la cuenta gratuita | Reiniciar el túnel o usar un plan pago para sesiones prolongadas |
