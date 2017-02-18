@call C:\WinDDK\7600.16385.1\bin\setenv.bat C:\WinDDK\7600.16385.1\ fre x64 WIN7
cd /d %~dp0
cd ../../Src/FirewallEx
rmdir /S /Q objfre_win7_amd64
nmake
move objfre_win7_amd64\amd64\superfirewallexrate.sys ..\Bin\..\ndis\superfirewallexrate_amd64.sys
@if not %errorlevel% ==0 goto exceptionEnd

@echo ========================================
@echo Build X64 Success
pause
exit

:exceptionEnd
@echo ========================================
@echo Build X64 Failed
pause
exit