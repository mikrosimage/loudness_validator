#ifndef QTVECTORVIEWER_H
#define QTVECTORVIEWER_H

#include <QtGui/QLabel>
#include <list>
#include <vector>

class QtVectorViewer : public QLabel
{
    Q_OBJECT
public:
    explicit QtVectorViewer ( QWidget* parent = 0 );
    void paintEvent         ( QPaintEvent * event );
    void addData            ( std::vector<float> datas, int r, int g, int b, int a );

    void clear();
    
private:
    int  scaledXPosition ( float value );
    int  scaledYPosition ( float value );
    void drawVector      ( QPainter *painter, std::vector<float>& datas, QColor& color );

    int   dataOfHistogram;
    float maxValue;
    float minValue;
    
    std::list< std::vector< float > > listOfDatas;
    std::list< QColor > listOfColor;

    int leftBorder;
    int rightBorder;
    int topBorder;
    int bottomBorder;
    
    int valuesOnX;
    int valuesOnY;
};

#endif
