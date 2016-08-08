set PATH="D:\Programm\CMake\bin\";%PATH%
call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat"

pushd build.win64.vc2012

call cmake.exe -G "Visual Studio 14 Win64" -DOpenCV_DIR="D:/Programm/OpenCV31/opencv/build" ..

@rem -D OpenCV=<"D:/Programm/OpenCV 3.1/opencv/build">
@rem -D OpenCV="D:/Programm/OpenCV 3.1/pencv/build/x64/vc14/lib"

@rem msbuild grozov_r_a.sln /property:Configuration=Debug /m

@rem msbuild grozov_r_a.sln /property:Configuration=Release /m

popd