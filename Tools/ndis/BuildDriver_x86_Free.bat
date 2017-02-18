@call C:\WinDDK\7600.16385.1\bin\setenv.bat C:\WinDDK\7600.16385.1\ fre x86 WIN7
cd /d %~dp0
cd ../../Src/FirewallEx
rmdir /S /Q objfre_win7_x86
nmake
move objfre_win7_x86\i386\superfirewallexrate.sys ..\..\Bin\ndis\superfirewallexrate_i386.sys
@if not %errorlevel% ==0 goto exceptionEnd

@echo ========================================
@echo Build X86 Success
pause
exit

:exceptionEnd
@echo ========================================
@echo Build X86 Failed
pause
exit
