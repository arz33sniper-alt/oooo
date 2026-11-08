#include <QApplication>
#include "VoleGuiWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    VoleGuiWindow window;
    window.show();
    return app.exec();
}
