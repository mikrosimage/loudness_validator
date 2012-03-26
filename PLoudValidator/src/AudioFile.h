#ifndef AUDIOFILE_H
#define AUDIOFILE_H

#include <QtGui/QtGui>

class AudioFile : public QWidget
{
  Q_OBJECT
public:
  AudioFile              ( QString text, QStatusBar* mainStatusBar, QWidget *parent = 0 );
  
  void    setText        ( QString text );

  void    dragEnterEvent ( QDragEnterEvent *event );
  void    dropEvent      ( QDropEvent      *event );
  
  QString getFilename    ( );
  
private slots:
  void openFile          ( );
  void setFilename       ( QString          filename );
  
private:
  QGridLayout   grid;
  QLabel        label;
  QLineEdit     lineEdit;
  QPushButton   button;
  QStatusBar   *statusBar;
  QString       lastSelectedAdress;
};

#endif // AUDIOFILE_H
