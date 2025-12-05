rem This batch file runs the .bmp files through a tool that creates a .h header file for the .bmp file passed in.
rem The .h file produced is 2 dimensional array of 32-bit integers for 24 bit colour, in the correct format for
rem the WiiU OSScreen pixel function. 
rem
rem see https://github.com/MartinButlerAAA/ProcessBmp for the ProcessBmp tool.
rem
rem This tool can only handle .bmp images. Any other images need to be converted to 24-bit colour .bmp files, which can
rem be done using Microsoft Paint.
rem 
cd .
processBmp Allowed.bmp
processBmp Blank.bmp
processBmp Selected.bmp

processBmp Green1.bmp
processBmp Green2.bmp
processBmp Green3.bmp
processBmp Green4.bmp
processBmp Green5.bmp

processBmp Red1.bmp
processBmp Red2.bmp
processBmp Red3.bmp
processBmp Red4.bmp
processBmp Red5.bmp

processBmp Middle.bmp

pause

