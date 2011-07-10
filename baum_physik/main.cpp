#include <GL/glew.h>

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtOpenGL>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>


#include "openGLQtContext.h"
bool		checkForError (std::string const& command);


int main(int argc, char **argv)
{
//	Q_INIT_RESOU+RCE(pbuffers);

	

	QApplication a(argc, argv);
	if (!QGLFormat::hasOpenGL()) {
		QMessageBox::information(0, "OpenGL", "This system does not support OpenGL.");
		return -1;
	}

	QGLFormat* form = new QGLFormat();
	form->setVersion(4, 0);
	form->setDoubleBuffer (TRUE);
	form->setDepth (TRUE);
	form->setAlpha (TRUE);
	form->setProfile (QGLFormat::CoreProfile);
	form->setOption (QGL::NoDeprecatedFunctions);
	


	OpenGLQtContext widget (form, (QWidget*)0);
	widget.resize(600, 600);
	widget.move(500,500);
	widget.show();
	
	
	return a.exec();
}
