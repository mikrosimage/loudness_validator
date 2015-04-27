#ifndef QTVECTORHISTOGRAM_H
#define QTVECTORHISTOGRAM_H

#include <QtGui/QLabel>
#include <list>
#include <vector>

class QtVectorHistogram : public QLabel
{
    Q_OBJECT
public:
    explicit QtVectorHistogram ( QWidget* parent = 0 );
    void paintEvent         ( QPaintEvent * event );
    void addData            ( std::vector<int> datas, int r, int g, int b, int a );

    void clear();
    
private:
    int  scaledXPosition ( float value );
    int  scaledYPosition ( float value );
    void drawVector      ( QPainter *painter, std::vector<int>& datas, QColor& color );

    int   dataOfHistogram;
    int   maxValue;
    int   minValue;
    
    std::list< std::vector<int> > listOfDatas;
    std::list< QColor > listOfColor;

    int leftBorder;
    int rightBorder;
    int topBorder;
    int bottomBorder;
    
    int valuesOnX;
    int valuesOnY;
};


#endif
