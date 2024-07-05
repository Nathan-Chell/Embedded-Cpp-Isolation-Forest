@echo off

:: Remove contents of the build directory
echo.
echo ### - Removing contents of the build directory
rmdir build /S /Q

:: Create build directory
echo. 
echo ### - Creating build directory
mkdir build
cd build

:: Configure with CMake
echo.
echo ### - Configuring with CMake
cmake ..

:: Build the Release configuration
echo.
echo ### - Building the Release configuration
cmake --build . --config Release

:: Navigate back to the root directory
echo.
echo ### - Navigating back to the root directory
cd ..

::run the executable
echo.
echo ### - Running the executable
echo.
.\build\Release\Embedded-Cpp-Isolation-Forest.exe
