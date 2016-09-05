set flags=
set config=Release
if /i "%MODE%"=="debug" set config=Debug
set test=false
set vs_toolset=x64
set msbuild_platform=x64

:nextarg
if "%1"=="" goto argsdone
if /i "%1"=="debug"        set config=Debug&goto argok
if /i "%1"=="release"      set config=Release&goto argok
if /i "%1"=="x86"          set target_arch=ia32&set msbuild_platform=WIN32&set vs_toolset=x86&goto argok
if /i "%1"=="x64"          set target_arch=x64&set msbuild_platform=x64&set vs_toolset=x64&goto argok
if /i "%1"=="test"         set test=true
:argok
shift
goto nextarg
:argsdone

if "%config%" == "Debug" set flags=/Od /Zi /MD /DEBUG
if "%config%" == "Release" set flags=/Ox /MD /DNDEBUG

set CFLAGS=/nologo /W3 %flags% -Ibuild\libuv\include

call "%VS140COMNTOOLS%\..\..\vc\vcvarsall.bat" %vs_toolset%
set GYP_MSVS_VERSION=2015
python build\winbuild.py || exit /B 1
cmd /C build\vcbuildwrap.cmd || exit /B 1
copy build\libuv\Chirp%config%\lib\libuv.lib uv.lib || exit /B 1
if "%test%"=="true" goto testit
goto theend

:testit
build\libuv\Chirp%config%\run-tests.exe || exit /B 1
:theend
exit /B 0
