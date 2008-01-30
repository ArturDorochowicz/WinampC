GCCPATH = C:\Progra~1\MinGW
APPNAME = WinampC
RESFILE = Resources

ifndef DEBUG
	CFLAGS = -Os -Wall
	LDFLAGS = -shared -s -fno-exceptions -fno-rtti
else
	CFLAGS = -Wall
	LDFLAGS = -shared
endif

$(APPNAME).dll : $(APPNAME).o $(RESFILE).o
	$(GCCPATH)\bin\gcc $(LDFLAGS) -o $(APPNAME).dll $(APPNAME).o $(RESFILE).o

$(APPNAME).o : $(APPNAME).c
	$(GCCPATH)\bin\gcc $(CFLAGS) -c $(APPNAME).c

$(RESFILE).o : $(RESFILE).rc
	$(GCCPATH)\bin\windres -o $(RESFILE).o $(RESFILE).rc