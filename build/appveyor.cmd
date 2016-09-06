set PATH=%PYTHONVER%;%PYTHONVER%\Scripts;%PATH%
pip install -U cffi || exit /B 1
pip install -U -e . || exit /B 1
python setup.py bdist_wheel || exit /B 1
@REM 7z a build.zip .
