#include "VoleGuiWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <bitset>

using namespace std;

VoleGuiWindow::VoleGuiWindow(QWidget* parent) : QMainWindow(parent) {
    setupUi();
    refreshDisplay();
}

void VoleGuiWindow::setupUi() {
    setWindowTitle("Vole Machine Simulator GUI - CS213 (Section S39_S40)");
    resize(1180, 760);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    // Top section: Data Input Window & Screen Output
    QHBoxLayout* topLayout = new QHBoxLayout();

    QGroupBox* inputGroup = new QGroupBox("Data Input Window (Vole Machine Program)", this);
    QVBoxLayout* inputLayout = new QVBoxLayout(inputGroup);
    codeEditor = new QPlainTextEdit(this);
    codeEditor->setPlainText(
        "; Sample Program: Print FCAI to Screen (3 RFF)\n"
        "2146 ; R1 = 'F'\n"
        "31FF ; Store to Screen (FF)\n"
        "2143 ; R1 = 'C'\n"
        "31FF ; Store to Screen (FF)\n"
        "2141 ; R1 = 'A'\n"
        "31FF ; Store to Screen (FF)\n"
        "2149 ; R1 = 'I'\n"
        "31FF ; Store to Screen (FF)\n"
        "C000 ; Halt"
    );
    inputLayout->addWidget(codeEditor);

    QHBoxLayout* addrBar = new QHBoxLayout();
    addrBar->addWidget(new QLabel("Start Address (Hex Even 00..FE):", this));
    startAddrEdit = new QLineEdit("10", this);
    startAddrEdit->setMaximumWidth(60);
    addrBar->addWidget(startAddrEdit);
    QPushButton* openFileBtn = new QPushButton("Browse File...", this);
    connect(openFileBtn, &QPushButton::clicked, this, &VoleGuiWindow::onLoadFromFile);
    addrBar->addWidget(openFileBtn);
    addrBar->addStretch();
    inputLayout->addLayout(addrBar);
    topLayout->addWidget(inputGroup, 3);

    QGroupBox* screenGroup = new QGroupBox("Memory-Mapped Screen Output (Address FF)", this);
    QVBoxLayout* screenLayout = new QVBoxLayout(screenGroup);
    screenLayout->addWidget(new QLabel("ASCII Output:", this));
    screenAsciiBox = new QPlainTextEdit(this);
    screenAsciiBox->setReadOnly(true);
    screenLayout->addWidget(screenAsciiBox);
    screenLayout->addWidget(new QLabel("Hex Output Stream:", this));
    screenHexBox = new QPlainTextEdit(this);
    screenHexBox->setReadOnly(true);
    screenLayout->addWidget(screenHexBox);
    topLayout->addWidget(screenGroup, 2);

    mainLayout->addLayout(topLayout, 2);

    // Middle section: CPU Registers & 16x16 Main Memory Matrix
    QHBoxLayout* midLayout = new QHBoxLayout();

    QGroupBox* cpuGroup = new QGroupBox("CPU & 16 Registers (R0 - RF)", this);
    QVBoxLayout* cpuLayout = new QVBoxLayout(cpuGroup);

    QHBoxLayout* statusRow = new QHBoxLayout();
    pcLabel = new QLabel("PC: 00", this);
    irLabel = new QLabel("IR: 0000", this);
    statusLabel = new QLabel("Status: Ready", this);
    statusRow->addWidget(pcLabel);
    statusRow->addWidget(irLabel);
    statusRow->addWidget(statusLabel);
    cpuLayout->addLayout(statusRow);

    regTable = new QTableWidget(16, 3, this);
    regTable->setHorizontalHeaderLabels({"Hex", "Binary", "Dec (2's Comp)"});
    for (int i = 0; i < 16; i++) {
        stringstream ss;
        ss << "R" << uppercase << hex << i;
        regTable->setVerticalHeaderItem(i, new QTableWidgetItem(QString::fromStdString(ss.str())));
    }
    regTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    cpuLayout->addWidget(regTable);
    midLayout->addWidget(cpuGroup, 2);

    QGroupBox* memGroup = new QGroupBox("Main Memory (16 x 16 Matrix = 256 Bytes)", this);
    QVBoxLayout* memLayout = new QVBoxLayout(memGroup);
    memTable = new QTableWidget(16, 16, this);
    for (int i = 0; i < 16; i++) {
        stringstream ss;
        ss << uppercase << hex << i;
        QString h = QString::fromStdString(ss.str());
        memTable->setHorizontalHeaderItem(i, new QTableWidgetItem(h));
        memTable->setVerticalHeaderItem(i, new QTableWidgetItem(h));
    }
    memTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    memTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    memLayout->addWidget(memTable);
    midLayout->addWidget(memGroup, 4);

    mainLayout->addLayout(midLayout, 4);

    // Bottom section: Action Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* clearBtn = new QPushButton("Clear Memory", this);
    QPushButton* loadBtn = new QPushButton("Load Data", this);
    QPushButton* runBtn = new QPushButton("Run", this);
    QPushButton* stepBtn = new QPushButton("Single Step", this);
    QPushButton* haltBtn = new QPushButton("Halt", this);
    QPushButton* helpBtn = new QPushButton("Help", this);

    connect(clearBtn, &QPushButton::clicked, this, &VoleGuiWindow::onClearMemory);
    connect(loadBtn, &QPushButton::clicked, this, &VoleGuiWindow::onLoadData);
    connect(runBtn, &QPushButton::clicked, this, &VoleGuiWindow::onRunAll);
    connect(stepBtn, &QPushButton::clicked, this, &VoleGuiWindow::onSingleStep);
    connect(haltBtn, &QPushButton::clicked, this, &VoleGuiWindow::onHalt);
    connect(helpBtn, &QPushButton::clicked, this, &VoleGuiWindow::onHelp);

    btnLayout->addWidget(clearBtn);
    btnLayout->addWidget(loadBtn);
    btnLayout->addWidget(runBtn);
    btnLayout->addWidget(stepBtn);
    btnLayout->addWidget(haltBtn);
    btnLayout->addWidget(helpBtn);
    mainLayout->addLayout(btnLayout);
}

void VoleGuiWindow::refreshDisplay() {
    const Memory& mem = machine.getMemory();
    const RegisterFile& regs = machine.getRegisterFile();
    const CPU& cpu = machine.getCPU();
    const Screen& scr = machine.getScreen();

    int pc = cpu.getPC();
    stringstream pcss;
    pcss << "PC: " << uppercase << hex << setw(2) << setfill('0') << pc;
    pcLabel->setText(QString::fromStdString(pcss.str()));
    irLabel->setText(QString::fromStdString("IR: " + cpu.getIR()));
    statusLabel->setText(cpu.isHalted() ? "Status: HALTED" : "Status: READY");

    for (int i = 0; i < 16; i++) {
        uint8_t val = regs.getRegister(i);
        int8_t signedVal = (int8_t)val;
        regTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(regs.getHexRegister(i))));
        regTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(bitset<8>(val).to_string())));
        regTable->setItem(i, 2, new QTableWidgetItem(QString::number((int)signedVal)));
    }

    for (int r = 0; r < 16; r++) {
        for (int c = 0; c < 16; c++) {
            int addr = r * 16 + c;
            QTableWidgetItem* item = new QTableWidgetItem(QString::fromStdString(mem.getHexCell(addr)));
            if (addr == pc || addr == pc + 1) {
                item->setBackground(QColor(255, 240, 180));
            }
            memTable->setItem(r, c, item);
        }
    }

    screenAsciiBox->setPlainText(QString::fromStdString(scr.getAsciiOutput()));
    screenHexBox->setPlainText(QString::fromStdString(scr.getHexOutput()));
}

void VoleGuiWindow::onLoadData() {
    string tempFile = "temp_gui_program.txt";
    ofstream out(tempFile);
    out << codeEditor->toPlainText().toStdString();
    out.close();

    int startAddr = 0x10;
    try {
        startAddr = stoi(startAddrEdit->text().toStdString(), 0, 16);
    } catch (...) {
        QMessageBox::warning(this, "Invalid Address", "Start address must be a valid hex number.");
        return;
    }

    machine.reset(startAddr);
    if (!machine.loadProgram(tempFile, startAddr)) {
        QMessageBox::warning(this, "Load Error", "Program contains invalid instructions or bad start address (must be even < FF).");
        return;
    }
    refreshDisplay();
}

void VoleGuiWindow::onLoadFromFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Vole Program", "", "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty()) return;

    ifstream in(fileName.toStdString());
    if (in.is_open()) {
        stringstream buffer;
        buffer << in.rdbuf();
        codeEditor->setPlainText(QString::fromStdString(buffer.str()));
        in.close();
        onLoadData();
    }
}

void VoleGuiWindow::onRunAll() {
    machine.runAll();
    refreshDisplay();
}

void VoleGuiWindow::onSingleStep() {
    machine.step();
    refreshDisplay();
}

void VoleGuiWindow::onHalt() {
    while (!machine.getCPU().isHalted()) {
        break;
    }
    statusLabel->setText("Status: HALTED (User Stopped)");
}

void VoleGuiWindow::onClearMemory() {
    machine.reset(0);
    refreshDisplay();
}

void VoleGuiWindow::onHelp() {
    QMessageBox::information(this, "Vole Machine Help",
        "Vole Machine Simulator (CS213 - 2026/27)\n\n"
        "Instructions Supported:\n"
        "1 RXY : LOAD R from Memory[XY]\n"
        "2 RXY : LOAD R with immediate XY\n"
        "3 RXY : STORE R to Memory[XY] (3 RFF writes to Screen)\n"
        "4 0RS : MOVE register R to register S\n"
        "5 RST : ADD S and T (2's Complement) -> R\n"
        "6 RST : ADD S and T (Floating-Point Excess-4) -> R\n"
        "7 RST : OR S and T -> R\n"
        "8 RST : AND S and T -> R\n"
        "9 RST : XOR S and T -> R\n"
        "A R0X : ROTATE R right X times\n"
        "B RXY : JUMP to XY if R == R0\n"
        "C 000 : HALT execution");
}
