#include <GL/glew.h>

#include "externalFunc.h"

#include <QtCore/QtCore>
#include <QtGui/QtGui>
//#include <QtOpenGL>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <qwidget.h>
#include <qimage.h>
//#include <QtOpenGL>
//#include <QGLWidget>
#include <QObject>
#include <QTimerEvent>
#include <QPainter>


#include "mainWindow.h"



int main(int argc, char **argv)
{
	QApplication a(argc, argv);
	if (!QGLFormat::hasOpenGL()) {
		QMessageBox::information(0, "OpenGL", "This system does not support OpenGL.");
		return -1;
	}

	MainWindow widget;
	widget.resize(1800, 900);
	widget.move(800,200);
	widget.showFullScreen();
	//widget.show();
	return a.exec();
}
