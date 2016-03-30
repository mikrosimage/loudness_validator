#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QtGui/QtGui>


class HelpDialog : public QDialog
{
    Q_OBJECT

public:

    HelpDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

public slots:
    void closeDialog();

private:
    QGridLayout    layoutHelp;

    QLabel         labelHelp;
    QLabel         labelAbout;

    QTextEdit      textHelp;

    QPushButton    closeHelpDialog;
};

#endif
