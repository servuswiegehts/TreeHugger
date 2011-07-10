#ifndef __OPENGLQTCONTEXT_H__
#define __OPENGLQTCONTEXT_H__

//#include "debug.h"
//GLM_SWIZZLE: erlaubt GLSL-ähnlichen Zugriff auf Vektorkomponenten, 
//z.B. glm::vec4 a();
//	   glm::vec2 b();
//	   B.yx = A.wy;
//     B = A.xx;
//Vorsicht! Kann Namenskonflikte mit anderen Headern/Libraries erzeugen
//#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"
//#include "glm/glm/gtc/matrix_transform.hpp"

#define WINDOW_TITLE_PREFIX "Title"
#define NUMBER_OF_SHADERS 2
#define BUFFER_OFFSET(i) ((char*) NULL + (i))

#include "shader.h"

#include <QtOpenGL>
#include <QGLWidget>
#include <QObject>
#include <cmath>
#include <QTimerEvent>


#include <iostream>
#include <vector>
#include <map>



class OpenGLQtContext : public QGLWidget
{
	Q_OBJECT	// must include this if you use Qt signals/slots
	
	public:
				OpenGLQtContext(QGLFormat *context, QWidget *parent = 0);
				~OpenGLQtContext();
	
	protected:
		void 		initializeGL();
		void 		paintGL();
		void 		resizeGL(int width, int height);
		void 		mousePressEvent(QMouseEvent *);
		void 		mouseReleaseEvent(QMouseEvent *);
		void 		mouseMoveEvent(QMouseEvent *);
		
		void		keyPressEvent(QKeyEvent *);

	private:

		void		initScene();

		bool		initVertexArrayObject();

		void		genTree();

		
	public slots:
		void		timeOut();
		void		timeOut2();
		
	private:		
		QPoint 			lastPos_;
		QTimer*			glTimer_;
		QTimer*			glTimer2_;
		QElapsedTimer 		timer_;
		
		int			frameCount_;
		int			richtung_;
		float			timeSinceLastFrame_;
		
		glm::mat4		projectionMatrix_;
		glm::mat4		viewMatrix_;
		glm::mat4		modelMatrix_;
		glm::mat4		modelMatrix2_;

		unsigned int		vaoID_[4];
		unsigned int		vboID_[8];
		unsigned int		iboID_[4];

		unsigned int		tfID_[7];
		unsigned int		tfoID_[2];
		unsigned int		texBuf_[2];
		unsigned int		texID_[2];

		unsigned int		radBuf_;

		//std::vector<Shader*>	shader_;
		Shader*		calcRotProgram_;
		Shader*		addRotProgram_;
		Shader*		applyRotProgram_;

		int		windowWidth_;
		int		windowHeight_;

		unsigned int		fbo_;
		unsigned int		fboTex_;
		unsigned int		fboDepth_;

		unsigned int		parentTex_;
		unsigned int		childCountTex_;
		unsigned int		childIDsTex_;
		unsigned int		forcesTex_;
		unsigned int		kiTex_[2];
		unsigned int		thickTex_;
		unsigned int		angleTex_;
		unsigned int		angleVeloTex_;
		unsigned int		childIDsPlaceTex_;
		unsigned int		posTex_[2];
		unsigned int		tfTex_[4];

		unsigned int		parentBuf_;
		unsigned int		childCountBuf_;
		unsigned int		childIDsBuf_;
		unsigned int		forcesBuf_;
		unsigned int		kiBuf_[2];
		unsigned int		thickBuf_;
		unsigned int		childIDsPlaceBuf_;

		int			projectionMatrixLocationCalc_;
		int			viewMatrixLocationCalc_;
		int			modelMatrixLocationCalc_;
		int			frameTimeLocationCalc_;
		int			frameCountLocationCalc_;

		int			projectionMatrixLocationApp_;
		int			viewMatrixLocationApp_;
		int			modelMatrixLocationApp_;

		int			projectionMatrixLocationAdd_;
		int			viewMatrixLocationAdd_;
		int			modelMatrixLocationAdd_;
};

std::ostream&	operator<<(std::ostream&, glm::mat4 const&);

#endif // OPENGLQTCONTEXT_H__
