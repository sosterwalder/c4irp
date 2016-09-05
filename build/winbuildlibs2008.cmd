set flags=
set config=Release
if /i "%MODE%"=="debug" set config=Debug
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

if "%config%" == "Debug" set flags=/Od /Zi /MD /DEBUG
if "%config%" == "Release" set flags=/Ox /MD /DNDEBUG

set CFLAGS=/nologo /W3 %flags% -Ibuild\libuv\include

call "%VS90COMNTOOLS%\..\..\vc\bin\vcvars64.bat"
set GYP_MSVS_VERSION=2008
copy "%VS90COMNTOOLS%\..\..\vc\bin\vcvars64.bat" "%VS90COMNTOOLS%\..\..\vc\bin\vcvarsamd64.bat"
copy "%VS90COMNTOOLS%\..\..\vc\bin\vcvars64.bat" "%VS90COMNTOOLS%\..\..\vc\bin\amd64\vcvarsamd64.bat"
@REM python build\winbuild.py || exit /B 1
pushd build\libuv
cmd /C vcbuild.bat nobuild %mode% %vs_toolset%
vcbuild.exe /platform:%msbuild_platform% libuv.vcproj Chirp%config%
popd
copy build\libuv\Chirp%config%\lib\libuv.lib uv.lib || exit /B 1
if "%test%"=="true" goto test-it
goto the-end

:test-it
vcbuild.exe /platform:%msbuild_platform% run-tests.vcproj Chirp%config%
build\libuv\Chirp%config%\run-tests.exe || exit /B 1
:the-end
