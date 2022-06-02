dir
"C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\MSBuild\Current\Bin\MSBuild.exe" saga_vpo.sln /p:Configuration=Release
mkdir release
xcopy C:\drone\src\build\network\line_direction\Release\line_direction.dll release
xcopy C:\drone\src\build\network\topologize\Release\topologize.dll release
xcopy C:\drone\src\build\watem\watem\Release\watem.dll release
powershell Compress-Archive -Path release\*.dll -DestinationPath c:\saga-vpo_%DRONE_TAG%.zip
