#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This is used to fetch the total CPU ticks straight from the kernel
static PyObject* sysmon_get_cpu_ticks(PyObject* self, PyObject* args)
{
    unsigned long long user, nice, system, idle,iowait, irq, softirq, steal;
    FILE* fp = fopen("/proc/stat", "r");
    if(!fp)
    {
        PyErr_SetString(PyExc_OSError, "Unable to read from /proc/stat");
        return NULL;
    }

    char buffer[256];
    unsigned long long total_idle = 0;
    unsigned long long total_time = 0;

    if(fgets(buffer, sizeof(buffer), fp))
    {
        if(strncmp(buffer, "cpu ", 4) == 0)
        {
            sscanf(buffer + 4, "%llu %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
            total_idle = idle + iowait; 
            total_time = user + nice + system + idle + iowait + irq + softirq + steal;
        }
    }

    fclose(fp);

    // returns a standard python tuple object containing raw unsigned long long
    return Py_BuildValue("KK", total_idle, total_time);
}


// Parses and aggregates raw allocations directly from memory nodes
static PyObject* sysmon_get_mem_bytes(PyObject* self, PyObject* args)
{
    FILE* fp = fopen("/proc/meminfo", "r");
    if(!fp)
    {
        PyErr_SetString(PyExc_OSError, "Unable to read from /proc/meminfo");
        return NULL;
    }

    char line[256];
    unsigned long long total_mem = 0;
    unsigned long long avail_mem = 0;

    while (fgets(line, sizeof(line), fp))
    {
        if(strncmp(line, "MemTotal:", 9) == 0)
        {
            sscanf(line + 9, "%llu", &total_mem);
        } else if(strncmp(line, "MemAvailable:", 13) == 0)
        {
            sscanf(line + 13, "%llu", &avail_mem);
        }
    }
    fclose(fp);

    // /proc/meminfo reports values in kB; convert them to raw bytes
    total_mem *= 1024;
    unsigned long long used_mem = (total_mem - (avail_mem * 1024));
    double percent = total_mem > 0 ? ((double)used_mem / total_mem) * 100.0 : 0.0;

    // Build a CPython native dictionary object to return safely
    return Py_BuildValue("{s:K, s:K, s:d}",
        "total", total_mem,
        "used", used_mem,
        "percent", percent    
    );
}

// Method layout profile mapping Python keywords to C entry points
static PyMethodDef SysmonMethods[] = {
    {"get_cpu_ticks", sysmon_get_cpu_ticks, METH_VARARGS, "Get total system CPU idle and total ticks."},
    {"get_mem_bytes", sysmon_get_mem_bytes, METH_VARARGS, "Get memory data mappings directly from the kernel memoery tables."},
    {NULL, NULL, 0, NULL} // Sentinal markup
};

// Blueprint declaring peroperties of the custom module structure
static struct PyModuleDef sysmonmodule = {
    PyModuleDef_HEAD_INIT,
    "sysmon",
    "CPython Native Linux Hardware Resource Engine",
    -1,
    SysmonMethods
};

// Module initializer entrypoint targeted during high-level execution imports
PyMODINIT_FUNC PyInit_sysmon(void)
{
    return PyModule_Create(&sysmonmodule);
}
