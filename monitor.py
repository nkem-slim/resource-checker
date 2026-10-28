import os
import time
import sys
import sysmon # to import the compiled binary extension directly

def format_bytes(b):
    for unit in ['B', 'KB', 'MB', 'GB']:
        if b < 1024:
            return f"{b:.2f} {unit}"
        b /= 1024
    return f"{b:.2f} TB"

def draw_bar(percent, length=20):
    filled = int(length * percent // 100)
    return f"[{'😆' * filled}{'-' * (length - filled)}] {percent:.1f}%"

def main():
    print("Initializing Core CPython Hardware Extension...")

    # Take initial C level CPU ticks snapshot
    last_idle, last_total = sysmon.get_cpu_ticks()

    try:
        while True:
            time.sleep(1.0)

            # 1. Compute delta cycles instantly via C allocations
            idle, total = sysmon.get_cpu_ticks()
            diff_idle = idle - last_idle
            diff_total = total - last_total

            cpu_pct = (1.0 - (diff_idle / diff_total)) * 100 if diff_total > 0 else 0.0
            last_idle, last_total = idle, total

            # 2. Extract Memory directly from C structures
            ram = sysmon.get_mem_bytes()

            # Render interface
            os.system('clear')
            print("=" * 55)
            print("         CPYTHON COMPILED EXTENSION ENGINE         ")
            print("=" * 55)
            print(f" CPU Usage: {draw_bar(cpu_pct)}")
            print(f" RAM Memory: {draw_bar(ram['percent'])}")
            print(f"             ({format_bytes(ram['used'])} / {format_bytes(ram['total'])})")
            print("=" * 55)
            print(" Press Ctrl+C to disconnect core threads safely.")

    except KeyboardInterrupt:
        print("\nExtension module disconnected safely.")

if __name__ == "__main__":
    main()
        
