GCC=c:\PROGRA~1\mingw

WinampC.dll: WinampC.o resources.o
	$(GCC)\bin\dllwrap --dllname=WinampC.dll --driver_name=$(GCC)\bin\gcc WinampC.o resources.o
	
WinampC.o: WinampC.c
	$(GCC)\bin\gcc -c WinampC.c

resources.o: resources.rc
	$(GCC)\bin\windres -O coff resources.rc resources.o
