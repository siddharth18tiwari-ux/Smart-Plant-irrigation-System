import tkinter as tk
from tkinter import ttk
import serial
import threading
import time

# ----- Configure Serial Port -----
ser = serial.Serial('COM3', 9600, timeout=1)
time.sleep(2)  # wait for Arduino

# ----- GUI -----
root = tk.Tk()
root.title("Irrigation System Monitor")
root.geometry("400x350")

tank_var = tk.IntVar()
plant_vars = [tk.IntVar() for _ in range(3)]
current_plant_var = tk.StringVar(value="None")

MAX_SENSOR = 1023
MOISTURE_THRESHOLD = 500  # full bar at <= 500

# Tank Level
tk.Label(root, text="Water Tank Level:").pack()
tank_progress = ttk.Progressbar(root, maximum=1000, variable=tank_var, length=300)
tank_progress.pack(pady=5)

# Plant Levels (Full bar at <= 500)
plant_progress = []
for i in range(3):
    tk.Label(root, text=f"Plant {i+1} Moisture ").pack()
    prog = ttk.Progressbar(root, maximum=MAX_SENSOR, variable=plant_vars[i], length=300)
    prog.pack(pady=2)
    plant_progress.append(prog)

# Current Plant
tk.Label(root, text="Currently Watering:").pack(pady=10)
current_label = tk.Label(root, textvariable=current_plant_var, font=("Helvetica", 12, "bold"))
current_label.pack()

# Buttons
def start_simulation():
    ser.write(b's')

def refill_tank():
    ser.write(b'r')

tk.Button(root, text="Start Simulation", command=start_simulation).pack(pady=5)
tk.Button(root, text="Refill Tank", command=refill_tank).pack(pady=5)

# ----- Serial Reading Thread -----
def read_serial():
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').strip()
            if "Plant" in line and "moisture" in line:
                try:
                    parts = line.split("|")
                    plant_part = parts[0].strip()
                    tank_part = parts[1].strip()
                    plant_index = int(plant_part.split()[1]) - 1
                    moisture_val = int(plant_part.split(":")[1].strip())
                    tank_val = int(tank_part.split(":")[1].strip())

                    # Map moisture to bar: full bar at <= 500, zero at 1023
                    if moisture_val <= MOISTURE_THRESHOLD:
                        bar_val = MAX_SENSOR  # full bar
                    else:
                        # Linear scaling between 500 and 1023
                        bar_val = int(MAX_SENSOR * (1 - (moisture_val - MOISTURE_THRESHOLD) / (MAX_SENSOR - MOISTURE_THRESHOLD)))

                    plant_vars[plant_index].set(bar_val)
                    tank_var.set(tank_val)
                    current_plant_var.set(f"Plant {plant_index + 1}")
                except:
                    pass
            if "fully watered" in line or "All plants" in line or "Tank empty" in line:
                current_plant_var.set("None")
        time.sleep(0.1)

thread = threading.Thread(target=read_serial, daemon=True)
thread.start()

root.mainloop()
