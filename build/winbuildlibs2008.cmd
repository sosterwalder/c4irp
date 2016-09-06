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

copy "%VS90COMNTOOLS%\..\..\vc\bin\vcvars64.bat" "%VS90COMNTOOLS%\..\..\vc\bin\vcvarsamd64.bat"
copy "%VS90COMNTOOLS%\..\..\vc\bin\vcvars64.bat" "%VS90COMNTOOLS%\..\..\vc\bin\amd64\vcvarsamd64.bat"
call "%VS90COMNTOOLS%\..\..\vc\vcvarsall.bat" %vs_toolset%
set GYP_MSVS_VERSION=2008
pushd build\libuv
cmd /C vcbuild.bat nobuild %mode% %vs_toolset% || exit /B 1
vcbuild.exe /platform:%msbuild_platform% libuv.vcproj Chirp%config% || exit /B 1
popd
copy build\libuv\Chirp%config%\lib\libuv.lib uv.lib || exit /B 1
python build\winbuild.py || exit /B 1
if "%test%"=="true" goto testit
goto theend

:testit
pushd build\libuv
vcbuild.exe /platform:%msbuild_platform% run-tests.vcproj Chirp%config% || exit /B 1
popd
build\libuv\Chirp%config%\run-tests.exe || exit /B 1
:theend
exit /B 0
