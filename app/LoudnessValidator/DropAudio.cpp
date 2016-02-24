#include "DropAudio.h"

#include <iostream>
#include <QtNetwork/QUrlInfo>

DropAudio::DropAudio(QStatusBar* mainStatusBar, QWidget* parent)
    : QPushButton(parent)
    , statusBar(mainStatusBar)
{
    setMinimumHeight(50);
    setAcceptDrops(true);
}

DropAudio::DropAudio(const QString& text, QStatusBar* mainStatusBar, QWidget* parent)
    : QPushButton(text, parent)
    , statusBar(mainStatusBar)
{
    setMinimumHeight(50);
    setAcceptDrops(true);
}

DropAudio::DropAudio(const QIcon& icon, const QString& text, QStatusBar* mainStatusBar, QWidget* parent)
    : QPushButton(icon, text, parent)
    , statusBar(mainStatusBar)
{
    setMinimumHeight(50);
    setAcceptDrops(true);
}

void DropAudio::dragEnterEvent(QDragEnterEvent* event)
{
    if(event->mimeData()->hasFormat("text/plain") || event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void DropAudio::dropEvent(QDropEvent* event)
{
    QStringList stringList;
    if(event->mimeData()->hasUrls())
    {
        QList<QUrl> list = event->mimeData()->urls();
        for(int i = 0; i < list.size(); i++)
        {
            QString string = list.at(i).path();
            string.remove(0, 1);
            std::cout << string.toLatin1().constData() << std::endl;
            stringList.append(string);
        }
    }

    if(event->mimeData()->hasFormat("text/plain"))
    {
        QString s = event->mimeData()->text();
        s.remove("file://");
        stringList = s.split("\n", QString::SkipEmptyParts);
    }

    for(int i = 0; i < stringList.size(); i++)
    {
        if(stringList.at(i).contains("L.wav"))
        {
            QFileInfo fi(stringList.at(i));
            if(fi.isReadable() && fi.isFile() && (stringList.at(i).contains(".wav") || stringList.at(i).contains(".aiff")))
            {
                statusBar->clearMessage();
                event->acceptProposedAction();
            }
            else
            {
                statusBar->showMessage("can't load droped file");
            }
            emit leftChannelWasDropped(stringList.at(i));
        }
        if(stringList.at(i).contains("R.wav"))
        {
            QFileInfo fi(stringList.at(i));
            if(fi.isReadable() && fi.isFile() && (stringList.at(i).contains(".wav") || stringList.at(i).contains(".aiff")))
            {
                statusBar->clearMessage();
                event->acceptProposedAction();
            }
            else
            {
                statusBar->showMessage("can't load droped file");
            }
            emit rightChannelWasDropped(stringList.at(i));
        }
        if(stringList.at(i).contains("C.wav"))
        {
            QFileInfo fi(stringList.at(i));
            if(fi.isReadable() && fi.isFile() && (stringList.at(i).contains(".wav") || stringList.at(i).contains(".aiff")))
            {
                statusBar->clearMessage();
                event->acceptProposedAction();
            }
            else
            {
                statusBar->showMessage("can't load droped file");
            }
            emit centerChannelWasDropped(stringList.at(i));
        }
        if(stringList.at(i).contains("Ls.wav"))
        {
            QFileInfo fi(stringList.at(i));
            if(fi.isReadable() && fi.isFile() && (stringList.at(i).contains(".wav") || stringList.at(i).contains(".aiff")))
            {
                statusBar->clearMessage();
                event->acceptProposedAction();
            }
            else
            {
                statusBar->showMessage("can't load droped file");
            }
            emit leftSurroundChannelWasDropped(stringList.at(i));
        }
        if(stringList.at(i).contains("Rs.wav"))
        {
            QFileInfo fi(stringList.at(i));
            if(fi.isReadable() && fi.isFile() && (stringList.at(i).contains(".wav") || stringList.at(i).contains(".aiff")))
            {
                statusBar->clearMessage();
                event->acceptProposedAction();
            }
            else
            {
                statusBar->showMessage("can't load droped file");
            }
            emit rightSurroundChannelWasDropped(stringList.at(i));
        }
        if(stringList.at(i).contains("Lf.wav"))
        {
            QFileInfo fi(stringList.at(i));
            if(fi.isReadable() && fi.isFile() && (stringList.at(i).contains(".wav") || stringList.at(i).contains(".aiff")))
            {
                statusBar->clearMessage();
                event->acceptProposedAction();
            }
            else
            {
                statusBar->showMessage("can't load droped file");
            }
            emit lfeChannelWasDropped(stringList.at(i));
        }
    }
}
