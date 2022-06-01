"C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\MSBuild\15.0\Bin\MSBuild.exe" saga_vpo.sln /p:Configuration=Release
mkdir release
xcopy C:\drone\src\build\network\line_direction\Release\line_direction.dll release
xcopy C:\drone\src\build\network\topologize\Release\line_direction.dll release
xcopy C:\drone\src\build\watem\watem\Release\watem.dll release
