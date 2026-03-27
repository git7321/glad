# Microsoft Developer Studio Project File - Name="wglgears" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=wglgears - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wglgears.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wglgears.mak" CFG="wglgears - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wglgears - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "."
# PROP BASE Intermediate_Dir "."
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "."
# PROP Intermediate_Dir "."
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gy /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /Oxs /GF /c
# ADD CPP /nologo /MD /W3 /Gy /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /Oxs /GF /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib opengl32.lib glad.lib winmm.lib msvcrt.lib /nologo /version:4.0 /subsystem:console /debug /machine:I386 /opt:ref,icf /release
# ADD LINK32 kernel32.lib user32.lib gdi32.lib opengl32.lib glad.lib winmm.lib msvcrt.lib /nologo /version:4.0 /subsystem:console /debug /machine:I386 /opt:ref,icf /release
# Begin Target

# Name "wglgears - Win32 Release"
# Begin Source File

SOURCE=.\wglgears.c
# End Source File
# End Target
# End Project
