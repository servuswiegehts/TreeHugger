/*
	qTUIO - TUIO Interface for Qt

	Original Version by Martin Blankenburg <martin.blankenburg@imis.uni-luebeck.de>
	Integrated into qTUIO by x29a <0.x29a.0@gmail.com>

	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QTUIO_H
#define QTUIO_H



#include <QThread>
#include <QMap>
#include <QEvent>
#include <QTouchEvent>
#include "./3rdparty/tuio/TuioListener.h"

namespace TUIO {
    class TuioClient;
    class TuioCursor;
}

class QGraphicsView;
class QGraphicsScene;
class QMainWindow;
class QRect;

class QTuio : public QThread, public TUIO::TuioListener
{

   Q_OBJECT
	   
public:
	QTuio(QObject *parent = 0);
    ~QTuio();

    void run();

    virtual void addTuioObject(TUIO::TuioObject *tobj);
    virtual void updateTuioObject(TUIO::TuioObject *tobj);
    virtual void removeTuioObject(TUIO::TuioObject *tobj);
    virtual void addTuioCursor(TUIO::TuioCursor *tcur);
    virtual void updateTuioCursor(TUIO::TuioCursor *tcur);
    virtual void removeTuioCursor(TUIO::TuioCursor *tcur);
    virtual void refresh(TUIO::TuioTime ftime);

private:
//    QGraphicsScene *theScene;
//    QGraphicsView *theView;
    QMainWindow *theMainWindow;
    QRect screenRect;
    TUIO::TuioClient *tuioClient;
    QMap<int, QTouchEvent::TouchPoint> *qTouchPointMap;
    bool tuioToQt(TUIO::TuioCursor *tcur, QEvent::Type eventType);
    bool running;

public:

 signals:
     void touchSignal(QTouchEvent*);
};

#endif // QTUIO_H
