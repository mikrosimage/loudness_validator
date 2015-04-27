#include "QtVectorHistogram.h"
#include <QtGui/QtGui>
#include <cmath>
#include <iostream>

QtVectorHistogram::QtVectorHistogram( QWidget* parent ) :
  QLabel        ( parent ),
  maxValue      ( 2.0 ),
  minValue      ( 0.0 ),
  leftBorder    ( 20 ),
  rightBorder   ( 10 ),
  topBorder     ( 10 ),
  bottomBorder  ( 20 ),
  valuesOnX     ( 10 ),
  valuesOnY     ( 10 )
{
	setMinimumWidth   (  100 );
	setMinimumHeight  (  100 );
	setMaximumWidth   ( 1000 );
	setMaximumHeight  ( 1000 );
	setBackgroundRole ( QPalette::Dark );
}

void QtVectorHistogram::addData( std::vector<int> datas, int r, int g, int b, int a )
{
	if(listOfDatas.size() != 0 )
	{
		if( listOfDatas.front().size() != datas.size() )
		{
			std::cout << "warning: QtVectorHistogram font and data size are differents : " << listOfDatas.front().size() << " != " << datas.size() << std::endl;
			return;
		}
	}
	listOfDatas.push_back( datas );

	dataOfHistogram  = listOfDatas.front().size();

	std::vector<int>::iterator it;
	for ( it=datas.begin() ; it != datas.end(); it++ )
	{
		minValue = std::min( *it, minValue );
		maxValue = std::max( *it, maxValue );
	}
	listOfColor.push_back( QColor( r, g, b, a ) );

	valuesOnX = datas.size() - 1;
}

void QtVectorHistogram::clear()
{
	maxValue = 2.0;
	minValue = 0.0;
	dataOfHistogram = 0;
	listOfDatas.clear();
	listOfColor.clear();
	update();
}

int  QtVectorHistogram::scaledXPosition ( float value )
{
	return   1.0 * ( width() - leftBorder - rightBorder ) * value / valuesOnX + leftBorder ;
}

int  QtVectorHistogram::scaledYPosition ( float value )
{
	float a = 1.0 * ( bottomBorder + topBorder - height() ) / ( maxValue - minValue );
	return   a * value + height() - bottomBorder - minValue * a ;
}

void QtVectorHistogram::drawVector( QPainter *painter, std::vector<int>& datas, QColor& color )
{
	std::vector<int>::iterator it;
	int index = 1;
	float step = 1.0 * ( scaledXPosition( valuesOnX ) - scaledXPosition( 0.0 ) ) / valuesOnX;
	float xPosition = 0;
	
	painter->setPen( color );
	painter->setBrush( QBrush( color, Qt::SolidPattern ) );
	
	
	std::vector<QPointF> points;
	points.reserve( datas.size() + 2 );
	points.push_back( QPointF( leftBorder, height() - bottomBorder ) );
	for ( it = datas.begin() ; it != datas.end(); it++, index++, xPosition += step )
	{
		points.push_back( QPointF( scaledXPosition( 1.0 * xPosition / step ), scaledYPosition( *it ) ) );
	}
	points.push_back( QPointF( scaledXPosition( 1.0 * ( xPosition - step ) / step ) , height() - bottomBorder ) );

	painter->drawPolygon( &points.at(0), datas.size() + 2 );
}

void QtVectorHistogram::paintEvent ( QPaintEvent * )
{
	QPainter painter( this );
	painter.setRenderHint( QPainter::Antialiasing );
	
	
	painter.setPen( Qt::transparent );
	painter.setBrush( QBrush( QColor( 162, 207, 158, 128 ), Qt::SolidPattern ) );
	painter.drawRect( scaledXPosition( 0.0 ), scaledYPosition( 0.0 ), scaledXPosition( valuesOnX ) - scaledXPosition( 0.0 ), scaledYPosition( minValue ) - scaledYPosition( 0.0 ) );
	
	painter.setPen( Qt::black );
	painter.setBrush( QBrush( Qt::black, Qt::SolidPattern ) );
	
	std::list< std::vector<int> >::iterator it;
	std::list< QColor           >::iterator itColor;
	
	for ( it = listOfDatas.begin(), itColor = listOfColor.begin() ; it != listOfDatas.end(); it++, itColor++ )
	{
		drawVector( &painter, *it, *itColor );
	}
	
	painter.setPen( Qt::black );
	painter.setBrush( QBrush( Qt::black, Qt::SolidPattern ) );
	painter.drawText( 0, scaledYPosition( maxValue ), QString::number( maxValue ) );
	painter.drawText( 0, scaledYPosition( minValue ), QString::number( minValue ) );
	painter.drawText( 0, scaledYPosition( 0.5 * ( maxValue + minValue ) ), QString::number( ( minValue + maxValue ) * 0.5 ) );
	
	painter.drawText( scaledXPosition( 0.0 )      , height() - bottomBorder * 0.5, QString::number( -70.0 ) );
	painter.drawText( scaledXPosition( valuesOnX ), height() - bottomBorder * 0.5, QString::number( 5.0 ) );
	//painter.drawText( scaledXPosition( -23.0 )    , scaledYPosition( minValue - 0.1 ), QString::number( -23.0 ) );
	
	painter.drawLine( scaledXPosition( 0.0 ), scaledYPosition( minValue ), scaledXPosition( 0.0 ), scaledYPosition( maxValue ) );
	painter.drawLine( scaledXPosition( 0.0 ), scaledYPosition( minValue ), scaledXPosition( valuesOnX ), scaledYPosition( minValue ) );
	
	QRect rect;
	rect.setSize      ( QSize ( width(), height() ) );
}
