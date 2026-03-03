/********************************************************************************
** Form generated from reading UI file 'displaywidget.ui'
**
** Created by: Qt User Interface Compiler version 6.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DISPLAYWIDGET_H
#define UI_DISPLAYWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_displayWidget
{
public:

    void setupUi(QWidget *displayWidget)
    {
        if (displayWidget->objectName().isEmpty())
            displayWidget->setObjectName("displayWidget");
        displayWidget->resize(600, 400);
        displayWidget->setMinimumSize(QSize(600, 400));
        displayWidget->setMaximumSize(QSize(600, 400));

        retranslateUi(displayWidget);

        QMetaObject::connectSlotsByName(displayWidget);
    } // setupUi

    void retranslateUi(QWidget *displayWidget)
    {
        displayWidget->setWindowTitle(QCoreApplication::translate("displayWidget", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class displayWidget: public Ui_displayWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DISPLAYWIDGET_H
