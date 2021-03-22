
INSTALLDIR = C:\hts_engine_API

all:
	cd lib
	nmake /f Makefile.mak
	cd ..

clean:
	cd lib
	nmake /f Makefile.mak clean
	cd ..

install::
	@if not exist "$(INSTALLDIR)\lib" mkdir "$(INSTALLDIR)\lib"
	cd lib
	copy *.lib $(INSTALLDIR)\lib
	cd ..
	@if not exist "$(INSTALLDIR)\include" mkdir "$(INSTALLDIR)\include"
	cd include
	copy *.h $(INSTALLDIR)\include
	cd ..
