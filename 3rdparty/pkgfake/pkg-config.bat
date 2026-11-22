@echo off
if "%1"=="--version" (
  echo 0.29.2
  exit /b 0
)
if "%1"=="--modversion" (
  exit /b 1
)
exit /b 1
