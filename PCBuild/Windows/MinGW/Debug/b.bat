set current=%~dp0
set base=%current%..\..\..\..\
set build=%base%build\Windows\MinGW\Debug\

cd %build%
mingw32-make
cd %current%