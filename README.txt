========================================================================
  CS213: Object Oriented Programming - Assignment 2 (Version 4.1)
  Vole Machine Simulator (Console + GUI Bonus)
  Section: S39_S40
  Supervised by: Dr. Mohammad El-Ramly
========================================================================

TEAM MEMBERS:
  1. Reem Hamdy Tawfik Alrefaay  - ID: 20250823
  2. Khaled Mohamed Hussien      - ID: 20250212
  3. Khaled Mohamed Fawzy        - ID: 20250213

------------------------------------------------------------------------
1. PROJECT STRUCTURE
------------------------------------------------------------------------
  - Memory.h / Memory.cpp                 : 256-byte Main Memory (16x16 matrix)
  - RegisterFile.h / RegisterFile.cpp     : 16 General-Purpose Registers (R0..RF)
  - Screen.h / Screen.cpp                 : Memory-Mapped Screen Output (Address FF)
  - Instruction.h / Instruction.cpp       : 16-bit Vole Instruction Decoder & Validator
  - ALU.h / ALU.cpp                       : Two's Complement, Excess-4 Float, Bitwise & Rotate
  - ProgramLoader.h / ProgramLoader.cpp   : File Parser, Comment Stripper & Validator
  - CPU.h / CPU.cpp                       : Fetch-Decode-Execute Cycle (PC, IR, Halt)
  - Machine.h / Machine.cpp               : Main Simulator Controller
  - ConsoleUI.h / ConsoleUI.cpp           : Interactive Console User Interface
  - A2-S39_S40-20250823-20250212-20250213.cpp : Main Console Entry Point
  - GUI/VoleGuiWindow.h / .cpp            : Qt5/Qt6 GUI Implementation
  - GUI/VoleMachineGUI.pro & CMakeLists.txt : Qt Creator Project Files
  - GUI/VoleGUI_Win32.cpp                 : Native Win32 GUI (Zero external library install needed)
  - programs/                             : 20 Sample Vole Machine Programs (.txt)
  - Snapshots/                            : Screenshots of the GUI Simulator in action

------------------------------------------------------------------------
2. HOW TO COMPILE & RUN THE CONSOLE SIMULATOR (PART 2)
------------------------------------------------------------------------
Using g++ (MinGW / GCC) from the project root directory:

  g++ -std=c++17 Memory.cpp RegisterFile.cpp Screen.cpp Instruction.cpp ALU.cpp CPU.cpp ProgramLoader.cpp Machine.cpp ConsoleUI.cpp A2-S39_S40-20250823-20250212-20250213.cpp -o VoleSimulator.exe

To run:
  ./VoleSimulator.exe

------------------------------------------------------------------------
3. HOW TO COMPILE & RUN THE GUI SIMULATOR (PART 3 - BONUS)
------------------------------------------------------------------------
Option A - Using Qt Creator (Qt5 or Qt6):
  1. Open Qt Creator.
  2. Click "Open Project" and select either:
       - GUI/VoleMachineGUI.pro  (qmake)
       - GUI/CMakeLists.txt      (CMake)
  3. Configure kit (Desktop Qt MinGW/MSVC) and click Run (Ctrl+R).

Option B - Direct Compilation with MinGW g++ (No third-party setup required):
  g++ -std=c++17 GUI/VoleGUI_Win32.cpp Memory.cpp RegisterFile.cpp Screen.cpp Instruction.cpp ALU.cpp CPU.cpp ProgramLoader.cpp Machine.cpp -o VoleGUI.exe -mwindows -lgdi32 -lcomdlg32

To run:
  ./VoleGUI.exe

------------------------------------------------------------------------
4. HOW TO USE THE GUI SIMULATOR
------------------------------------------------------------------------
  1. Type or paste Vole instructions in the "Data Input Window" (or click "Browse Program File...").
  2. Enter an even Hex Start Address (< FF, e.g., 10) in the "Start Address" box.
  3. Click "Load Data" to validate and load the instructions into the 16x16 Main Memory matrix.
  4. Click "Single Step" to execute one instruction at a time and watch PC, IR, R0..RF, and Memory update.
  5. Click "Run" to execute the program until HALT (C000).
  6. Any store to address FF (3 RFF) is displayed in both ASCII and Hex in the Screen Output box.
  7. Click "Clear Memory" to reset all memory cells, registers, and screen output.
