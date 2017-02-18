rem SET SIGNTOOL="..\Sign\signtool.exe"
SET SIGNTOOL="C:\Program Files (x86)\Windows Kits\8.1\bin\x86\signtool.exe"
rem SET INF2CAT="C:\WinDDK\7600.16385.1\bin\selfsign\inf2cat"
SET INF2CAT="C:\Program Files (x86)\Windows Kits\10\bin\x86\inf2cat.exe"
rem sha256
del amd64\*.sys i386\*.sys *.cat
copy superfirewallexrate_amd64.sys amd64\superfirewallexrate.sys
copy superfirewallexrate_i386.sys i386\superfirewallexrate.sys
%SIGNTOOL% sign /ac ..\Sign\MSCV-VSClass3.cer /fd sha256 /f ..\sign\125.pfx /p 123 /t http://timestamp.verisign.com/scripts/timstamp.dll /v amd64\*.sys i386\*.sys
@if not %errorlevel% ==0 goto exceptionEnd
%INF2CAT% /driver:%~dp0 /os:2000,XP_X86,XP_X64,Vista_X86,Vista_X64,7_X86,7_X64,Server2003_X86,Server2003_X64,Server2008_X86,Server2008_X64,Server2008R2_X64
@if not %errorlevel% ==0 goto exceptionEnd
%SIGNTOOL% sign /ac ..\Sign\MSCV-VSClass3.cer /fd sha256 /f ..\sign\125.pfx /p 123 /t http://timestamp.verisign.com/scripts/timstamp.dll /v *.cat
@if not %errorlevel% ==0 goto exceptionEnd
del ndis.7z
..\7z a ndis.7z amd64 i386 *.inf *.cat

rem sha1
del amd64\*.sys i386\*.sys *.cat
copy superfirewallexrate_amd64.sys amd64\superfirewallexrate.sys
copy superfirewallexrate_i386.sys i386\superfirewallexrate.sys
%SIGNTOOL% sign /ac ..\Sign\MSCV-VSClass3.cer /f ..\sign\sha1\sha1.pfx /p 321 /t http://timestamp.verisign.com/scripts/timstamp.dll /v amd64\*.sys i386\*.sys
@if not %errorlevel% ==0 goto exceptionEnd
%INF2CAT% /driver:%~dp0 /os:2000,XP_X86,XP_X64,Vista_X86,Vista_X64,7_X86,7_X64,Server2003_X86,Server2003_X64,Server2008_X86,Server2008_X64,Server2008R2_X64
@if not %errorlevel% ==0 goto exceptionEnd
%SIGNTOOL% sign /ac ..\Sign\MSCV-VSClass3.cer /f ..\sign\sha1\sha1.pfx /p 321 /t http://timestamp.verisign.com/scripts/timstamp.dll /v *.cat
@if not %errorlevel% ==0 goto exceptionEnd
del ndis_sha1.7z
..\7z a ndis_sha1.7z amd64 i386 *.inf *.cat

del amd64\*.sys i386\*.sys *.cat

@echo ========================================
@echo Sign Success
pause
exit

:exceptionEnd
@echo ========================================
@echo Sign Failed
pause

rem exit
