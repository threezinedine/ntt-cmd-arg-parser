set current=%~dp0
set base=%current%..\..\..\..\
set build=%base%build\Windows\MinGW\Release\

cd %build%
NTTArgParser_test.exe
cd %current%
