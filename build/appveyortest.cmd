set PATH=%PYTHONVER%;%PYTHONVER%\Scripts;%PATH%
set PYTHONPATH=%CD%
git clean -xdf
set MODE=debug
pip install -U -e . || exit /B 1
pip install -U click freeze hypothesis hypothesis-pytest pytest pytest_catchlog pytest_cov pytest_mock testfixtures || exit /B 1
py.test --doctest-modules chirp || exit /B 1
