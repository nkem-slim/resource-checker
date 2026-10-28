# CPython Native Linux System Resource Monitor

A real-time terminal system resource monitoring tool built using a **CPython Native C Extension**. Instead of relying on heavy third-party Python frameworks, this tool shifts hardware data collection down to compiled C logic that directly reads the Linux kernel subsystem (`/proc`).

---

## Folder Structure
```text
my_resource_tool/
├── sysmon.c             # Native C extension code (interacts with Linux kernel)
├── setup.py             # Build engine blueprint linking C to CPython
└── monitor.py           # Dashboard interface logic and loop execution
```

---

## Compilation & Run Commands

### Step 1: Compile the C Extension Module
To compile the raw `sysmon.c` file into a binary module that Python can import, run the following command in your terminal:

```bash
python3 setup.py build_ext --inplace
```

#### Command & Flag Breakdowns:
* **`python3 setup.py`**: Runs the compilation setup script using the Python 3 interpreter.
* **`build_ext`**: A command option instructing `setuptools` to invoke your system's C compiler (like `gcc`) to build the listed binary extensions.
* **`--inplace`**: A flag instructing the compiler to drop the resulting binary file (`.so`) directly into your current working directory next to your scripts, rather than hiding it inside a separate distribution build folder.

### Step 2: Launch the Resource Dashboard
Once the binary module finishes compiling, launch your terminal application by running:

```bash
python3 monitor.py
```

---

## How It Works Under the Hood

1. **`sysmon.c` (The C Layer)**: Speaks directly to the Linux Kernel files (`/proc/stat` and `/proc/meminfo`). It strips out text strings and converts raw hardware values into safe, native Python data structures using CPython's `Py_BuildValue` library engine.
2. **`setup.py` (The Compiler Link)**: Binds the C module structure to your active Python interpreter headers, generating a high-performance shared object (`.so`) file.
3. **`monitor.py` (The Interface Loop)**: Imports your custom compiled module just like any ordinary python file (`import sysmon`). It schedules an ongoing loop to pull metric updates, runs delta equations to derive active usage percentiles, and redraws the display bars cleanly every second.
# Resource Checker
