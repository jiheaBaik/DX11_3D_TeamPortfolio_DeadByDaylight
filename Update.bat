xcopy /y /s .\Engine\Public\*.* .\Reference\Header\
xcopy /y /s .\Engine\Bin\*.lib .\Reference\Library\
xcopy /y /s .\Engine\Bin\*.dll .\Client\Bin\
xcopy /y /s .\Engine\Bin\Shader\*.hlsl .\Client\Bin\Shader\

xcopy /y .\Engine\Private\assimp\*.dll .\Client\Bin\
xcopy /y .\Engine\Private\fmod\*.dll .\Client\Bin\