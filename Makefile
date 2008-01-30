GCC=c:\PROGRA~1\mingw

WinampC.dll: WinampC.o Resources.o
	$(GCC)\bin\gcc -shared -s -fno-exceptions -fno-rtti -o WinampC.dll WinampC.o Resources.o

WinampC.o: WinampC.c
	$(GCC)\bin\gcc -c -Os WinampC.c

Resources.o: Resources.rc
	$(GCC)\bin\windres -o Resources.o Resources.rc
