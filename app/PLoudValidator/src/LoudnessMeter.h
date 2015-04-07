#ifndef LOUDNESSMETER_H
#define LOUDNESSMETER_H

#include <QLabel>

class LoudnessMeter : public QLabel
{
    Q_OBJECT
public:
    explicit LoudnessMeter(QWidget *parent = 0);

    void paintEvent ( QPaintEvent * event );
signals:

public slots:

};

#endif // LOUDNESSMETER_H
