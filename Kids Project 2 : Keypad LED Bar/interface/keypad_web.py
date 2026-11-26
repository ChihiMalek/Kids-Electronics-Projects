"""
=====================================================
 KIDS PROJECT #2 : WEB INTERFACE (Python + NiceGUI)
=====================================================

A Python program that opens a web page in your browser:
  - The 9 virtual LEDs light up at the same time as the real LEDs.
  - Click the virtual keypad to control the real LEDs.
  - Shows the last key pressed, the mode and the game score.

Why this method?
  The Arduino is read by Python in the background (a "thread").
  The web page simply refreshes the display 10 times per second.
  The two parts are independent, so the page never freezes.

Requirements:
  pip install nicegui pyserial

How to use:
  1. Upload Kids_Project_2_Keypad_LEDs.ino to the Arduino.
  2. Close the Arduino IDE Serial Monitor (the port must be free).
  3. Run:  python keypad_web.py
  4. The page opens in your browser (http://localhost:8090).
  5. Choose the COM port and click "Connect".

Messages sent by the Arduino (one per line):
  L:<mask>   LEDs state (bit 0 = LED 1 ... bit 8 = LED 9)
  K:<key>    key pressed
  M:FREE / M:GAME   current mode
  S:<score>  game score
  R:OK / R:FAIL     game answer result

Author: Malek Chihi
"""

import threading

import serial
import serial.tools.list_ports
from nicegui import ui

BAUD_RATE = 9600
NUM_LEDS = 9

LED_ON = "#ffd23f"
LED_OFF = "#3b3b4f"

# ---------- Shared data ----------
# The reading thread WRITES here, the web page only READS here.
state = {
    "leds": 0,          # LEDs mask
    "key": "-",         # last key
    "mode": "FREE",
    "score": "0",
    "result": "",
    "raw": "(nothing received yet)",
    "connected": False,
    "status": "Not connected",
}
ser = None   # serial connection


# ---------- Serial communication ----------
def read_serial(port):
    """Runs in the background: reads the Arduino byte by byte."""
    global ser
    line = b""
    while state["connected"]:
        try:
            byte = port.read(1)          # waits up to 0.2 s for one byte
        except Exception as e:
            state["status"] = f"Connection lost: {e}"
            state["connected"] = False
            break
        if not byte:
            continue
        if byte == b"\n":
            handle_line(line.decode(errors="ignore").strip())
            line = b""
        else:
            line += byte


def handle_line(line):
    """Updates the shared data with one line from the Arduino."""
    if not line:
        return
    state["raw"] = line
    code, _, value = line.partition(":")
    code = code[-1:]                     # ignores noise at start-up
    value = value.strip()
    if code == "L" and value.isdigit():
        state["leds"] = int(value)
    elif code == "K":
        state["key"] = value
    elif code == "M":
        state["mode"] = value
        state["result"] = "Count the LEDs!" if value == "GAME" else ""
    elif code == "S":
        state["score"] = value
    elif code == "R":
        state["result"] = "Well done!" if value == "OK" else "Try again!"


def connect(port):
    global ser
    if state["connected"]:               # already connected -> disconnect
        disconnect()
        return
    if not port:
        ui.notify("Choose a port first", type="warning")
        return
    try:
        ser = serial.Serial(port, BAUD_RATE, timeout=0.2, write_timeout=1)
    except serial.SerialException as e:
        ui.notify(f"Error: {e} (close the Serial Monitor)", type="negative")
        return
    state["connected"] = True
    state["status"] = f"Connected to {port}"
    threading.Thread(target=read_serial, args=(ser,), daemon=True).start()


def disconnect():
    global ser
    state["connected"] = False
    if ser:
        ser.close()
    ser = None
    state["status"] = "Not connected"


def send_key(key):
    if ser and state["connected"]:
        try:
            ser.write(key.encode())
        except serial.SerialTimeoutException:
            ui.notify("The Arduino did not accept the key, try again", type="warning")
    else:
        ui.notify("Connect the Arduino first", type="warning")


def list_ports():
    return [p.device for p in serial.tools.list_ports.comports()]


# ---------- Web page ----------
@ui.page("/")
def main_page():
    ui.dark_mode(True)
    ui.query("body").style("background-color: #1e1e2e")

    with ui.column().classes("w-full items-center gap-4 p-4"):
        ui.label("KEYPAD LED BAR").classes("text-4xl font-bold text-white")
        ui.label("Kids Project #2").classes("text-gray-400")

        # Connection bar
        with ui.row().classes("items-center gap-2"):
            ports = list_ports()
            port_select = ui.select(ports, value=ports[0] if ports else None,
                                    label="Port").classes("w-40")

            def refresh_ports():
                port_select.options = list_ports()
                port_select.update()

            ui.button("Refresh", on_click=refresh_ports).props("flat")
            connect_btn = ui.button("Connect", on_click=lambda: connect(port_select.value))
        status_label = ui.label().classes("text-sm")

        # LEDs
        with ui.card().classes("p-6").style("background-color: #2a2a3d"):
            with ui.row().classes("gap-4"):
                led_circles = []
                for i in range(NUM_LEDS):
                    with ui.column().classes("items-center gap-1"):
                        circle = ui.element("div").style(
                            f"width: 48px; height: 48px; border-radius: 50%; "
                            f"background-color: {LED_OFF}; transition: background-color 0.1s")
                        ui.label(str(i + 1)).classes("text-gray-400 text-sm")
                    led_circles.append(circle)

        # Info boxes
        with ui.row().classes("gap-4"):
            info = {}
            for name in ["Last key", "Mode", "Score"]:
                with ui.card().classes("items-center px-8 py-3").style("background-color: #2a2a3d"):
                    ui.label(name).classes("text-gray-400 text-sm")
                    info[name] = ui.label("-").classes("text-3xl font-bold text-white")

        result_label = ui.label().classes("text-2xl font-bold")

        # Virtual keypad
        with ui.grid(columns=3).classes("gap-3"):
            for key in ["1", "2", "3", "4", "5", "6", "7", "8", "9", "*", "0", "#"]:
                color = "red" if key in "*#" else "blue"
                ui.button(key, color=color, on_click=lambda k=key: send_key(k)) \
                    .classes("w-20 h-16 text-2xl")

        raw_label = ui.label().classes("text-xs text-gray-500 font-mono")
        ui.label("1-9 : light LEDs  |  0 : all OFF  |  * : animation  |  # : counting game") \
            .classes("text-sm text-gray-400")

    def refresh_display():
        """Called 10 times per second: copies the shared data to the page."""
        mask = state["leds"]
        for i, circle in enumerate(led_circles):
            color = LED_ON if (mask >> i) & 1 else LED_OFF
            circle.style(f"background-color: {color}")
        info["Last key"].set_text(state["key"])
        info["Mode"].set_text(state["mode"])
        info["Score"].set_text(state["score"])

        result = state["result"]
        result_label.set_text(result)
        result_label.style("color: #7CFC9A" if result == "Well done!" else
                           "color: #ff8080" if result == "Try again!" else "color: white")

        status_label.set_text(state["status"])
        status_label.style("color: #7CFC9A" if state["connected"] else "color: #aaaaaa")
        connect_btn.set_text("Disconnect" if state["connected"] else "Connect")
        raw_label.set_text(f"Arduino : {state['raw']}")

    ui.timer(0.1, refresh_display)


if __name__ in {"__main__", "__mp_main__"}:
    ui.run(title="Kids Project #2 - Keypad LED Bar", port=8090, reload=False)
