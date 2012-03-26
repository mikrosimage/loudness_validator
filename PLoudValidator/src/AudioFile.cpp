#include "AudioFile.h"

#include <iostream>
#include <QtNetwork/QUrlInfo>

AudioFile::AudioFile( QString text, QStatusBar* mainStatusBar, QWidget *parent) :
  QWidget            ( parent ),
  grid               ( this ),
  label              ( text ),
  lineEdit           ( this ),
  button             ( "Open", this ),
  statusBar          ( mainStatusBar ),
  lastSelectedAdress ( QDir::home().absolutePath() )
{
  lineEdit.setAcceptDrops  ( false );
  setAcceptDrops           ( true );
  label.setMinimumWidth    ( 100 );
  lineEdit.setMinimumWidth ( 50  );
  lineEdit.setMaximumWidth ( 300 );
  button.setMinimumWidth   ( 100 );
  button.setMaximumWidth   ( 100 );
  
  button.setIcon( QIcon(":/icons/document-open.png") );

  grid.addWidget ( &label    , 0, 0 );
  grid.addWidget ( &lineEdit , 0, 1 );
  grid.addWidget ( &button   , 0, 2 );
  
  connect( &button, SIGNAL( clicked( ) ), SLOT( openFile () ) );
}

void AudioFile::setText( QString text )
{
  label.setText( text );
}

void AudioFile::openFile()
{
  lastSelectedAdress = QFileDialog::getOpenFileName(this, tr("Open wav File"),
                                                  lastSelectedAdress,
						  tr("Wav audio (*.wav);;AIFF audio (*.aiff)") );
  lineEdit.setText( lastSelectedAdress );
  std::cout << "open file for channel " << label.text().toLatin1().constData() << ":\t" << lastSelectedAdress.toLatin1().constData() << std::endl;
}

void AudioFile::setFilename ( QString filename )
{
	lineEdit.setText( filename );
        std::cout << "open file for channel " << label.text().toLatin1().constData() << ":\t" << filename.toLatin1().constData() << std::endl;
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
    lineEdit.setText( s );
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
  return lineEdit.text();
}
