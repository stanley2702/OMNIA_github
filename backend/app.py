"""
OMNIA — Backend (Flask)
----------------------------------------------------
Estado: IMPLEMENTADO. Sirve la interfaz web de control
y reenvía los comandos de movimiento al ESP32 sobre la
red local (o mediante ngrok cuando se controla a distancia).

El streaming de video (ESP32-CAM) se deja como integración
esperada / en desarrollo: la ruta /video_feed reenvía el
stream MJPEG del ESP32-CAM cuando este está disponible en
la red; si no lo está, sirve una imagen de reemplazo.

WRO Future Innovators 2026 — Proyecto OMNIA
Equipo: Fabricio Fabian Cabrera Cespedes / Fabricio Rueda Zapata
"""

from flask import Flask, render_template, request, jsonify, Response
import requests

app = Flask(__name__)

# ----------------- CONFIGURACIÓN -----------------
# IP local del robot en la red WiFi (impresa por el ESP32 al conectarse).
ROBOT_IP = "192.168.1.50"
ROBOT_HTTP_PORT = 80
ROBOT_BASE_URL = f"http://{ROBOT_IP}:{ROBOT_HTTP_PORT}"

# IP/puerto del stream de la ESP32-CAM (si está en la red).
CAMERA_STREAM_URL = f"http://{ROBOT_IP}:81/stream"

VALID_DIRECTIONS = {"forward", "backward", "left", "right", "stop"}


# ----------------- RUTAS DE INTERFAZ -----------------
@app.route("/")
def index():
    return render_template("index.html", robot_ip=ROBOT_IP)


# ----------------- API DE CONTROL -----------------
@app.route("/api/move", methods=["POST"])
def move():
    data = request.get_json(silent=True) or {}
    direction = data.get("dir")

    if direction not in VALID_DIRECTIONS:
        return jsonify({"status": "error", "message": "Dirección inválida"}), 400

    try:
        resp = requests.get(f"{ROBOT_BASE_URL}/move", params={"dir": direction}, timeout=2)
        return jsonify({"status": "ok", "dir": direction, "robot_response": resp.text}), resp.status_code
    except requests.exceptions.RequestException as e:
        # El robot no respondió: se informa al usuario en vez de fallar silenciosamente
        return jsonify({"status": "error", "message": f"No se pudo contactar al robot: {e}"}), 503


@app.route("/api/speed", methods=["POST"])
def set_speed():
    data = request.get_json(silent=True) or {}
    value = data.get("value", 200)

    try:
        resp = requests.get(f"{ROBOT_BASE_URL}/speed", params={"value": value}, timeout=2)
        return jsonify({"status": "ok", "robot_response": resp.text}), resp.status_code
    except requests.exceptions.RequestException as e:
        return jsonify({"status": "error", "message": f"No se pudo contactar al robot: {e}"}), 503


@app.route("/api/status")
def robot_status():
    try:
        resp = requests.get(f"{ROBOT_BASE_URL}/status", timeout=2)
        return jsonify(resp.json()), resp.status_code
    except requests.exceptions.RequestException:
        return jsonify({"status": "offline"}), 503


# ----------------- VIDEO (EN DESARROLLO) -----------------
@app.route("/video_feed")
def video_feed():
    """
    Reenvía el stream MJPEG de la ESP32-CAM.
    NOTA: esta función asume que la cámara ya está calibrada y
    disponible en la red. Actualmente en fase de pruebas.
    """
    try:
        upstream = requests.get(CAMERA_STREAM_URL, stream=True, timeout=3)
        return Response(
            upstream.iter_content(chunk_size=1024),
            content_type=upstream.headers.get("Content-Type", "multipart/x-mixed-replace"),
        )
    except requests.exceptions.RequestException:
        # Fallback: la cámara aún no está disponible en esta etapa del prototipo
        return Response("Cámara no disponible todavía (integración en desarrollo)", status=503)


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, threaded=True)
