set current=%~dp0
set base=%current%..\..\..\..\
set build=%base%build\Windows\MinGW\Release\

cmake -S %base% -B %build% -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DNTTArgParser_USE_TESTS=ON -DNTTArgParser_USE_EXAMPLES=ON