/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "brightnessControlWindow.h"

#ifndef QT_NO_SYSTEMTRAYICON

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QCloseEvent>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QMessageBox>
#include <iostream>

//! [0]
BrightnessControlWindow::BrightnessControlWindow()
{
    // set as a frameless window, since its more like a volume control then a dialog
    setWindowFlag(Qt::FramelessWindowHint,true);
    setWindowFlag(Qt::WindowStaysOnTopHint,true);

    createBrightnessSliderGroupBox();

    createActions();
    createTrayIcon();

//    connect(showIconCheckBox, &QAbstractButton::toggled, trayIcon, &QSystemTrayIcon::setVisible);
    connect(trayIcon, &QSystemTrayIcon::messageClicked, this, &BrightnessControlWindow::messageClicked);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &BrightnessControlWindow::iconActivated);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(brightnessGroupBox);
    setLayout(mainLayout);

    trayIcon->show();

    const QRect &systemTrayPosition = trayIcon->geometry();

    setWindowTitle(tr("Systray"));
    QSize *windowSize = new QSize(400, 100);
    resize(*windowSize);
    move(calculateWindowPositionNearSystemTray(systemTrayPosition,*windowSize));
}
//! [0]

/**
 * Returns the position to render a window just above the system tray
 * @param systemTrayPosition
 * @param windowSize
 * @return
 */
QPoint BrightnessControlWindow::calculateWindowPositionNearSystemTray(QRect systemTrayPosition, QSize windowSize) {
    QPoint topOfSystemTrayStart = systemTrayPosition.topRight();
    topOfSystemTrayStart.setX( topOfSystemTrayStart.x() );
    topOfSystemTrayStart.setY( topOfSystemTrayStart.y() - windowSize.height() );
    return topOfSystemTrayStart;
}

//! [1]
void BrightnessControlWindow::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QDialog::setVisible(visible);
}
//! [1]

//! [2]
void BrightnessControlWindow::closeEvent(QCloseEvent *event)
{
#ifdef Q_OS_MACOS
    if (!event->spontaneous() || !isVisible()) {
        return;
    }
#endif
    if (trayIcon->isVisible()) {
        hide();
        event->ignore();
    }
}
//! [2]

//! [4]
void BrightnessControlWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        std::cout << "Tray icon single clicked" << std::endl;
        // toggle visibility on icon click
        if (!isVisible()) {
            show();
        } else {
            hide();
        }
        break;
    case QSystemTrayIcon::DoubleClick:

        break;
    case QSystemTrayIcon::MiddleClick:
        break;
    default:
        ;
    }
}
//! [4]

//! [6]
void BrightnessControlWindow::messageClicked()
{
    QMessageBox::information(nullptr, tr("Systray"),
                             tr("Sorry, I already gave what help I could.\n"
                                "Maybe you should try asking a human?"));
}
//! [6]

void BrightnessControlWindow::createBrightnessSliderGroupBox()
{
    brightnessGroupBox = new QGroupBox(tr("Monitor Brightness"));


    QHBoxLayout *iconLayout = new QHBoxLayout;


    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setFocusPolicy(Qt::StrongFocus);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setTickInterval(10);
    slider->setSingleStep(1);

    iconLayout->addWidget(slider);
    iconLayout->addStretch();
    brightnessGroupBox->setLayout(iconLayout);
}


void BrightnessControlWindow::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, &QAction::triggered, this, &QWidget::showMaximized);

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void BrightnessControlWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
//    QIcon icon = QIcon("./../sunwhite.ico");
    QIcon icon = QIcon(":/taskbar-icon.ico");
    trayIcon->setIcon(icon);
    trayIcon->setVisible(true);
    std::cout << "Tray icon created" << std::endl;


}

#endif
