#ifndef LOUDNESSHISTORY_H
#define LOUDNESSHISTORY_H

#include <QtGui/QtGui>


class LoudnessHistory : public QLabel
{
    Q_OBJECT
public:
    LoudnessHistory( QWidget* parent = 0 );
   /* 
private:
    void paintEvent( QPaintEvent *event );*/
};

#endif
