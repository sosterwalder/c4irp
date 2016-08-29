pushd libuv
call vcbuild.bat nobuid %config% %vs_toolset%
msbuild uv.sln /t:%target% /p:Configuration=Chirp%config% /p:Platform="%msbuild_platform%" /clp:NoSummary;NoItemAndPropertyList;Verbosity=minimal /nologo
popd
copy libuv\Chirp%config%\lib\libuv.lib uv.lib || exit /B 1
copy config.defs.h config.h || exit /B 1
python .scripts\win-build.py || exit /B 1
