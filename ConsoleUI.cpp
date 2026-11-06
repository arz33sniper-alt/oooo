#include "ConsoleUI.h"
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

ConsoleUI::ConsoleUI(Machine& m) : machine(m) {
}

void ConsoleUI::start() {
    int choice = -1;
    while (true) {
        displayMenu();
        if (!(cin >> choice)) {
            break;
        }

        if (choice == 0) {
            cout << "Goodbye!" << endl;
            break;
        }
        else if (choice == 1) {
            handleLoadProgram();
        }
        else if (choice == 2) {
            handleOverwriteRegister();
        }
        else if (choice == 3) {
            handleOverwriteMemory();
        }
        else if (choice == 4) {
            handleRunAtOnce();
        }
        else if (choice == 5) {
            handleRunStepByStep();
        }
        else if (choice == 6) {
            displayFullState();
        }
        else if (choice == 7) {
            machine.reset(0);
            cout << "Machine cleared." << endl;
        }
        else {
            cout << "Invalid option, try again." << endl;
        }
    }
}

void ConsoleUI::displayMenu() const {
    cout << "\n--- Vole Machine Menu ---\n";
    cout << "1. Load Program from File\n";
    cout << "2. Overwrite Register\n";
    cout << "3. Overwrite Memory Cell\n";
    cout << "4. Run Program at Once\n";
    cout << "5. Run Program Step by Step\n";
    cout << "6. Display Machine Status\n";
    cout << "7. Clear / Reset Machine\n";
    cout << "0. Exit\n";
    cout << "Choice: ";
}

void ConsoleUI::displayFullState() const {
    displayMemoryMatrix();
    displayRegistersAndCPU();
    displayScreen();
}

void ConsoleUI::displayMemoryMatrix() const {
    const Memory& mem = machine.getMemory();
    cout << "\nMain Memory (16x16):\n";
    cout << "    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n";
    for (int r = 0; r < 16; r++) {
        cout << uppercase << hex << r << " | ";
        for (int c = 0; c < 16; c++) {
            cout << mem.getHexCell(r * 16 + c) << " ";
        }
        cout << "\n";
    }
    cout << dec;
}

void ConsoleUI::displayRegistersAndCPU() const {
    const RegisterFile& regs = machine.getRegisterFile();
    const CPU& cpu = machine.getCPU();

    cout << "\nCPU & Registers:\n";
    cout << "PC: " << uppercase << hex << setw(2) << setfill('0') << cpu.getPC()
         << "   IR: " << cpu.getIR()
         << "   Halted: " << (cpu.isHalted() ? "Yes" : "No") << "\n";

    for (int i = 0; i < 8; i++) {
        cout << "R" << uppercase << hex << i << ": " << regs.getHexRegister(i)
             << "    R" << (i + 8) << ": " << regs.getHexRegister(i + 8) << "\n";
    }
    cout << dec;
}

void ConsoleUI::displayScreen() const {
    const Screen& scr = machine.getScreen();
    cout << "\nScreen Output:\n";
    cout << "Hex  : " << scr.getHexOutput() << "\n";
    cout << "ASCII: " << scr.getAsciiOutput() << "\n";
}

void ConsoleUI::handleLoadProgram() {
    string filePath, hexAddr;
    cout << "Enter file name: ";
    cin >> filePath;
    cout << "Enter start address in hex (even, e.g. 0A or 10): ";
    cin >> hexAddr;

    int addr = 0;
    try {
        addr = stoi(hexAddr, 0, 16);
    } catch (...) {
        cout << "Invalid hex address." << endl;
        return;
    }

    if (machine.loadProgram(filePath, addr)) {
        cout << "Program loaded successfully." << endl;
        displayFullState();
    }
}

void ConsoleUI::handleOverwriteMemory() {
    string hexAddr, hexVal;
    cout << "Enter memory address in hex (00 - FF): ";
    cin >> hexAddr;
    cout << "Enter new byte value in hex (00 - FF): ";
    cin >> hexVal;

    int addr = 0;
    try {
        addr = stoi(hexAddr, 0, 16);
    } catch (...) {
        cout << "Invalid address." << endl;
        return;
    }

    if (machine.overwriteMemoryCell(addr, hexVal)) {
        cout << "Memory updated." << endl;
    } else {
        cout << "Invalid input." << endl;
    }
}

void ConsoleUI::handleOverwriteRegister() {
    string hexReg, hexVal;
    cout << "Enter register in hex (0 - F): ";
    cin >> hexReg;
    cout << "Enter new byte value in hex (00 - FF): ";
    cin >> hexVal;

    int idx = 0;
    try {
        idx = stoi(hexReg, 0, 16);
    } catch (...) {
        cout << "Invalid register." << endl;
        return;
    }

    if (machine.overwriteRegister(idx, hexVal)) {
        cout << "Register updated." << endl;
    } else {
        cout << "Invalid input." << endl;
    }
}

void ConsoleUI::handleRunAtOnce() {
    machine.runAll();
    cout << "\nProgram finished running:\n";
    displayFullState();
}

void ConsoleUI::handleRunStepByStep() {
    string line;
    getline(cin, line);
    while (!machine.getCPU().isHalted()) {
        machine.step();
        displayFullState();
        if (machine.getCPU().isHalted()) {
            cout << "Program halted.\n";
            break;
        }
        cout << "Press Enter for next step (or type q to stop): ";
        getline(cin, line);
        if (!line.empty() && (line[0] == 'q' || line[0] == 'Q'))
            break;
    }
}
