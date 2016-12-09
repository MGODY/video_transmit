/********************************************************************************
** Form generated from reading UI file 'client.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLIENT_H
#define UI_CLIENT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_1;
    QAction *action_2;
    QAction *action_3;
    QAction *action_4;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QGridLayout *gridLayout;
    QLabel *label_4;
    QLabel *label_3;
    QLabel *label_1;
    QLabel *label_2;
    QGridLayout *gridLayout_2;
    QPushButton *Buttron_accept;
    QPushButton *Button_stop;
    QMenuBar *menubar;
    QMenu *chose;
    QStatusBar *statusbar;
    QToolBar *toolBar;
    QToolBar *toolBar_2;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->setEnabled(true);
        MainWindow->resize(960, 805);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        action_1 = new QAction(MainWindow);
        action_1->setObjectName(QString::fromUtf8("action_1"));
        action_2 = new QAction(MainWindow);
        action_2->setObjectName(QString::fromUtf8("action_2"));
        action_3 = new QAction(MainWindow);
        action_3->setObjectName(QString::fromUtf8("action_3"));
        action_4 = new QAction(MainWindow);
        action_4->setObjectName(QString::fromUtf8("action_4"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setFrameShape(QFrame::Box);

        gridLayout->addWidget(label_4, 1, 1, 1, 1);

        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setFrameShape(QFrame::Box);

        gridLayout->addWidget(label_3, 1, 0, 1, 1);

        label_1 = new QLabel(centralwidget);
        label_1->setObjectName(QString::fromUtf8("label_1"));
        label_1->setEnabled(true);
        sizePolicy.setHeightForWidth(label_1->sizePolicy().hasHeightForWidth());
        label_1->setSizePolicy(sizePolicy);
        label_1->setFrameShape(QFrame::Box);
        label_1->setLineWidth(1);
        label_1->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_1, 0, 0, 1, 1);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFrameShape(QFrame::Box);

        gridLayout->addWidget(label_2, 0, 1, 1, 1);


        horizontalLayout->addLayout(gridLayout);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        Buttron_accept = new QPushButton(centralwidget);
        Buttron_accept->setObjectName(QString::fromUtf8("Buttron_accept"));

        gridLayout_2->addWidget(Buttron_accept, 0, 0, 1, 1);

        Button_stop = new QPushButton(centralwidget);
        Button_stop->setObjectName(QString::fromUtf8("Button_stop"));

        gridLayout_2->addWidget(Button_stop, 1, 0, 1, 1);


        horizontalLayout->addLayout(gridLayout_2);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 960, 28));
        chose = new QMenu(menubar);
        chose->setObjectName(QString::fromUtf8("chose"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);
        toolBar_2 = new QToolBar(MainWindow);
        toolBar_2->setObjectName(QString::fromUtf8("toolBar_2"));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar_2);

        menubar->addAction(chose->menuAction());
        chose->addAction(action_1);
        chose->addAction(action_2);
        chose->addAction(action_3);
        chose->addAction(action_4);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        action_1->setText(QApplication::translate("MainWindow", "\345\233\276\345\203\217\344\270\200", 0, QApplication::UnicodeUTF8));
        action_2->setText(QApplication::translate("MainWindow", "\345\233\276\345\203\217\344\272\214", 0, QApplication::UnicodeUTF8));
        action_3->setText(QApplication::translate("MainWindow", "\345\233\276\345\203\217\344\270\211", 0, QApplication::UnicodeUTF8));
        action_4->setText(QApplication::translate("MainWindow", "\345\233\276\345\203\217\345\233\233", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_1->setText(QApplication::translate("MainWindow", "\346\262\241\346\234\211\346\216\245\345\217\227\345\210\260\345\233\276\345\203\217", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "NO frame", 0, QApplication::UnicodeUTF8));
        Buttron_accept->setText(QApplication::translate("MainWindow", "\345\274\200\345\247\213\346\216\245\345\217\227\345\233\276\345\203\217", 0, QApplication::UnicodeUTF8));
        Button_stop->setText(QApplication::translate("MainWindow", "\345\201\234\346\255\242\346\216\245\345\217\227", 0, QApplication::UnicodeUTF8));
        chose->setTitle(QApplication::translate("MainWindow", "\351\200\211\346\213\251", 0, QApplication::UnicodeUTF8));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0, QApplication::UnicodeUTF8));
        toolBar_2->setWindowTitle(QApplication::translate("MainWindow", "toolBar_2", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIENT_H
