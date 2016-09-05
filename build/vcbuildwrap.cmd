set PATH=C:\Python27;C:\Python27\Scripts;%PATH%
pushd build\libuv
call vcbuild.bat nobuild %mode% %vs_toolset%
msbuild.exe libuv.vcxproj /t:%target% /p:Configuration=Chirp%config% /p:Platform="%msbuild_platform%" /clp:NoSummary;NoItemAndPropertyList;Verbosity=minimal /nologo || exit /B 1
if "%test%"=="true" goto testit
goto theend

:testit
msbuild.exe run-tests.vcxproj /t:%target% /p:Configuration=Chirp%config% /p:Platform="%msbuild_platform%" /clp:NoSummary;NoItemAndPropertyList;Verbosity=minimal /nologo || exit /B 1
:theend
popd

