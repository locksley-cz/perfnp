cmake -H. -Bbuild -Ax64 || goto :error
cmake --build build --config Release || goto :error
build\Release\tests.exe || goto :error

:error
echo Failed with error #%errorlevel%.
exit /b %errorlevel%
