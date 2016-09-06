set PATH=%PYTHON%;%PYTHON%\Scripts;%PATH%
pip install -U click freeze hypothesis hypothesis-pytest pytest pytest_catchlog pytest_cov pytest_mock testfixtures || exit /B 1
py.test --doctest-modules chirp || exit /B 1
