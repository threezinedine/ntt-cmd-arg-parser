@echo off

set current=%~dp0
set base=%current%..\..\..\..\
set build=%base%build\Windows\MinGW\Release\
set examples=%build%examples\

for %%i in (%examples%*.exe) do (
    echo Running %%i
    %%i
)