#ifndef AUDIOFILE_H
#define AUDIOFILE_H

#include <QtGui/QtGui>

class AudioFile : public QWidget
{
    Q_OBJECT
public:
    AudioFile              ( QString text, QStatusBar* mainStatusBar, QString iconFile = ":/icons/document-open.png", int widgetSize = 50, QWidget *parent = 0 );

    void    setText        ( QString text );

    void    dragEnterEvent ( QDragEnterEvent *event );
    void    dropEvent      ( QDropEvent      *event );

    QString getFilename    ( );

private slots:
    void openFile          ( );
    void setFilename       ( QString          filename );

signals:
    void fileWasSelected   ( );

private:
    QGridLayout   grid;
    QLabel        label;
    QPushButton   button;
    QStatusBar   *statusBar;
    QString       lastSelectedAdress;
    QFileInfo     filename;
};

#endif
