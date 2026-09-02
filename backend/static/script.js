/*
  OMNIA — Lógica de la interfaz web
  Envía comandos de movimiento al backend Flask, que a su vez
  los reenvía al ESP32. Implementado y funcional.
*/

const buttons = {
  forward:  document.getElementById("btnForward"),
  backward: document.getElementById("btnBackward"),
  left:     document.getElementById("btnLeft"),
  right:    document.getElementById("btnRight"),
  stop:     document.getElementById("btnStop"),
};

async function sendMove(direction) {
  try {
    const res = await fetch("/api/move", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ dir: direction }),
    });
    const data = await res.json();
    if (data.status !== "ok") {
      console.warn("Comando no confirmado por el robot:", data.message);
    }
  } catch (err) {
    console.error("Error de red al enviar comando:", err);
  }
}

// Botones: mantener presionado = mover, soltar = detener
function bindHoldControl(button, direction) {
  const start = (e) => { e.preventDefault(); sendMove(direction); };
  const stop  = (e) => { e.preventDefault(); sendMove("stop"); };

  button.addEventListener("mousedown", start);
  button.addEventListener("touchstart", start);
  button.addEventListener("mouseup", stop);
  button.addEventListener("mouseleave", stop);
  button.addEventListener("touchend", stop);
}

bindHoldControl(buttons.forward, "forward");
bindHoldControl(buttons.backward, "backward");
bindHoldControl(buttons.left, "left");
bindHoldControl(buttons.right, "right");
buttons.stop.addEventListener("click", () => sendMove("stop"));

// Control de velocidad
const speedSlider = document.getElementById("speedSlider");
speedSlider.addEventListener("change", async () => {
  try {
    await fetch("/api/speed", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ value: parseInt(speedSlider.value, 10) }),
    });
  } catch (err) {
    console.error("Error al actualizar velocidad:", err);
  }
});

// Estado del robot (poll cada 5s)
async function pollStatus() {
  const statusEl = document.getElementById("robotStatus");
  try {
    const res = await fetch("/api/status");
    const data = await res.json();
    if (data.status === "online") {
      statusEl.textContent = `Estado: conectado (IP ${data.ip}, velocidad ${data.speed})`;
    } else {
      statusEl.textContent = "Estado: robot no disponible";
    }
  } catch {
    statusEl.textContent = "Estado: sin conexión al servidor";
  }
}

pollStatus();
setInterval(pollStatus, 5000);

// Soporte de teclado (flechas) para pruebas de escritorio
document.addEventListener("keydown", (e) => {
  const map = { ArrowUp: "forward", ArrowDown: "backward", ArrowLeft: "left", ArrowRight: "right" };
  if (map[e.key]) sendMove(map[e.key]);
});
document.addEventListener("keyup", (e) => {
  if (["ArrowUp", "ArrowDown", "ArrowLeft", "ArrowRight"].includes(e.key)) sendMove("stop");
});
