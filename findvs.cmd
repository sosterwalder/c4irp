@rem Look for Visual Studio 2015
if not defined VS140COMNTOOLS goto vc-set-2013
if not exist "%VS140COMNTOOLS%\..\..\vc\vcvarsall.bat" goto vc-set-2013
set VS90COMNTOOLS="%VS140COMNTOOLS%
echo Using Visual Studio 2015
goto vs-found

:vc-set-2013
@rem Look for Visual Studio 2013
if not defined VS120COMNTOOLS goto vc-set-2012
if not exist "%VS120COMNTOOLS%\..\..\vc\vcvarsall.bat" goto vc-set-2012
set VS90COMNTOOLS="%VS120COMNTOOLS%
echo Using Visual Studio 2013
goto vs-found

:vc-set-2012
@rem Look for Visual Studio 2012
if not defined VS110COMNTOOLS goto vc-set-2010
if not exist "%VS110COMNTOOLS%\..\..\vc\vcvarsall.bat" goto vc-set-2010
set VS90COMNTOOLS="%VS110COMNTOOLS%
echo Using Visual Studio 2012
goto vs-found

:vc-set-2010
@rem Look for Visual Studio 2010
if not defined VS100COMNTOOLS goto vc-set-2008
if not exist "%VS100COMNTOOLS%\..\..\vc\vcvarsall.bat" goto vc-set-2008
set VS90COMNTOOLS="%VS100COMNTOOLS%
echo Using Visual Studio 2010
goto vs-found

:vc-set-2008
@rem Look for Visual Studio 2008
if not defined VS90COMNTOOLS goto vc-set-notfound
if not exist "%VS90COMNTOOLS%\..\..\vc\vcvarsall.bat" goto vc-set-notfound
echo Using Visual Studio 2008
goto vs-found

:vc-set-notfound
echo Visual Studio not found
exit /B 1

:vs-found
