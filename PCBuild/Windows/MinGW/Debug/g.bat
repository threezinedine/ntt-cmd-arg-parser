set current=%~dp0
set base=%current%..\..\..\..\
set build=%base%build\Windows\MinGW\Debug\

cmake -S %base% -B %build% -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DNTTArgParser_USE_TESTS=ON -DNTTArgParser_USE_EXAMPLES=ON