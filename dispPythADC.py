"""
File:   dispPythADC.py
Author: Uchenna Osemeka, Sol
Created on November 28th, 2025
"""

import serial
import time
import csv
import matplotlib.pyplot as plt
from collections import deque
from queue import Queue, Empty
import threading

# ======================
# USER CONFIG
# ======================
PORT = "COM8"
BAUD = 4800
ADC_MAX = 1023
VREF = 3.3
WINDOW = 30
CSV_FILE = "ADCShow.csv"

# ======================
# SETUP CSV
# ======================
def setup_csv():
    with open(CSV_FILE, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["Time (s)", "ADC Value", "Voltage (V)"])
    print(f"[OK] CSV log created: {CSV_FILE}")

# ======================
# SERIAL READER THREAD
# ======================
def serial_reader(plot_queue, csv_queue, stop_event):
    while not stop_event.is_set():
        try:
            # Open and close serial each iteration
            with serial.Serial(PORT, BAUD, timeout=1) as ser:
                line = ser.readline().decode(errors="ignore").strip()
                if line and line.lstrip().isdigit():
                    adc = int(line)
                    t = time.time()
                    V = (adc / ADC_MAX) * VREF
                    plot_queue.put((t, adc, V))
                    csv_queue.put((t, adc, V))  # for CSV logging
        except Exception as e:
            print("[ERROR] Serial read:", e)
        time.sleep(0.01)  # small delay to avoid hogging CPU

# ======================
# CSV LOGGER THREAD
# ======================
def csv_logger(csv_queue, stop_event):
    with open(CSV_FILE, "a", newline="") as f:
        writer = csv.writer(f)
        while not stop_event.is_set() or not csv_queue.empty():
            try:
                t, adc, V = csv_queue.get(timeout=0.1)
                writer.writerow([t, adc, V])
            except Empty:
                continue

# ======================
# MAIN FUNCTION
# ======================
def main():
    setup_csv()

    times = deque(maxlen=WINDOW)
    adc_values = deque(maxlen=WINDOW)
    voltages = deque(maxlen=WINDOW)

    plot_queue = Queue()
    csv_queue = Queue()
    stop_event = threading.Event()

    # Start threads
    thread_serial = threading.Thread(
    target=serial_reader, 
    args=(plot_queue, csv_queue, stop_event), 
    daemon=True)
    thread_serial.start()

    thread_csv = threading.Thread(target=csv_logger, args=(csv_queue, stop_event), daemon=True)
    thread_csv.start()

    # Setup plots
    plt.ion()
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(8, 6))
    line_adc, = ax1.plot([], [], label="ADC Value")
    line_volt, = ax2.plot([], [], label="Voltage (V)")

    ax1.set_ylabel("ADC Value")
    ax1.set_xlabel("Time (s)")
    ax1.legend(loc="upper right")
    ax1.grid(True)

    ax2.set_ylabel("Voltage (V)")
    ax2.set_xlabel("Time (s)")
    ax2.legend(loc="upper right")
    ax2.grid(True)
    
    # For ADC plot
    ax1.set_ylim(0, ADC_MAX+35)    # 0 to 1023+35
    

    # For Voltage plot
    ax2.set_ylim(0, VREF+0.7)       # 0 to 3.3V+ 0.7


    start_time = time.time()
    print("[READY] Reading, logging, and plotting...  (Ctrl-C to stop)")

    try:
        while True:
            try:
                t, adc, V = plot_queue.get(timeout=1.5)
            except Empty:
                plt.pause(0.1)
                continue

            # Adjust relative time
            t_rel = t - start_time

            # Update deques
            times.append(t_rel)
            adc_values.append(adc)
            voltages.append(V)

            # Update plots
            line_adc.set_xdata(times)
            line_adc.set_ydata(adc_values)
            ax1.relim()
            ax1.autoscale_view(scalex=True, scaley=False)  # only auto-scale x-axis
            

            line_volt.set_xdata(times)
            line_volt.set_ydata(voltages)
            ax2.relim()
            ax2.autoscale_view(scalex=True, scaley=False)  # only auto-scale x-axis
            
            
            plt.draw()
            plt.pause(0.1)

    except KeyboardInterrupt:
        print("\n[EXIT] User stopped program.")
    finally:
        stop_event.set()
        thread_serial.join()
        thread_csv.join()
        plt.ioff()
        print(f"[OK] Threads stopped. CSV saved: {CSV_FILE}")


if __name__ == "__main__":
    main()

