# Arquitectura Técnica — OMNIA

## 1. Visión general

OMNIA opera bajo un modelo cliente-servidor de dos rutas complementarias:

- **Ruta de comando:** transporta las instrucciones de movimiento desde el usuario hasta los motores del robot.
- **Ruta de retroalimentación:** transporta el video capturado por la cámara del robot de vuelta hacia el usuario.

```
USUARIO REMOTO
   │  interacción (botones / joystick / teclado)
   ▼
Frontend (HTML/CSS/JS)
   │  fetch() → POST /api/move, /api/speed, GET /api/status
   ▼
Backend Flask
   │  requests → GET /move?dir=..., /speed?value=..., /status
   ▼
ESP32 (WebServer embebido)
   │  GPIO digital + PWM (LEDC)
   ▼
Driver L298N
   │  corriente controlada
   ▼
Motores DC (tracción diferencial)

ESP32-CAM ──(MJPEG stream, puerto 81)──► Backend Flask (/video_feed) ──► Frontend (<img>)
```

## 2. Protocolo de comunicación (estado actual)

Comunicación por HTTP (solicitud-respuesta), elegida por simplicidad para esta primera versión del prototipo.

| Endpoint (ESP32) | Método | Parámetros | Efecto |
|---|---|---|---|
| `/move` | GET | `dir=forward\|backward\|left\|right\|stop` | Ejecuta el movimiento correspondiente |
| `/speed` | GET | `value=0-255` | Ajusta la velocidad PWM de los motores |
| `/status` | GET | — | Devuelve estado del robot en JSON |

| Endpoint (Backend) | Método | Función |
|---|---|---|
| `/` | GET | Sirve la interfaz web |
| `/api/move` | POST | Reenvía comando de movimiento al ESP32 |
| `/api/speed` | POST | Reenvía ajuste de velocidad al ESP32 |
| `/api/status` | GET | Consulta estado del robot |
| `/video_feed` | GET | Reenvía (proxy) el stream MJPEG de la ESP32-CAM |

## 3. Mejora planificada: WebSocket

El modelo HTTP de solicitud-respuesta introduce sobrecarga de conexión en cada comando individual. Se evalúa migrar la comunicación de control hacia **WebSocket**, que mantiene una conexión persistente y bidireccional, reduciendo la latencia percibida durante el control continuo.

## 4. Comunicación a larga distancia

Para pruebas de teleoperación fuera de la red local se utiliza **ngrok**, que publica temporalmente el servidor Flask hacia Internet sin requerir configuración de router o IP pública.

Tecnologías evaluadas para una versión de producción a mayor escala:
- **MQTT** — mensajería ligera para comandos entre múltiples robots y la plataforma central.
- **WebRTC** — comunicación en tiempo real para reducir la latencia de video.
- **ROS2** — modularización del software del robot en nodos independientes.
- **Cloud Robotics** — cómputo y coordinación distribuidos en la nube para múltiples robots.

## 5. Seguridad ante pérdida de conexión (principio de diseño)

Ante una desconexión, el robot debe:
1. Detener funciones críticas de movimiento.
2. Mantener activos los sensores de seguridad disponibles.
3. Conservar el último estado válido.
4. Intentar reconectarse automáticamente.
5. Evitar movimientos no solicitados por el usuario.

*(Este comportamiento forma parte del diseño de arquitectura; su implementación completa en firmware está en la hoja de ruta del proyecto.)*
