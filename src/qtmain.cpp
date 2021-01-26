//
// Created by callum on 22/12/2020.
//

#include <QApplication>
#include <QPushButton>
#include "brightnessControlWindow.h"

int main(int argc, char **argv) {
    QApplication app (argc, argv);

    BrightnessControlWindow window;
    window.show();

    return app.exec();
}