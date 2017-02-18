# Microsoft Developer Studio Generated NMAKE File, Based on TaskKeyHook.dsp
!IF "$(CFG)" == ""
CFG=TaskKeyHook - Win32 Debug
!MESSAGE No configuration specified. Defaulting to TaskKeyHook - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "TaskKeyHook - Win32 Debug" && "$(CFG)" != "TaskKeyHook - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TaskKeyHook.mak" CFG="TaskKeyHook - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TaskKeyHook - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TaskKeyHook - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TaskKeyHook - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\TaskKeyHook.dll"


CLEAN :
	-@erase "$(INTDIR)\TaskKeyHook.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\TaskKeyHook.dll"
	-@erase "$(OUTDIR)\TaskKeyHook.exp"
	-@erase "$(OUTDIR)\TaskKeyHook.ilk"
	-@erase "$(OUTDIR)\TaskKeyHook.lib"
	-@erase "$(OUTDIR)\TaskKeyHook.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TaskKeyHook.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\TaskKeyHook.pdb" /debug /machine:I386 /out:"$(OUTDIR)\TaskKeyHook.dll" /implib:"$(OUTDIR)\TaskKeyHook.lib" 
LINK32_OBJS= \
	"$(INTDIR)\TaskKeyHook.obj"

"$(OUTDIR)\TaskKeyHook.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TaskKeyHook - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\TaskKeyHook.dll"


CLEAN :
	-@erase "$(INTDIR)\TaskKeyHook.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\TaskKeyHook.dll"
	-@erase "$(OUTDIR)\TaskKeyHook.exp"
	-@erase "$(OUTDIR)\TaskKeyHook.ilk"
	-@erase "$(OUTDIR)\TaskKeyHook.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TaskKeyHook.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\TaskKeyHook.pdb" /machine:I386 /out:"$(OUTDIR)\TaskKeyHook.dll" /implib:"$(OUTDIR)\TaskKeyHook.lib" 
LINK32_OBJS= \
	"$(INTDIR)\TaskKeyHook.obj"

"$(OUTDIR)\TaskKeyHook.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("TaskKeyHook.dep")
!INCLUDE "TaskKeyHook.dep"
!ELSE 
!MESSAGE Warning: cannot find "TaskKeyHook.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "TaskKeyHook - Win32 Debug" || "$(CFG)" == "TaskKeyHook - Win32 Release"
SOURCE=.\TaskKeyHook.cpp

"$(INTDIR)\TaskKeyHook.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

