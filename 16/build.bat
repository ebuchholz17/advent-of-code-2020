@echo off

cl -MT -nologo -GR- -EHa- -O2 -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -FC -Z7  main.cpp /Feadvent.exe /link /OPT:REF /SUBSYSTEM:CONSOLE 
REM cl -MT -nologo -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -FC -Z7  main.cpp /Feadvent.exe /link /OPT:REF /SUBSYSTEM:CONSOLE Winmm.lib

popd

