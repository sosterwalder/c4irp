set PATH=%PYTHON%;%PYTHON%\Scripts;%PATH%
pip install -U cffi || exit /B 1
pip install -U -e . || exit /B 1
python setup.py bdist_wheel
@REM 7z a build.zip .
