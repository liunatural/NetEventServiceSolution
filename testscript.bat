@echo off
set var = 0

:continue
echo run %var% times
start VRTestClient.exe
set /a var+=1
if %var% lss 15 goto continue

echo run completes
pause