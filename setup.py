from setuptools import setup, Extension

# Define your custom C-based extension package target
sysmon_extension = Extension('sysmon', sources=['sysmon.c'])

setup(
    name='SysmonEngine',
    version='1.0',
    description='CPython Native Linux Extension Tool',
    ext_modules=[sysmon_extension]
)
