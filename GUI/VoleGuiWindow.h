#ifndef VOLEGUIWINDOW_H
#define VOLEGUIWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include "../Machine.h"

class VoleGuiWindow : public QMainWindow {
    Q_OBJECT

private:
    Machine machine;
    QPlainTextEdit* codeEditor;
    QLineEdit* startAddrEdit;
    QTableWidget* regTable;
    QTableWidget* memTable;
    QLabel* pcLabel;
    QLabel* irLabel;
    QLabel* statusLabel;
    QPlainTextEdit* screenHexBox;
    QPlainTextEdit* screenAsciiBox;

    void setupUi();
    void refreshDisplay();

private slots:
    void onLoadData();
    void onLoadFromFile();
    void onRunAll();
    void onSingleStep();
    void onHalt();
    void onClearMemory();
    void onHelp();

public:
    VoleGuiWindow(QWidget* parent = nullptr);
};

#endif
