set PATH=%PYTHONVER%;%PYTHONVER%\Scripts;%PATH%
pip install -U cffi || exit /B 1
python setup.py bdist_wheel || exit /B 1
