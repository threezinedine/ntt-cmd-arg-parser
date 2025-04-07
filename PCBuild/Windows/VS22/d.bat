set current=%~dp0
set base=%current%..\..\..\
set docs=%base%docs\
set html=%docs%\html\

cd %docs%
Doxygen Doxyfile
%html%index.html
cd %current%

