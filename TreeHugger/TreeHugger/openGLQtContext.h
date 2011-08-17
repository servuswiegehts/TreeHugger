#ifndef __OPENGLQTCONTEXT_H__
#define __OPENGLQTCONTEXT_H__

#define BUFFER_OFFSET(i) ((char*) NULL + (i))

//Eigende Header
#include "shader.h"
#include "cylinder.h"
#include "tree.h"
#include "lsystem.hpp"
#include "ocl.h"
//
//GLM-Header
	//GLM_SWIZZLE: erlaubt GLSL-ähnlichen Zugriff auf Vektorkomponenten, 
	//z.B. glm::vec4 a();
	//	   glm::vec2 b();
	//	   B.yx = A.wy;
	//     B = A.xx;
	//Vorsicht! Kann Namenskonflikte mit anderen Headern/Libraries erzeugen
//#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/ext.hpp>
//
//QT-Header
#include <QtOpenGL>
#include <QGLWidget>
#include <QObject>
#include <QTimerEvent>
#include <QPainter>
//
//STL-Header
#include <iostream>
#include <vector>
#include <map>
#include <ctime>
#include<sstream>
#include <cmath>
//
//OCL-Header
#include <CL/cl.h>
#include <CL/cl_gl.h>
//


class OpenGLQtContext : public QGLWidget
{
	Q_OBJECT	// nötig um Qt signals/slots zu nutzen
	
	public:

							OpenGLQtContext(QGLFormat *context, QWidget *parent = 0);
							~OpenGLQtContext();

			// public, da von mainWindow verwendet
			void			killTree(std::string);
			void			setIteration(int);
			int				getIteration(void)	const;
			void			makeWind();
	
	protected:

			// überschriebene Funktionen der Elternklassen
			void 			initializeGL();
			void 			paintGL();
			void 			resizeGL(int width, int height);


			void 			mousePressEvent(QMouseEvent *);
			//void 			mouseReleaseEvent(QMouseEvent *);
			void 			mouseMoveEvent(QMouseEvent *);
			void			keyPressEvent(QKeyEvent *);
			bool			event(QEvent* event);
			void			touchEvent(QTouchEvent* event);

	private:

		void			initScene();
		void			initTree(std::string a);
		void			genTree();
		void			make_teller();




	public slots:

		void		timeOutFPS();
		void		timeOutWind();
		void		rotTree(bool);


	private:

		// Stuff
		bool			clWindOnOff_;
		int				frameCount_;
		QPoint 			lastPos_;
		QTimer*			timerFPS_;
		QTimer*			glTimer2_;
		QElapsedTimer 	timerSinceLastFrame_;
		QTimer*			timerWind_;
		//
		
		//
		// clWind-Variablen
		//
		ocl				oclObject;
		GLuint			oclvbo[1];
		GLuint			ocltexture[1];
		cl_float4		* source;
		cl_float4		* dest;
		cl_int			X,Y,Z;
		cl_int			size;
		cl_float		force;
		cl_float		visc;
		//

		//
		// aktueller Baum
		//
		std::string		filename_;
		std::string		currentTreeString_;
		tree			currentTree_;
		Lsystem			TreeHugger_;
		//
		bool			calcTreeGrowth_;
		unsigned int	iteration_;
		unsigned int	maxIteration_;
		//
		std::vector<std::vector<float> >		vertexArrayVector_;
		std::vector<std::vector<float> >		radArrayVector_;
		std::vector<std::vector<unsigned int> >	indexArrayVector_;
		std::vector<std::vector<float> >		dirArrayVector_;
		//
		std::vector<std::vector<int>* >			parentArrayVector_;
		std::vector<std::vector<int>* >			childCountArrayVector_;
		std::vector<std::vector<int>* >			childIDsPlaceArrayVector_;
		std::vector<std::vector<int>* >			childIDsArrayVector_;
		std::vector<std::vector<float>* >		thicknessArrayVector_;
		std::vector<unsigned int>				rootVector_;
		//

		//
		// Texturen
		//
		unsigned int	rindeTex_;
		unsigned int	bodenTex_;
		unsigned int	blattTex_;
		unsigned int	grasTex_;
		// Texturdaten
		float*		barkdata_;
		float*		leafdata_;
		float*		tellerdata_;
		float*		grasdata_;	
		//

		//
		// Shader
		//
		Shader*		calcRotProgram_;
		Shader*		applyRotProgram_;
		Shader*		tellerProgram_;
		//

		//
		// Shader-Uniforms
		//
		int			windCount_;
		bool		windOnOff_;
		bool		leafOnOff_;
		bool		texOnOff_;
		glm::vec3	lightPos_;
		//

		//
		// Shader-Transformationsmatrizen
		//
		glm::mat4	projectionMatrix_;
		glm::mat4	viewMatrix_;
		glm::mat4	modelMatrix_;
		// Transformationsmatrizen-Locations
		unsigned int	projectionMatrixLocationCalc_;
		unsigned int	viewMatrixLocationCalc_;
		unsigned int	modelMatrixLocationCalc_;
		//
		unsigned int	projectionMatrixLocationApp_;
		unsigned int	viewMatrixLocationApp_;
		unsigned int	modelMatrixLocationApp_;
		//
		unsigned int	projectionMatrixLocationTeller_;
		unsigned int	viewMatrixLocationTeller_;
		unsigned int	modelMatrixLocationTeller_;
		//

		//
		// Uniform-Locations
		unsigned int	frameTimeLocationCalc_;
		unsigned int	frameCountLocationCalc_;
		unsigned int	windOnOffLocationCalc_;
		//		
		unsigned int	windOnOffLocationApp_;
		unsigned int	leafOnOffLocApp_;
		unsigned int	texturOnOffLocationApp_;
		unsigned int	lightPosLocApp_;
		//

		//
		// VertexArrayObjects/IndexBuffer/VertexBuffer/sonstiges
		//
		// Baum
		unsigned int	baumVAO_;
		unsigned int	baumVerticesBuf_;
		unsigned int	radBuf_;
		unsigned int	dirBuf_;
		unsigned int	baumIBO_;
		//
		unsigned int	vertexCount_;
		unsigned int	indexCount_;
		//
		// Transform-FeedBack-Buffer/Texturen
		unsigned int	angleVeloBufTF_[2];
		unsigned int	posBufTF_[2];
		unsigned int	kiBufTF_[2];
		//
		unsigned int	posTexTF_[2];
		unsigned int	angleVeloTexTF_[2];
		unsigned int	kiTexTF_[2];
		//
		// Teller
		unsigned int	tellerVAO_;
		unsigned int	teller_ver_Buf_;
		unsigned int	teller_tex_Buf_;
		unsigned int	teller_ind_Buf_;
		//
		unsigned int	kreisVAO_;
		unsigned int	kreis_ver_Buf_;
		unsigned int	kreis_tex_Buf_;
		unsigned int	kreis_ind_Buf_;
		//
		unsigned int	indexTellerCount_;
		unsigned int	indexKreisCount_;
		//

		//
		// Eltern-Kind-Buffer/Texturen
		//
		unsigned int		parentTex_;
		unsigned int		childCountTex_;
		unsigned int		childIDsTex_;
		unsigned int		childIDsPlaceTex_;
		unsigned int		thickTex_;
		//
		unsigned int		parentBuf_;
		unsigned int		childCountBuf_;
		unsigned int		childIDsBuf_;
		unsigned int		childIDsPlaceBuf_;
		unsigned int		thickBuf_;
		//


		//std::map<std::string,Cylinder>				oldcylinder_;
		//std::map<std::string,float>					oldradius_;
		//std::map<std::string,Cylinder>::iterator	it_1;
		//std::map<std::string,float>::iterator		it_2;
		//std::map<std::string,float>::iterator		it_3;
		//int			vertex_;	

};


#endif // OPENGLQTCONTEXT_H__
