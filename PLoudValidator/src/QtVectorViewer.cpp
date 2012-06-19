#include "QtVectorViewer.h"
#include <QtGui/QtGui>
#include <cmath>
#include <iostream>

QtVectorViewer::QtVectorViewer( QWidget* parent ) :
  QLabel        ( parent ),
  maxValue      (   0.0 ),
  minValue      ( -70.0 ),
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

void QtVectorViewer::addData( std::vector<float> datas, int r, int g, int b, int a )
{
	if(listOfDatas.size() != 0 )
	{
		if( listOfDatas.front().size() != datas.size() )
		{
			std::cout << "warning: datas are not same size ( list of datas: " << listOfDatas.front().size() << ", datas: " << datas.size() << " )." << std::endl;
			return;
		}
	}
	listOfDatas.push_back( datas );
	
	dataOfHistogram  = listOfDatas.front().size();
	
	std::vector<float>::iterator it;
	for ( it=datas.begin() ; it != datas.end(); it++ )
	{
		minValue = std::min( *it, minValue );
		maxValue = std::max( *it, maxValue );
	}
	listOfColor.push_back( QColor( r, g, b, a ) );
	
	valuesOnX = datas.size() - 1;
}

void QtVectorViewer::clear()
{
	maxValue =   0.0;
	minValue = -70.0;
	dataOfHistogram = 0;
	listOfDatas.clear();
	listOfColor.clear();
	update();
}

int  QtVectorViewer::scaledXPosition ( float value )
{
	return   1.0 * ( width() - leftBorder - rightBorder ) * value / valuesOnX + leftBorder ;
}

int  QtVectorViewer::scaledYPosition ( float value )
{
	float a = 1.0 * ( bottomBorder + topBorder - height() ) / ( maxValue - minValue );
	return   a * value + height() - bottomBorder - minValue * a ;
}

void QtVectorViewer::drawVector( QPainter *painter, std::vector<float>& datas, QColor& color )
{
	std::vector<float>::iterator it;
	int index = 1;
	float step = 1.0 * ( scaledXPosition( valuesOnX ) - scaledXPosition( 0.0 ) ) / valuesOnX;
	float xPosition = 0;
	
	painter->setPen( color );
	painter->setBrush( QBrush( color, Qt::SolidPattern ) );
	
	QPointF points[ datas.size() + 2 ];
	points[0] = QPointF( leftBorder, height() - bottomBorder );
	for ( it = datas.begin() ; it != datas.end(); it++, index++, xPosition += step )
	{
		points[index] = QPointF( scaledXPosition( 1.0 * xPosition / step ), scaledYPosition( *it ) );
	}
	points[datas.size()+1] = QPointF( scaledXPosition( 1.0 * ( xPosition - step ) / step ) , height() - bottomBorder );
	
	painter->drawPolygon( points, datas.size() + 2 );
}

void QtVectorViewer::paintEvent ( QPaintEvent * )
{
	QPainter painter( this );
	painter.setRenderHint( QPainter::Antialiasing );
	
	
	painter.setPen( Qt::transparent );
	painter.setBrush( QBrush( QColor( 162, 207, 158, 128 ), Qt::SolidPattern ) );
	painter.drawRect( scaledXPosition( 0.0 ), scaledYPosition( -23.0 ), scaledXPosition( valuesOnX ) - scaledXPosition( 0.0 ), scaledYPosition( minValue ) - scaledYPosition( -23.0 ) );
	painter.setBrush( QBrush( QColor( 219, 140, 21, 128 ), Qt::SolidPattern ) );
	painter.drawRect( scaledXPosition( 0.0 ), scaledYPosition( -20.0 ), scaledXPosition( valuesOnX ) - scaledXPosition( 0.0 ), scaledYPosition( -23.0 ) - scaledYPosition( -20.0 ) );
	painter.setBrush( QBrush( QColor( 219,  75,  26, 128 ), Qt::SolidPattern ) );
	painter.drawRect( scaledXPosition( 0.0 ), scaledYPosition(   0.0 ), scaledXPosition( valuesOnX ) - scaledXPosition( 0.0 ), scaledYPosition( -20.0 ) - scaledYPosition(   0.0 ) );
	
	painter.setPen( QColor( 0, 0, 0, 128 ) );
	painter.drawText( ( scaledXPosition( valuesOnX ) - scaledXPosition( 0.0 ) ) * 0.5, scaledYPosition( -23.0 ), "-23.0" );
	painter.setPen( QColor( 0, 0, 0, 128 ) );
	painter.drawText( ( scaledXPosition( valuesOnX ) - scaledXPosition( 0.0 ) ) * 0.7, scaledYPosition( -20.0 ), "-20.0" );
	
	painter.setPen( Qt::black );
	painter.setBrush( QBrush( Qt::black, Qt::SolidPattern ) );
	
	std::list< std::vector<float> >::iterator it;
	std::list< QColor                 >::iterator itColor;
	
	for ( it = listOfDatas.begin(), itColor = listOfColor.begin() ; it != listOfDatas.end(); it++, itColor++ )
	{
		drawVector( &painter, *it, *itColor );
	}
	
	painter.setPen( Qt::black );
	painter.setBrush( QBrush( Qt::black, Qt::SolidPattern ) );
	painter.drawText( 0, scaledYPosition( maxValue ), QString::number( maxValue ) );
	painter.drawText( 0, scaledYPosition( minValue ), QString::number( minValue ) );
	painter.drawText( 0, scaledYPosition( 0.5 * ( maxValue + minValue ) ), QString::number( ( minValue + maxValue ) * 0.5 ) );
	
	painter.drawLine( scaledXPosition( 0.0 ), scaledYPosition( minValue ), scaledXPosition( 0.0 ), scaledYPosition( maxValue ) );
	painter.drawLine( scaledXPosition( 0.0 ), scaledYPosition( minValue ), scaledXPosition( valuesOnX ), scaledYPosition( minValue ) );
	
	QRect rect;
	rect.setSize      ( QSize ( width(), height() ) );
}
