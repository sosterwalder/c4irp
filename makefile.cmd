set PATH=C:\Python27;C:\Python27\Scripts;%PATH%

set flags=
set config=Release
set test=false
set vs_toolset=x64
set msbuild_platform=x64

:next-arg
if "%1"=="" goto args-done
if /i "%1"=="debug"        set config=Debug&goto arg-ok
if /i "%1"=="release"      set config=Release&goto arg-ok
if /i "%1"=="x86"          set target_arch=ia32&set msbuild_platform=WIN32&set vs_toolset=x86&goto arg-ok
if /i "%1"=="x64"          set target_arch=x64&set msbuild_platform=x64&set vs_toolset=x64&goto arg-ok
if /i "%1"=="test"         set test=true
:arg-ok
shift
goto next-arg
:args-done

if "%config%" == "Debug" set flags=/Od /MD
if "%config%" == "Release" set flags=/Ox /MD /DNDEBUG

set CFLAGS=/nologo /W3 %flags% -Ilibuv\include

call "%VS90COMNTOOLS%\..\..\vc\bin\vcvars64.bat"
set GYP_MSVS_VERSION=2008
copy "%VS90COMNTOOLS%\..\..\vc\bin\vcvars64.bat" "%VS90COMNTOOLS%\..\..\vc\bin\vcvarsamd64.bat"
copy "%VS90COMNTOOLS%\..\..\vc\bin\vcvars64.bat" "%VS90COMNTOOLS%\..\..\vc\bin\amd64\vcvarsamd64.bat"
cmd /C ".scripts\win-build.cmd" || exit /B 1
pip install -U cffi
python chirp_cffi/high_level.py || exit /B 1

if "%test%"=="true" goto test-it
goto the-end

:test-it
python chirp_cffi/low_level.py || exit /B 1
pip install -U click freeze hypothesis hypothesis-pytest pytest pytest_catchlog pytest_cov pytest_mock testfixtures || exit /B 1
@rem TODO libuv\Debug\\run-tests.exe || exit /B 1
:the-end
