set PATH=C:\Python27;C:\Python27\Scripts;%PATH%
pip install -U cffi
pip install -U -e .  || exit /B 0
