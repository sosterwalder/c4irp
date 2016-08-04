set PATH=C:\Python27;C:\Python27\Scritps;%PATH%

set CFLAGS=/c /nologo /Ox /W3 /GL /DNDEBUG /MD -Ilibuv\include

set config=Debug
set vs_toolset=x64

:next-arg
if "%1"=="" goto args-done
if /i "%1"=="debug"        set config=Debug&goto arg-ok
if /i "%1"=="release"      set config=Release&goto arg-ok
if /i "%1"=="x86"          set target_arch=ia32&set msbuild_platform=WIN32&set vs_toolset=x86&goto arg-ok
if /i "%1"=="x64"          set target_arch=x64&set msbuild_platform=x64&set vs_toolset=x64&goto arg-ok
:arg-ok
shift
goto next-arg
:args-done

@rem Look for Visual Studio 2015
if not defined VS140COMNTOOLS goto vc-set-2013
if not exist "%VS140COMNTOOLS%\..\..\vc\vcvarsall.bat" goto vc-set-2013
call "%VS140COMNTOOLS%\..\..\vc\vcvarsall.bat" %vs_toolset%
set GYP_MSVS_VERSION=2015
echo Using Visual Studio 2015
goto vs-found

:vc-set-2013
@rem Look for Visual Studio 2013
if not defined VS120COMNTOOLS goto vc-set-2012
if not exist "%VS120COMNTOOLS%\..\..\vc\vcvarsall.bat" goto vc-set-2012
call "%VS120COMNTOOLS%\..\..\vc\vcvarsall.bat" %vs_toolset%
set GYP_MSVS_VERSION=2013
echo Using Visual Studio 2013
goto vs-found

:vc-set-2012
@rem Look for Visual Studio 2012
if not defined VS110COMNTOOLS goto vc-set-2010
if not exist "%VS110COMNTOOLS%\..\..\vc\vcvarsall.bat" goto vc-set-2010
call "%VS110COMNTOOLS%\..\..\vc\vcvarsall.bat" %vs_toolset%
set GYP_MSVS_VERSION=2012
echo Using Visual Studio 2012
goto vs-found

:vc-set-2010
@rem Look for Visual Studio 2010
if not defined VS100COMNTOOLS goto vc-set-2008
if not exist "%VS100COMNTOOLS%\..\..\vc\vcvarsall.bat" goto vc-set-2008
call "%VS100COMNTOOLS%\..\..\vc\vcvarsall.bat" %vs_toolset%
set GYP_MSVS_VERSION=2010
echo Using Visual Studio 2010
goto vs-found

:vc-set-2008
@rem Look for Visual Studio 2008
if not defined VS90COMNTOOLS goto vc-set-notfound
if not exist "%VS90COMNTOOLS%\..\..\vc\vcvarsall.bat" goto vc-set-notfound
call "%VS90COMNTOOLS%\..\..\vc\vcvarsall.bat" %vs_toolset%
set GYP_MSVS_VERSION=2008
echo Using Visual Studio 2008
goto vs-found

:vc-set-notfound
echo Visual Studio not found
exit /B 1

:vs-found

cmd

pushd libuv
call vcbuild.bat /test %debug% %vs_toolset% || exit /B 1
popd
copy config.defs.h config.h
python .scripts\win-build.py

@rem TODO libuv\Debug\\run-tests.exe || exit /B 1
