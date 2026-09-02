# OMNIA 🤖🌎
### Plataforma robótica de exploración cultural mediante teleoperación remota
**WRO Future Innovators 2026 · Tema: "Los Robots Conocen la Cultura" (Robots Meet Culture)**

OMNIA es un robot móvil terrestre teleoperado que permite a un usuario controlar de forma remota, a través de Internet, un robot físico ubicado en un espacio cultural o turístico real, explorando el lugar en tiempo real mediante una cámara embarcada.

> *"Explora el mundo desde cualquier lugar controlando robots reales."*

---

## ⚠️ Estado del proyecto (transparencia para el jurado)

Este repositorio distingue explícitamente entre lo que **ya está implementado y probado en el prototipo físico**, y lo que forma parte del **diseño de arquitectura / hoja de ruta** del proyecto. Esta separación sigue el criterio de documentación técnica de un Cuaderno de Ingeniería WRO: no atribuir al prototipo capacidades que aún no han sido construidas.

| Componente | Estado |
|---|---|
| Firmware ESP32 — control de motores (avance, retroceso, giro izq./der., detener) | ✅ **Implementado y probado en hardware real** |
| Chasis físico + tracción diferencial + driver L298N | ✅ **Implementado y probado en hardware real** |
| Backend Flask (servidor de control) | ✅ Implementado — funcional en red local / con ngrok |
| Frontend web (panel de control con botones/joystick) | ✅ Implementado — funcional |
| Streaming de video en tiempo real (ESP32-CAM) | 🧪 **Simulado / en desarrollo** — el código incluye la integración esperada, pendiente de calibración final de la cámara |
| Control de múltiples robots / plataforma en la nube | 🗺️ **Diseño de arquitectura futura** (no implementado aún) |
| Reconocimiento cultural por IA, realidad aumentada, audio bidireccional | 🗺️ **Hoja de ruta / mejoras futuras** |

---

## 🧩 Arquitectura general

```
USUARIO REMOTO (navegador web / celular)
        │
        ▼
Interfaz web OMNIA (HTML + CSS + JS)
        │  fetch() → peticiones HTTP
        ▼
Servidor Flask (Python) ── expuesto a Internet vía ngrok
        │  requests HTTP
        ▼
ESP32 (Wi-Fi) — servidor de comandos embebido
        │  GPIO / PWM
        ▼
Driver L298N ──► Motores DC (tracción diferencial)

ESP32-CAM ──► Streaming MJPEG ──► Frontend (visor de video)
```

---

## 🔧 Hardware utilizado

| Componente | Función |
|---|---|
| ESP32 (dev board) | Controlador central, conectividad Wi-Fi |
| ESP32-CAM (OV2640) | Captura y transmisión de video |
| Driver L298N | Control de potencia de motores (doble puente H) |
| 4x Motores DC amarillos (TT) | Tracción diferencial |
| Batería / powerbank | Alimentación autónoma |
| Chasis impreso en 3D (PLA) | Estructura mecánica |

Ver detalle completo en [`/hardware/BOM.md`](./hardware/BOM.md).

---

## 📁 Estructura del repositorio

```
OMNIA/
├── README.md
├── LICENSE
├── .gitignore
├── firmware/
│   └── omnia_esp32/
│       └── omnia_esp32.ino        # Firmware del ESP32 (control de motores)
├── backend/
│   ├── app.py                     # Servidor Flask
│   ├── requirements.txt
│   ├── templates/
│   │   └── index.html             # Interfaz web de control
│   └── static/
│       ├── style.css
│       └── script.js
├── hardware/
│   └── BOM.md                     # Lista de materiales y presupuesto
└── docs/
    ├── architecture.md            # Arquitectura técnica detallada
    └── setup_guide.md             # Guía de instalación y despliegue
```

---

## 🚀 Cómo ejecutar el proyecto

### 1. Firmware (ESP32)
1. Abre `firmware/omnia_esp32/omnia_esp32.ino` en el Arduino IDE (o PlatformIO).
2. Configura tu red Wi-Fi en las variables `WIFI_SSID` y `WIFI_PASSWORD`.
3. Ajusta los pines de conexión al driver L298N si tu cableado difiere.
4. Sube el firmware al ESP32. Anota la IP que imprime por el puerto serial.

### 2. Backend (Flask)
```bash
cd backend
pip install -r requirements.txt
python app.py
```

### 3. Exponer el servidor a Internet (pruebas de teleoperación remota)
```bash
ngrok http 5000
```
Comparte el enlace generado por ngrok para controlar el robot desde cualquier lugar.

### 4. Abrir el panel de control
Entra a `http://localhost:5000` (o al enlace de ngrok) y usa los botones/joystick de la interfaz web.

Guía paso a paso completa en [`/docs/setup_guide.md`](./docs/setup_guide.md).

---

## 👥 Equipo

| Integrante | Rol |
|---|---|
| Fabricio Fabian Cabrera Cespedes | Arquitectura de teleoperación, software, backend, plataforma web |
| Fabricio Rueda Zapata | Diseño mecánico, hardware, construcción y pruebas físicas |

Universidad de Ingeniería y Tecnología (UTEC) — Lima, Perú.

---

## 📄 Licencia

Este proyecto se distribuye bajo licencia MIT. Ver [`LICENSE`](./LICENSE).
