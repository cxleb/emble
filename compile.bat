@echo off
bin\nasm -fwin64 output.asm
link output.obj legacy_stdio_definitions.lib msvcrt.lib /SUBSYSTEM:CONSOLE