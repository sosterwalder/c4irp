set PATH=%PYTHONVER%;%PYTHONVER%\Scripts;%PATH%
set PYTHONPATH=%CD%
pip install -U cffi requests || exit /B 1
if "%TESTLIB%"=="true" goto theend
python setup.py bdist_wheel || exit /B 1
:theend
