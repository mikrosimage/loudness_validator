#include "LoudnessHistory.h"

LoudnessHistory::LoudnessHistory(QWidget* parent)
    : QLabel(parent)
{
    this->setMinimumSize(500, 500);
}
/*
void LoudnessHistory::paintEvent( QPaintEvent * )
{
    QImage image( size(), QImage::Format_ARGB32_Premultiplied );
    QPainter imagePainter( &image );
    imagePainter.initFrom( this );
    imagePainter.setRenderHint( QPainter::Antialiasing, true );
    imagePainter.eraseRect( rect() );
    //draw( &imagePainter );
    imagePainter.end();

    QPainter widgetPainter(this);
    widgetPainter.drawImage( 0, 0, image );
}
*/