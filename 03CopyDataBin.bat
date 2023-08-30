mkdir bin

xcopy  data bin\data /e /i /h /y

copy x64\Release\OpenGolf.exe bin
copy x64\Release\Editor.exe bin