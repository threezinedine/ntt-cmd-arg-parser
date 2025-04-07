set current=%~dp0
set base=%current%..\..\..\
set build=%base%build\Windows\VS22

cmake -S %base% -B %build% -G "Visual Studio 17 2022" -DNTTLib_USE_TESTS=ON -DNTTLib_USE_EXAMPLES=ON -DNTTArgParser_USE_TESTS=ON -DNTTArgParser_USE_EXAMPLES=ON