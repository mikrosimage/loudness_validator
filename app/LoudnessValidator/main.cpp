#include <QtGui/QApplication>
#include <QtGui/QPlastiqueStyle>
#include "PLoudGui.h"


int main(int argc, char** argv)
{
	QApplication::setStyle( new QPlastiqueStyle );
	QApplication app(argc, argv);
	app.setStyleSheet("QPushButton { background-color: yellow }");
	
	app.setStyleSheet(
						"QMainWindow, QDialog, QMenuBar, QMenu {"
						"		background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
						"		stop: 0 #606060, stop: 1.0 #404040);"
						"}"
				
						"QLabel, QMenuBar, QMenu {"
						"	color:#fff;"
						"}"

						"QMenuBar::item {"
						"	border-radius: 6px;"
						"	color: #fff;"
						"	padding: 0.1em;"
						"	margin: 0.2em;"
						"}"
				
						"QPushButton, QComboBox {"
						"	border: 2px solid #333;"
						"	border-radius: 6px;"
						"	color: #fff;"
						"	padding: 0.1em;"
						"	margin: 0.2em;"
						"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
						"		stop: 0 #606060, stop: 1.0 #000000);"
						"}"

						"QPushButton:hover, QComboBox:hover, QMenuBar::item:selected, QMenu::item:selected {"
						"	background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
						"		stop: 0 #06B5E1, stop: 0.1 #06B5E1,"
						"		stop: 0.2 #385F69 stop: 1.0 #010101);"
						"}"
						"QPushButton:pressed, QComboBox:pressed, QMenuBar::item:pressed, QMenu::item:pressed {"
						"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
						"		stop: 0 #606060, stop: 1.0 #000000);"
						"}"
				
						"QProgressBar { border: 2px solid grey; border-radius: 5px; background: #202020; color:#fff; }"
						"QProgressBar::chunk { background-color: #5FA573; width: 20px; }"
						"QProgressBar { border: 2px solid grey; border-radius: 5px; text-align: center; }"
					
						"QTabWidget::pane {"
						"	border-top: 2px solid #C2C7CB;"
						"	position: absolute;"
						"	top: -1.0em;"
						"	padding-top: 0.8em;"
						"}"
						
						"QTabWidget::tab-bar {"
						"	alignment: center;"
						"}"

						"QTabBar::tab {"
						"	background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
						"		stop: 0 #385F69, stop: 1.0 #223B41);"
						"	color: #fff;"
						"	border-radius: 4px;"
						"	min-width: 20ex;"
						"	min-height: 4ex;"
						"	padding: 4px;"
						"	margin-left: 4px;"
						"	margin-right: 4px;"
						"}"
						
						"QTabBar::tab:selected, QTabBar::tab:hover {"
						"	background: qlineargradient("
						"		x1: 0, y1: 0,x2: 0, y2: 1,"
						"		stop: 0 #06B5E1, stop: 0.1 #06B5E1,"
						"		stop: 0.2 #385F69 stop: 1.0 #223B41);"
						"}"
				
						"QTabBar::tab:hover {"
						"	color: #06B5E1;"
						"}"

						"QLCDNumber, QLabel#result {"
						"	border: 2px solid #333;"
						"	border-radius: 10px;"
						"	padding: 2px;"
						"	background: #ddd;"
						"}"
					  );
	PLoudGui foo;
	foo.show();
	return app.exec();
}
