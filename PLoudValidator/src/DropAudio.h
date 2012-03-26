#ifndef DROPAUDIO_H
#define DROPAUDIO_H

#include <QtGui/QtGui>

class DropAudio : public QPushButton
{
    Q_OBJECT
public:
    DropAudio( QStatusBar* mainStatusBar, QWidget* parent = 0 );
    DropAudio( const QString& text, QStatusBar* mainStatusBar, QWidget* parent = 0 );
    DropAudio( const QIcon& icon, const QString& text, QStatusBar* mainStatusBar, QWidget* parent = 0 );

    void    dragEnterEvent ( QDragEnterEvent *event );
    void    dropEvent      ( QDropEvent      *event );

signals:
    void    centerChannelWasDropped        ( QString filename );
    void    leftChannelWasDropped          ( QString filename );
    void    rightChannelWasDropped         ( QString filename );
    void    leftSurroundChannelWasDropped  ( QString filename );
    void    rightSurroundChannelWasDropped ( QString filename );
    void    lfeChannelWasDropped           ( QString filename );


private:
    QStatusBar* statusBar;
};

#endif // DROPAUDIO_H
