#include "LoudnessMeter.h"
#include <QtGui/QtGui>

LoudnessMeter::LoudnessMeter(QWidget* parent)
    : QLabel(parent)
{
    setMinimumWidth(400);
    setMinimumHeight(300);
    setMaximumWidth(400);
    setMaximumHeight(300);
    setBackgroundRole(QPalette::Dark);
    // setAutoFillBackground ( true );
    // setText("LUFS Meter");
}

void LoudnessMeter::paintEvent(QPaintEvent*)
{
    int side = qMin(width(), height());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.save();
    painter.translate(0.0, height() * 1.3);
    painter.scale(1.0 * width() / side, 1.0 * height() / side);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawPie(-0.25 * side, -side, 1.5 * side, 3.0 * side, 55 * 16, 70 * 16);
    painter.restore();

    painter.setPen(Qt::black);
    QRect rect;
    rect.setSize(QSize(width(), height()));
    // painter.drawText( rect, Qt::AlignHCenter, tr("Loudness Meter [LUFS]") );
}
