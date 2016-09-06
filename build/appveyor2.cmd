set PATH=C:\Python27;C:\Python27\Scripts;%PATH%
pip install -U cffi
pip install -U -e .
7z a build.zip .
exit /B 0
