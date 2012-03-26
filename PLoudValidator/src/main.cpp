#include <QtGui/QApplication>
#include <QtGui/QPlastiqueStyle>
#include "PLoudGui.h"


int main(int argc, char** argv)
{
    QApplication::setStyle( new QPlastiqueStyle );
    QApplication app(argc, argv);
    PLoudGui foo;
    foo.show();
    return app.exec();
}
