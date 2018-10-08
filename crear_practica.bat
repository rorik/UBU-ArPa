:: Made by Roderick D. for Arquitecturas Paralelas - UBU - 2018/2019
:: https://rorik.me | https://github.com/rorik | https://about@rorik.me
:: This software is protected under GNU General Public License v3.0,
:: for more information check https://github.com/rorik/UBU-ArPa
@ECHO off
ECHO Script para insertar una nueva practica a la solucion.
:ask_id
ECHO.
SET /p ID="Introducir numero de la practica: "
SET NAME=Practica%ID%

IF EXIST %NAME% (
	ECHO ERROR - El directorio %NAME% ya existe, pruebe con otro identificador o borre el directorio.
	GOTO ask_id
)

SET ERRORLEVEL=
CHOICE /C DM /M "Utilizar DeinoMPI o MS-MPI?"
IF %ERRORLEVEL%==1 (
	SET TYPE="Deino"
	SET DESC="DeinoMPI"
) ELSE (
	SET TYPE="MS"
	SET DESC="Microsoft-MPI"
)

SET ERRORLEVEL=
CHOICE /M "Crear proyecto %NAME% de tipo %DESC% en %CD%\%NAME%?"
IF %ERRORLEVEL%==2 (
	GOTO ask_id
)

MKDIR %NAME%
ROBOCOPY .PracticaX/%TYPE% %NAME% > NUL
SETLOCAL EnableExtensions EnableDelayedExpansion
	FOR /f "delims=" %%A IN ('type "%NAME%\PracticaX.vcxproj"') DO (
		SET "LINE=%%A"
		SET "NEWLINE=!LINE:PracticaX=%NAME%!"
		ECHO !NEWLINE!>>"%NAME%\%NAME%.vcxproj"
	)
	DEL /F "%NAME%\PracticaX.vcxproj"
ENDLOCAL
REN %NAME%\PracticaX.cpp %NAME%.cpp
ATTRIB -r %NAME%\%NAME%.cpp
ECHO Proyecto %NAME% creado, insertar en la solucion desde Visual Studio.

ECHO.
SET ERRORLEVEL=
CHOICE /M "Crear otro proyecto?"
IF %ERRORLEVEL%==1 (
	GOTO ask_id
)

ECHO.
ECHO Ejecucion finalizada, presione cualquier tecla para finalizar...
PAUSE > NUL