#include "AudioFile.h"

#include <iostream>
#include <QtNetwork/QUrlInfo>

AudioFile::AudioFile( QString text, QStatusBar* mainStatusBar, QString iconFile, int widgetSize, QWidget *parent) :
	QWidget            ( parent ),
	grid               ( this ),
	label              ( text ),
	button             ( "", this ),
	statusBar          ( mainStatusBar ),
	lastSelectedAdress ( QDir::home().absolutePath() )
{
	setAcceptDrops           ( true );
	button.setMinimumWidth   ( 100 );
	button.setMaximumWidth   ( 1000 );
	button.setMinimumHeight  ( 30 );
	button.setMaximumHeight  ( 1000 );
	button.showMaximized();
	
	button.setIcon( QIcon( iconFile ) );
	button.setIconSize( QSize( widgetSize, widgetSize ) );

	grid.addWidget( &label,  0, 0 );
	grid.addWidget( &button, 0, 1 );
	
	QDir defaultPath = QDir( "~/Music" );
	
	if( defaultPath.exists () )
		lastSelectedAdress = defaultPath.path();
	
	connect( &button, SIGNAL( clicked( ) ), SLOT( openFile () ) );
}

void AudioFile::setText( QString text )
{
	label.setText( text );
}

void AudioFile::openFile()
{
	lastSelectedAdress = QFileDialog::getOpenFileName(  this, tr("Open wav File"),
														lastSelectedAdress,
														tr("Wav audio (*.wav);;AIFF audio (*.aiff)") );
	filename.setFile( lastSelectedAdress );
	button.setText( filename.fileName() );
	std::cout << "open file for channel " << label.text().toLatin1().constData() << ":\t" << lastSelectedAdress.toLatin1().constData() << std::endl;
	emit fileWasSelected();
}

void AudioFile::setFilename ( QString filename )
{
	button.setText( filename );
	std::cout << "set file for channel " << label.text().toLatin1().constData() << ":\t" << filename.toLatin1().constData() << std::endl;
	emit fileWasSelected();
}

void AudioFile::dragEnterEvent(QDragEnterEvent *event)
{
	if ( event->mimeData()->hasFormat("text/plain") || event->mimeData()->hasUrls() )
		event->acceptProposedAction();
}

void AudioFile::dropEvent ( QDropEvent* event )
{
	QString s;
	if ( event->mimeData()->hasUrls() )
	{
		s = event->mimeData()->urls().at(0).path();
		s.remove( 0,1 ); // remove first caracter on windows: /C:/Documents....
	}
	
	if (event->mimeData()->hasFormat("text/plain"))
	{
		s = event->mimeData()->text();
		s.remove( "file://" );
	}
	
	QFileInfo fi( s );
	if( fi.isReadable() && fi.isFile() && ( s.contains(".wav") || s.contains(".aiff") ) )
	{
		button.setText( s );
		statusBar->clearMessage ();
		event->acceptProposedAction();
	}
	else
	{
		statusBar->showMessage("can't load droped file");
	}
}

QString AudioFile::getFilename()
{
	return filename.absoluteFilePath();
}

