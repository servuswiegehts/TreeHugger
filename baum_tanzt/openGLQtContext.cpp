#include "openGLQtContext.h"

struct Points
{
	glm::vec4 startPoint;
	glm::vec4 endPoint;
	
	Points(glm::vec4, glm::vec4);
	Points();
};

Points::Points(glm::vec4 start, glm::vec4 end) : startPoint(start), endPoint(end) {};
Points::Points() {};

std::ostream& operator<<(std::ostream& stream, Points const& p)
{
	std::cout << p.startPoint.x << ", " << p.startPoint.y << ", " << p.startPoint.z << ", " << p.startPoint.z << " | " << p.endPoint.x << ", " << p.endPoint.y << ", " << p.endPoint.z << ", " << p.endPoint.w << std::endl;
	return stream;
}


static void CALLBACK amdCallback(GLuint id,    GLenum category,        GLenum severity,        GLsizei length, const GLchar* message,  GLvoid* userParam)
{
        std::cout << std::endl << "Debug-CallBack:\n" << "ID: " << id << "\nCategory: " << category << "\nSeverity: " << severity << "\nLaenge: " << length << "\nMessage: " << message << std::endl << std::endl;
}
void setupAmdDebugPrinter()
{
        glDebugMessageEnableAMD(GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
        glDebugMessageCallbackAMD(&amdCallback, NULL);
        glDebugMessageInsertAMD(GL_DEBUG_CATEGORY_APPLICATION_AMD, GL_DEBUG_SEVERITY_HIGH_AMD, 1, 4, "test");
}



//
//Konstruktoren, Destruktoren
//
OpenGLQtContext::OpenGLQtContext(QGLFormat* context, QWidget *parent) :
		QGLWidget 		(*context, parent) ,
		
		lastPos_		(),		
		glTimer_		(),
		glTimer2_		(),
		timer_			(),

		frameCount_		(0),
		richtung_		(1),	
		timeSinceLastFrame_ 	(0.0f),
	
		projectionMatrix_	(glm::mat4(1.0f)),
		viewMatrix_			(glm::mat4(1.0f)),
		modelMatrix_		(glm::mat4(1.0f)),
		modelMatrix2_		(glm::mat4(1.0f)),

		vaoID_			(),
		vboID_			(),
		iboID_			(),

		tfID_			(),
		tfoID_			(),
		texBuf_			(),
		texID_			(),
		radBuf_			(),

		calcRotProgram_ (0),
		addRotProgram_	(0),
		applyRotProgram_(0),

		windowWidth_	(600),
		windowHeight_	(600),

		parentTex_		(0),
		childCountTex_	(0),
		childIDsTex_	(0),
		forcesTex_		(0),
		kiTex_			(),
		thickTex_		(0),
		angleTex_		(0),
		angleVeloTex_	(0),
		childIDsPlaceTex_(0),

		parentBuf_		(0),
		childCountBuf_	(0),
		childIDsBuf_	(0),
		forcesBuf_		(0),
		kiBuf_			(),
		thickBuf_		(0),
		childIDsPlaceBuf_(0),

		projectionMatrixLocationCalc_ (0),
		viewMatrixLocationCalc_       (0),
		modelMatrixLocationCalc_      (0),

		projectionMatrixLocationApp_  (0),
		viewMatrixLocationApp_        (0),
		modelMatrixLocationApp_		  (0),

		projectionMatrixLocationAdd_  (0),
		viewMatrixLocationAdd_		  (0),
		modelMatrixLocationAdd_	      (0)
{

	// Titel setzen
	setWindowTitle(tr("OpenGL"));
	
	// Qt kann in mehrere Contexte zeichnen, dies setzt unseren neu erstellten als aktuellen 
	makeCurrent();
	
	
	if (!isValid())
		std::cout << "ogl context invalid" << std::endl;	
	if (!format().doubleBuffer())
		std::cout << "ogl doublebuffer deaktiviert" << std::endl;
	if ( !format().stencil() )
		qWarning( "Could not get stencil buffer; results will be suboptimal" );
	if ( !format().alpha() )
		qWarning( "Could not get alpha channel; results will be suboptimal" );
	if ( !format().depth() )
		qWarning( "Could not get depth buffer; results will be suboptimal" );	
		
		
		
	setFocusPolicy ( Qt::ClickFocus);
	//grabMouse();
	//grabKeyboard();
	//


	glTimer_ = new QTimer(this);
	connect(glTimer_, SIGNAL(timeout()), this, SLOT(timeOut()));
	glTimer_->start(1000);
	

	glTimer2_ = new QTimer(this);
	connect(glTimer2_, SIGNAL(timeout()), this, SLOT(timeOut2()));
	glTimer2_->start(20);
}
//

OpenGLQtContext::~OpenGLQtContext()
{

}
//


//
// initializeGL
//
// virtuelle Funktion der Elternklasse wird hierdurch überschrieben
// initialisieren von glew, setzen von hintergrundfarbe und einigen OpenGL-states
//
void OpenGLQtContext::initializeGL()
{
	glewExperimental = GL_TRUE;
	GLenum glewInitResult = glewInit();

	if(GLEW_OK != glewInitResult)
	{
		std::cout << "ERROR: "<<glewGetErrorString(glewInitResult) << std::endl;
		exit(EXIT_FAILURE);
	}			
			if (glewIsExtensionSupported("GL_ARB_debug_output"))
			{
					std::cout << "setting up opengl ARB debug output" << std::endl;
					//setupArbDebugPrinter();
			}
			else if (glewIsExtensionSupported("GL_AMD_debug_output"))
			{
					std::cout << "setting up opengl AMD debug output" << std::endl;
					setupAmdDebugPrinter();
			}
			else
			{
					std::cout << "failed to setup opengl debug printer ARB/AMD" << std::endl;
			}


	std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	int contflags=888;
	glGetIntegerv(GL_CONTEXT_FLAGS, &contflags);
	std::cout << "Context-Flags: " << contflags << std::endl;

	glClearDepth(1.0f);
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );


	glEnable( GL_DEPTH_TEST );
	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	initScene();
}
//


//
// initScene
//
// Erstellen von Shadern, Objekten (z.B. Zylinder)
// Setzen von Matrizen
//
void OpenGLQtContext::initScene()
{
	calcRotProgram_  = new Shader("calcRot.vert", "calcRot.geom", "calcRot.frag");
	//addRotProgram_ = new Shader("addRot.vert", "addRot.geom", "addRot.frag");
	applyRotProgram_ = new Shader("applyRot.vert", "applyRot.geom", "applyRot.frag");
	

	static const char* transFeedbVar[] =
	{
		"outAngle",
		"outAngleVelo",
		"outKiForce",
		"gl_Position"
	};
	glTransformFeedbackVaryings(calcRotProgram_->getID(), 4, transFeedbVar, GL_SEPARATE_ATTRIBS);	
	glLinkProgram (calcRotProgram_->getID());



	static const char* transFeedbVa[] =
	{
		"posRot"
	};
	glTransformFeedbackVaryings(applyRotProgram_->getID(), 1, transFeedbVa, GL_SEPARATE_ATTRIBS);	
	glLinkProgram (applyRotProgram_->getID());

	printProgramInfoLog(applyRotProgram_->getID());

	// Projektionsmatrix und Viewport setzen
	projectionMatrix_ = glm::perspective(60.0f, 600.0f / 600.0f, 1.0f, 100.f);
	glViewport(0, 0, 600.0f, 600.0f);

	// Viewmatrix setzen (= Kamera)
	viewMatrix_ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -3.0f, -14.f)); // Create our view matrix
	modelMatrix_ = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));  // Create our model matrix

	std::cout << viewMatrix_ << std::endl << modelMatrix_ << std::endl << projectionMatrix_ << std::endl;
	

	projectionMatrixLocationCalc_ = glGetUniformLocation(calcRotProgram_->getID(), "projectionMatrix");
	viewMatrixLocationCalc_ = glGetUniformLocation(calcRotProgram_->getID(), "viewMatrix");
	modelMatrixLocationCalc_ = glGetUniformLocation(calcRotProgram_->getID(), "modelMatrix");
	frameTimeLocationCalc_ = glGetUniformLocation(calcRotProgram_->getID(), "frameTime");
	frameCountLocationCalc_ = glGetUniformLocation(calcRotProgram_->getID(), "frameCount");

	projectionMatrixLocationApp_ = glGetUniformLocation(applyRotProgram_->getID(), "projectionMatrix");
	viewMatrixLocationApp_ = glGetUniformLocation(applyRotProgram_->getID(), "viewMatrix");
	modelMatrixLocationApp_ = glGetUniformLocation(applyRotProgram_->getID(), "modelMatrix");


	initVertexArrayObject();



	genTree();

	timer_.start();

	int a;
	glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS, &a);
	std::cout << "Anzahl TF-Comp: " << a << std::endl;
}
//


//
// paintGL
//
// virtuelle Funktion der Elternklasse wird hierdurch überschrieben
// Render-Funktion
//
void OpenGLQtContext::paintGL()
{	
	// buffer leeren
   	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


	calcRotProgram_->bind();
	glUniformMatrix4fv(projectionMatrixLocationCalc_, 1, GL_FALSE, &projectionMatrix_[0][0]); // Send our projection matrix to the shader[Shader::currentShader_]
	glUniformMatrix4fv(viewMatrixLocationCalc_, 1, GL_FALSE, &viewMatrix_[0][0]); // Send our view matrix to the shader[Shader::currentShader_]
	glUniformMatrix4fv(modelMatrixLocationCalc_, 1, GL_FALSE, &modelMatrix_[0][0]); // Send our model matrix to the shader[Shader::currentShader_]
	
	float time = timer_.elapsed();
	timer_.restart();
	glUniform1f(frameTimeLocationCalc_, time);
	glUniform1i(frameCountLocationCalc_, frameCount_);


	applyRotProgram_->bind();
	glUniformMatrix4fv(projectionMatrixLocationApp_, 1, GL_FALSE, &projectionMatrix_[0][0]); // Send our projection matrix to the shader[Shader::currentShader_]
	glUniformMatrix4fv(viewMatrixLocationApp_, 1, GL_FALSE, &viewMatrix_[0][0]); // Send our view matrix to the shader[Shader::currentShader_]
	glUniformMatrix4fv(modelMatrixLocationApp_, 1, GL_FALSE, &modelMatrix_[0][0]); // Send our model matrix to the shader[Shader::currentShader_]

	applyRotProgram_->unbind();


	int testi = 2;




	//
	//--------------------------------------------------------
	//Berechne Kräfte auf einzelne Zylinder
	//Verwende Ki vom letzten Durchlauf
	//--------------------------------------------------------
	//
	calcRotProgram_->bind();


	if (frameCount_ % 2 == 0)
	{
		glBindVertexArray(vaoID_[0]);

		//Angle
		//AngleVelo
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfID_[3]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, tfID_[3]);
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfID_[1]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfID_[1]);

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_BUFFER, tfTex_[0]);
		glBindBuffer(GL_TEXTURE_BUFFER, tfID_[0]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, tfID_[0]);

		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_BUFFER, tfTex_[2]);
		glBindBuffer(GL_TEXTURE_BUFFER, tfID_[2]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, tfID_[2]);


		//pos
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfID_[5]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 3, tfID_[5]);



		//Ki-Force out
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, kiBuf_[1]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, kiBuf_[1]);

		//Ki-Force in
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_BUFFER, kiTex_[0]);
		glBindBuffer(GL_TEXTURE_BUFFER, kiBuf_[0]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, kiBuf_[0]);

		testi = 0;
	}
	else
	{		
		glBindVertexArray(vaoID_[1]);

		//angle
		//angleVelo
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfID_[2]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, tfID_[2]);
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfID_[0]);	
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfID_[0]);

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_BUFFER, tfTex_[1]);
		glBindBuffer(GL_TEXTURE_BUFFER, tfID_[1]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, tfID_[1]);

		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_BUFFER, tfTex_[3]);
		glBindBuffer(GL_TEXTURE_BUFFER, tfID_[3]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, tfID_[3]);	


		//pos
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfID_[5]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 3, tfID_[5]);



		//Ki-Force out
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, kiBuf_[0]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, kiBuf_[0]);
		//	
		//Ki-Force in
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_BUFFER, kiTex_[1]);
		glBindBuffer(GL_TEXTURE_BUFFER, kiBuf_[1]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, kiBuf_[1]);

		testi = 1;
	}

	int texChildCount = glGetUniformLocation(calcRotProgram_->getID(), "childCountTex");
	glUniform1i(texChildCount, 1);
	int texChildIDs = glGetUniformLocation(calcRotProgram_->getID(), "childIDsTex");
	glUniform1i(texChildIDs, 2);
	int texChildPlace = glGetUniformLocation(calcRotProgram_->getID(), "childIDsPlaceTex");
	glUniform1i(texChildPlace, 3);
	int texKi = glGetUniformLocation(calcRotProgram_->getID(), "kiTex");
	glUniform1i(texKi, 4);
	int texThickness = glGetUniformLocation(calcRotProgram_->getID(), "thicknessTex");
	glUniform1i(texThickness, 5);
	int texAngle = glGetUniformLocation(calcRotProgram_->getID(), "angleTex");
	glUniform1i(texAngle, 6);
	int texAngleVelo = glGetUniformLocation(calcRotProgram_->getID(), "angleVeloTex");
	glUniform1i(texAngleVelo, 7);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, childCountTex_);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_BUFFER, childIDsTex_);	

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_BUFFER, childIDsPlaceTex_);	

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_BUFFER, thickTex_);	




	glBeginTransformFeedback(GL_LINES);
		glEnable(GL_RASTERIZER_DISCARD);
			
		
			glDrawElements(GL_LINES, 18, GL_UNSIGNED_INT, 0);
		
			
		glDisable(GL_RASTERIZER_DISCARD);
	glEndTransformFeedback();




	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_BUFFER, 0);
	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_BUFFER, 0);
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_BUFFER, 0);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_BUFFER, 0);
		
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	calcRotProgram_->unbind();
	//
	//----------------------------------------------------------------
	//

	//---------------------------------
	//Bufferinhalt ausgeben
	//
	glBindBuffer(GL_ARRAY_BUFFER, tfID_[5]);
	float *test = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

	for (int i = 0; i < 4*20; i++)
	{
		std::cout << test[i] << " ";

		if (i % 4 == 3)
			std::cout << std::endl;
	}
	std::cout << std::endl;

	glUnmapBuffer(GL_ARRAY_BUFFER);
	//-----------------------------------


	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_BUFFER, 0);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);


//glFinish();

	//
	//--------------------------------------------------------
	//Addiert Kräfte auf Zylinder und Kinder auf
	//
	//--------------------------------------------------------
	//
	applyRotProgram_->bind();

	if (frameCount_ % 2 == 0)
	{
		glBindVertexArray(vaoID_[0]);

		testi = 0;
	}
	else
	{
		glBindVertexArray(vaoID_[1]);

		testi = 1;
	}

	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfID_[4]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfID_[4]);



	int texParent = glGetUniformLocation(applyRotProgram_->getID(), "parent");
	glUniform1i(texParent, 0);
	int texPos = glGetUniformLocation(applyRotProgram_->getID(), "posTex");
	glUniform1i(texPos, 6);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, parentTex_);		

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_BUFFER, posTex_);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, tfID_[5]);




	glBeginTransformFeedback(GL_TRIANGLES);


			glDrawElements(GL_LINES, 18, GL_UNSIGNED_INT, 0);


	glEndTransformFeedback();




	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_BUFFER, 0);


	////---------------------------------
	////Bufferinhalt ausgeben
	////
	//glBindBuffer(GL_ARRAY_BUFFER, tfID_[4]);
	//float *test = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

	//for (int i = 30*4; i < 30*4*10; i++)
	//{
	//	std::cout << test[i] << " ";

	//	if (i % 4 == 3)
	//		std::cout << std::endl;
	//}
	//std::cout << std::endl;

	//glUnmapBuffer(GL_ARRAY_BUFFER);
	////-----------------------------------

	applyRotProgram_->unbind();
	//
	//----------------------------------------------------------------
	//

	++frameCount_;

	update();
}
//


//
// resizeGL
//
// virtuelle Funktion der Elternklasse wird hierdurch überschrieben
//
void OpenGLQtContext::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	resize(width,height);
	projectionMatrix_ = glm::perspective(60.0f, float(width) / float(height), 0.1f, 100.f);
}
//


//
// keyPressEvent
//
// wird aufgerufen, sobald eine Taste gedrückt wird
//
void OpenGLQtContext::keyPressEvent(QKeyEvent* event)
{
	int key = event->key();

	switch (key)
	{
	
		case Qt::Key_Left:	//std::cout << "pfeillinks" << std::endl;
					viewMatrix_ *= glm::translate (glm::mat4(1.0f), glm::vec3 (1.0f, 0.0f, 0.0f));
					break;
					
		case Qt::Key_Right:	//std::cout << "pfeilrechts" << std::endl;
					viewMatrix_ *= glm::translate (glm::mat4(1.0f), glm::vec3 (-1.0f, 0.0f, 0.0f));
					break;
					
		case Qt::Key_Up:	//std::cout << "pfeilup" << std::endl;
					viewMatrix_ = glm::translate (viewMatrix_, glm::vec3 (0.0f, 1.0f, 0.0f));
					break;
					
		case Qt::Key_Down:	//std::cout << "pfeildown" << std::endl;
					viewMatrix_ = glm::translate (viewMatrix_, glm::vec3 (0.0f, -1.0f, 0.0f));
					break;
					
		case Qt::Key_PageUp:	//std::cout << "pageup" << std::endl;
					viewMatrix_ = glm::translate (viewMatrix_, glm::vec3 (0.0f, 0.0f, 1.0f));
					break;
					
		case Qt::Key_PageDown:	//std::cout << "pagedown" << std::endl;
					viewMatrix_ = glm::translate (viewMatrix_, glm::vec3 (0.0f, 0.0f, -1.0f));
					break;
					
		case Qt::Key_Escape:	std::cout << "tschüss!" << std::endl;
					close();
					break;
		
		default:		std::cout << "key nicht belegt" << std::endl;
	}

	//wichtig, aufruf von update(), damit szene neu berechnet wird
	update();
		
}
//


//
// mousePressEvent
//
// wird aufgerufen, wenn eine Maustaste gedrückt wird
//
void OpenGLQtContext::mousePressEvent(QMouseEvent *event)
{
	lastPos_ = event->pos();
}
//


//
// mouseMoveEvent
//
// wird aufgerufen, wenn die Maus bewegt wird
//
void OpenGLQtContext::mouseMoveEvent(QMouseEvent *event)
{
	int dx = event->x() - lastPos_.x();
	int dy = event->y() - lastPos_.y();

	//std::cout << dx << " " << dy << std::endl;

	if (event->buttons() & Qt::LeftButton) 
	{
		viewMatrix_ = glm::rotate(viewMatrix_, float(dy)/2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		viewMatrix_ = glm::rotate(viewMatrix_, float(dx)/2.0f, glm::vec3(0.0f, 1.0f, 0.0f));		
	} 
	else if (event->buttons() & Qt::RightButton) 
	{
		viewMatrix_ = glm::rotate(viewMatrix_, float(dy)/2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		viewMatrix_ = glm::rotate(viewMatrix_, float(dx)/2.0f, glm::vec3(0.0f, 0.0f, 1.0f));

	}
	lastPos_ = event->pos();
	update();
}
//


//
// mouseReleaseEvent
//
// wird aufgerufen, wenn eine Maustaste losgelassen wird
//
void OpenGLQtContext::mouseReleaseEvent(QMouseEvent *event)
{
//	emit clicked();
}
//

//
// timeOut-Funktionen
//
// wird aufgerufen, wenn eine Maustaste losgelassen wird
//
void OpenGLQtContext::timeOut()
{
	static int ueberspringen = 0;
	
	ueberspringen = (ueberspringen+1)%2;
	
	if (ueberspringen == 1)
	{
		richtung_ = (richtung_+1)%2;
	}

}

void OpenGLQtContext::timeOut2()
{
	update();
}
//



//
// Überladener Operator zur Ausgabe von Matrizen
//
std::ostream& operator<<(std::ostream& stream, glm::mat4 const& mat)
{
	//for (unsigned int i = 0; i < 4; ++i )
	//{
	//	for (unsigned int j = 0; j < 4; ++j )
	//	{
	//		std::cout << mat[i][j] << " ";
	//	}
	//	std::cout << std::endl;
	//}

	//return stream;
	stream << "<glm_mat4>" << std::endl;

	stream << "<row ";
	stream << "x=\"" << glm::row(mat, 0)[0] << "\" ";
	stream << "y=\"" << glm::row(mat, 0)[1] << "\" ";
	stream << "z=\"" << glm::row(mat, 0)[2] << "\" ";
	stream << "w=\"" << glm::row(mat, 0)[3] << "\" ";
	stream << "/>" << std::endl;
	stream << "<row ";
	stream << "x=\"" << glm::row(mat, 1)[0] << "\" ";
	stream << "y=\"" << glm::row(mat, 1)[1] << "\" ";
	stream << "z=\"" << glm::row(mat, 1)[2] << "\" ";
	stream << "w=\"" << glm::row(mat, 1)[3] << "\" ";
	stream << "/>" << std::endl;
	stream << "<row ";
	stream << "x=\"" << glm::row(mat, 2)[0] << "\" ";
	stream << "y=\"" << glm::row(mat, 2)[1] << "\" ";
	stream << "z=\"" << glm::row(mat, 2)[2] << "\" ";
	stream << "w=\"" << glm::row(mat, 2)[3] << "\" ";
	stream << "/>" << std::endl;
	stream << "<row ";
	stream << "x=\"" << glm::row(mat, 3)[0] << "\" ";
	stream << "y=\"" << glm::row(mat, 3)[1] << "\" ";
	stream << "z=\"" << glm::row(mat, 3)[2] << "\" ";
	stream << "w=\"" << glm::row(mat, 3)[3] << "\" ";
	stream << "/>" << std::endl;
	stream << "</glm_mat4>";

	return stream;
}
//



//
//checkForError
//
bool	checkForError (std::string const& command)
{
	unsigned int error = 1;
	bool res = false;
	do
	{
		error = glGetError();

		switch(error)
		{
		case GL_INVALID_ENUM:
			std::cout << command << "(GL_INVALID_ENUM): " << std::endl;
			std::cout << "invalid value for enumerated argument" << std::endl;
			res = true;
			break;

		case GL_INVALID_VALUE:
			std::cout << command << "(GL_INVALID_VALUE): " << std::endl;
			std::cout << "numeric argument is out of range" << std::endl;
			res = true;
			break;

		case GL_INVALID_OPERATION:
			std::cout << command << "(GL_INVALID_OPERATION): " << std::endl;
			std::cout << "befehl im momentanen zustand von opengl nicht zulässig" << std::endl;
			res = true;
			break;
			
		case GL_STACK_OVERFLOW:
			std::cout << command << "(GL_STACK_OVERFLOW): " << std::endl;
			std::cout << "befehl würde zu stack overflow führen" << std::endl;
			res = true;
			break;

		case GL_STACK_UNDERFLOW:
			std::cout << command << "(GL_STACK_UNDERFLOW): " << std::endl;
			std::cout << "befehl würde zu stack underflow führen" << std::endl;
			res = true;
			break;

		case GL_OUT_OF_MEMORY:
			std::cout << command << "(GL_OUT_OF_MEMORY): " << std::endl;
			std::cout << "nicht genug speicher um befehl auszuführen, weiteres verhalten undefiniert" << std::endl;
			res = true;
			break;

		case GL_TABLE_TOO_LARGE:
			std::cout << command << "(GL_TABLE_TOO_LARGE): " << std::endl;
			std::cout << "table überschreitet maximal unterstützte table size" << std::endl;
			res = true;
			break;

		case GL_NO_ERROR:
			break;
		default:
			std::cout << command << "unbekannter fehler" << std::endl;
			res = true;
			break;
		}

	}while (error != GL_NO_ERROR);

	return res;
}
//


//
//initVertexArrayObject
//
bool OpenGLQtContext::initVertexArrayObject()
{

	const int size = 40;
	float vertexArray[size];
	float colorArray[size];
	float tfArray1[2*size];
	float tfArray2[2*size];

	vertexArray[0]	=  0.0f;
	vertexArray[1]	=  0.0f;
	vertexArray[2]	=  0.0f;
	vertexArray[3]	=  1.0f;

	vertexArray[4]	=  0.0f;
	vertexArray[5]	=  1.0f;
	vertexArray[6]	=  0.0f;
	vertexArray[7]	=  1.0f;

	vertexArray[8]	=  0.0f;
	vertexArray[9]	=  2.0f;
	vertexArray[10]	=  0.0f;
	vertexArray[11]	=  1.0f;

	vertexArray[12]	=  0.0f;
	vertexArray[13]	=  3.0f;
	vertexArray[14]	=  0.0f;
	vertexArray[15]	=  1.0f;

	vertexArray[16]	=  0.0f;
	vertexArray[17]	=  4.0f;
	vertexArray[18]	=  0.0f;
	vertexArray[19]	=  1.0f;

	vertexArray[20]	=  2.0f;
	vertexArray[21]	=  6.0f;
	vertexArray[22]	=  0.0f;
	vertexArray[23]	=  1.0f;

	vertexArray[24]	= -2.0f;
	vertexArray[25]	=  6.0f;
	vertexArray[26]	=  0.0f;
	vertexArray[27]	=  1.0f;

	vertexArray[28]	= -4.0f;
	vertexArray[29]	=  8.0f;
	vertexArray[30]	=  0.0f;
	vertexArray[31]	=  1.0f;

	vertexArray[32]	=  0.0f;
	vertexArray[33]	=  5.0f;
	vertexArray[34]	=  0.0f;
	vertexArray[35]	=  1.0f;

	vertexArray[36]	=  0.0f;
	vertexArray[37]	=  6.0f;
	vertexArray[38]	= -1.0f;
	vertexArray[39]	=  1.0f;

	colorArray[0]   = 1.0f;
	colorArray[1]   = 1.0;
	colorArray[2]   = 1.0f;
	colorArray[3]   = 1.0f;

	colorArray[4]   = 0.0f;
	colorArray[5]   = 1.0;
	colorArray[6]   = 1.0f;
	colorArray[7]   = 1.0f;

	colorArray[8]    = 1.0f;
	colorArray[9]    = 0.0;
	colorArray[10]   = 1.0f;
	colorArray[11]   = 1.0f;

	colorArray[12]   = 1.0f;
	colorArray[13]   = 1.0;
	colorArray[14]   = 0.0f;
	colorArray[15]   = 1.0f;

	colorArray[16]   = 0.0f;
	colorArray[17]   = 1.0;
	colorArray[18]   = 0.0f;
	colorArray[19]   = 1.0f;

	colorArray[20]   = 0.0f;
	colorArray[21]   = 0.0;
	colorArray[22]   = 1.0f;
	colorArray[23]   = 1.0f;

	colorArray[24]   = 1.0f;
	colorArray[25]   = 0.0;
	colorArray[26]   = 0.0f;
	colorArray[27]   = 1.0f;

	colorArray[28]   = 0.5f;
	colorArray[29]   = 0.5;
	colorArray[30]   = 0.5f;
	colorArray[31]   = 1.0f;

	colorArray[32]   = 1.0f;
	colorArray[33]   = 1.0;
	colorArray[34]   = 1.0f;
	colorArray[35]   = 1.0f;

	colorArray[36]   = 0.5f;
	colorArray[37]   = 1.0;
	colorArray[38]   = 0.5f;
	colorArray[39]   = 1.0f;


	for (int i = 0; i < 2*size; ++i)
	{
		tfArray1[i] = 0.0f;

		//
		//if (i % 4 == 0)
		//	tfArray1[i] = 10.0f;
		//else if (i % 4 == 2)
		//	tfArray1[i] = 0.0f;
	}
	for (int i = 0; i < 2*size; ++i)
	{
		tfArray2[i] = 0.0f;;

		//
		//if (i % 4 == 0)
		//	tfArray2[i] = 10.0f;
		//else if (i % 4 == 2)
		//	tfArray2[i] = 0.0f;
	}

	float radienArray[10];

	for(int i = 0; i < 10; ++i)
	{
		radienArray[i] = 1.0f;
	}

unsigned int indexArray[18];

	indexArray[0] = 0;
	indexArray[1] = 1;
	indexArray[2] = 1;
	indexArray[3] = 2;
	indexArray[4] = 2;
	indexArray[5] = 3;
	indexArray[6] = 3;
	indexArray[7] = 4;
	indexArray[8] = 4;
	indexArray[9] = 5;
	indexArray[10] = 4;
	indexArray[11] = 6;
	indexArray[12] = 6;
	indexArray[13] = 7;
	indexArray[14] = 4;
	indexArray[15] = 8;
	indexArray[16] = 8;
	indexArray[17] = 9;


	glGenVertexArrays(2, &vaoID_[0]);
	glGenBuffers(2, &vboID_[0]);
	glGenBuffers(6, &tfID_[0]);
	glGenBuffers(2, &iboID_[0]);
	glGenBuffers(1, &radBuf_);

		glBindBuffer(GL_ARRAY_BUFFER, tfID_[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tfArray1), tfArray1, GL_DYNAMIC_COPY);
		//glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, 0);
		//glEnableVertexAttribArray(3);

		glBindBuffer(GL_ARRAY_BUFFER, tfID_[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tfArray2), tfArray2, GL_DYNAMIC_COPY);
		//glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);
		//glEnableVertexAttribArray(4);

		glBindBuffer(GL_ARRAY_BUFFER, tfID_[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tfArray1), tfArray1, GL_DYNAMIC_COPY);
		//glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, 0);
		//glEnableVertexAttribArray(3);


		glBindBuffer(GL_ARRAY_BUFFER, tfID_[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tfArray2), tfArray2, GL_DYNAMIC_COPY);
		//glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);
		//glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(vaoID_[0]);
		glBindBuffer(GL_ARRAY_BUFFER, vboID_[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(0);


		glBindBuffer(GL_ARRAY_BUFFER, vboID_[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(colorArray), colorArray, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);





		glBindBuffer(GL_ARRAY_BUFFER, radBuf_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(radienArray), radienArray, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID_[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexArray), indexArray, GL_STATIC_DRAW);

	glBindVertexArray(0);



	glBindVertexArray(vaoID_[1]);
		glBindBuffer(GL_ARRAY_BUFFER, vboID_[0]);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(0);


		glBindBuffer(GL_ARRAY_BUFFER, vboID_[1]);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);






		glBindBuffer(GL_ARRAY_BUFFER, radBuf_);

		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID_[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexArray), indexArray, GL_STATIC_DRAW);

	glBindVertexArray(0);


	//
	//Buffer um Output in Buffer/Texturen zu ueberpruefen
	//
	glBindBuffer(GL_ARRAY_BUFFER, tfID_[5]);
	glBufferData(GL_ARRAY_BUFFER, 2*sizeof(vertexArray), NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindBuffer(GL_ARRAY_BUFFER, tfID_[4]);
	glBufferData(GL_ARRAY_BUFFER, 32*sizeof(vertexArray), NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//

	return checkForError("initVertexArrayObject");
}
//



//
// genTree
//
void OpenGLQtContext::genTree()
{
	glGenTextures(1, &parentTex_);
	glGenTextures(1, &childCountTex_);
	glGenTextures(1, &childIDsTex_);
	glGenTextures(1, &forcesTex_);
	glGenTextures(1, &childIDsPlaceTex_);
	glGenTextures(1, &kiTex_[0]);
	glGenTextures(1, &kiTex_[1]);
	glGenTextures(1, &posTex_);
	glGenTextures(1, &thickTex_);
	glGenTextures(4, &tfTex_[0]);

	glGenBuffers(1, &parentBuf_);
	glGenBuffers(1, &childCountBuf_);
	glGenBuffers(1, &childIDsBuf_);
	glGenBuffers(1, &forcesBuf_);
	glGenBuffers(1, &childIDsPlaceBuf_);
	glGenBuffers(1, &kiBuf_[0]);
	glGenBuffers(1, &kiBuf_[1]);
	glGenBuffers(1, &thickBuf_);

//
// Punkte
//
	std::map<int, Points> zylinderArray;
	
	zylinderArray.insert (std::pair<int, Points> (0, Points(glm::vec4(2.0f, 0.0f, 0.0f, 1.0f), glm::vec4(2.0f, 1.0f, 0.0f, 1.0f))));
	zylinderArray.insert (std::pair<int, Points> (1, Points(glm::vec4(2.0f, 1.0f, 0.0f, 1.0f), glm::vec4(2.0f, 2.0f, 0.0f, 1.0f))));
	zylinderArray.insert (std::pair<int, Points> (2, Points(glm::vec4(2.0f, 2.0f, 0.0f, 1.0f), glm::vec4(2.0f, 3.0f, 0.0f, 1.0f))));
	zylinderArray.insert (std::pair<int, Points> (3, Points(glm::vec4(2.0f, 3.0f, 0.0f, 1.0f), glm::vec4(2.0f, 4.0f, 0.0f, 1.0f))));
	zylinderArray.insert (std::pair<int, Points> (4, Points(glm::vec4(2.0f, 4.0f, 0.0f, 1.0f), glm::vec4(3.0f, 5.0f, 0.0f, 1.0f))));
	zylinderArray.insert (std::pair<int, Points> (5, Points(glm::vec4(2.0f, 4.0f, 0.0f, 1.0f), glm::vec4(2.0f, 5.0f, 0.0f, 1.0f))));
	zylinderArray.insert (std::pair<int, Points> (6, Points(glm::vec4(2.0f, 5.0f, 0.0f, 1.0f), glm::vec4(2.0f, 6.0f, 0.0f, 1.0f))));
	zylinderArray.insert (std::pair<int, Points> (7, Points(glm::vec4(2.0f, 4.0f, 0.0f, 1.0f), glm::vec4(1.0f, 5.0f, 0.0f, 1.0f))));
	zylinderArray.insert (std::pair<int, Points> (8, Points(glm::vec4(1.0f, 5.0f, 0.0f, 1.0f), glm::vec4(0.0f, 6.0f, 0.0f, 1.0f))));
	
			//	for (unsigned int i = 0; i < 500; ++i)
			//	{
			//		zylinderArray.insert (std::pair<int, Points> (i+9, Points(glm::vec4(1.0f, 5.0f, 0.0f, 1.0f), glm::vec4(0.0f, 6.0f, 0.0f, 1.0f))));
			//		
			//	}

	std::cout << "Inhalt ZylinderArray" << std::endl;
	for (std::map<int, Points>::iterator i = zylinderArray.begin(); i != zylinderArray.end(); ++i)
	{
		std::cout << (*i).second;
	}
//	



//
// Parent
//
	std::map<int, int> parentArray;
	int parentArrayBuf[9];
	
	for (std::map<int,Points>::reverse_iterator i = zylinderArray.rbegin(); i != zylinderArray.rend(); ++i)
	{
		for (std::map<int, Points>::reverse_iterator j = zylinderArray.rbegin(); j != zylinderArray.rend(); ++j)
		{
			if (i->second.startPoint == j->second.endPoint)
			{
				parentArray[i->first] = j->first;
		//		std::cout << i->first << " " << j->first << std::endl;
				break; 
			}
		}
	}
	for (std::map<int, int>::iterator i = parentArray.begin(); i != parentArray.end(); ++i)
	{
		parentArrayBuf[i->first] = i->second;
	}
	parentArrayBuf[0] = -1;

	for(int i = 0; i < 9; ++i)
	{
		std::cout << parentArrayBuf[i] << " ";
	}
	std::cout << std::endl;

	std::cout << std::endl << "Inhalt ParentArray:" << std::endl;
	

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, parentTex_); // Bind the texture fbo_texture 

	glBindBuffer(GL_TEXTURE_BUFFER, parentBuf_);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(parentArrayBuf), parentArrayBuf, GL_DYNAMIC_COPY);

	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, parentBuf_);

	//std::cout << parentArrayBuf[0] << " ";
	for (std::map<int, int>::iterator i = parentArray.begin(); i != parentArray.end(); ++i)
	{
		std::cout << i-> first << " | ";
	}
	std::cout << std::endl << "------------------------------------" << std::endl;
	for (std::map<int, int>::iterator i = parentArray.begin(); i != parentArray.end(); ++i)
	{
		std::cout << i->second << " | ";
	}	
	std::cout << std::endl;
//



//
// childCount
//
	std::map<int, int> childArrayCount;
	int childArrayCountBuf[9];
	
	for (unsigned int i = 0; i < zylinderArray.size(); ++i)
	{
		childArrayCount.insert (std::pair<int, int> (i, 0));
	}
	
	for (std::map<int, int>::iterator i = parentArray.begin(); i != parentArray.end(); ++i)
	{	
		++childArrayCount[i->second];
	}

	for (std::map<int, int>::iterator i = childArrayCount.begin(); i != childArrayCount.end(); ++i)
	{
		childArrayCountBuf[i->first] = i->second;
	}


	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, childCountTex_); // Bind the texture fbo_texture 

	glBindBuffer(GL_TEXTURE_BUFFER, childCountBuf_);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(childArrayCountBuf), childArrayCountBuf, GL_DYNAMIC_COPY);

	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, childCountBuf_);


	std::cout << std::endl << "Inhalt ChildArrayCount: " << std::endl;
	
	for (std::map<int, int>::iterator i = childArrayCount.begin(); i != childArrayCount.end(); ++i)
	{
		std::cout << i-> first << " | ";
	}
	std::cout << std::endl << "------------------------------------" << std::endl;
	for (std::map<int, int>::iterator i = childArrayCount.begin(); i != childArrayCount.end(); ++i)
	{
		std::cout << i->second << " | ";
	}
	std::cout << std::endl;
//	



//
// childIDsPlace
//
	int childArrayIDsPlaceBuf[9] = {0, 1, 2, 3, -1, 6, -1, 7, -1};

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_BUFFER, childIDsPlaceTex_);

	glBindBuffer(GL_TEXTURE_BUFFER, childIDsPlaceBuf_);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(childArrayIDsPlaceBuf), childArrayIDsPlaceBuf, GL_DYNAMIC_COPY);

	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, childIDsPlaceBuf_);

	for(int i = 0; i < 9; ++i)
	{
		std::cout << childArrayIDsPlaceBuf[i] << " ";
	}
//



//
// childIDs
//
	std::map<int, int> childArrayIDs;
	int childArrayIDsBuf[8];
	int count = 0;
	
	for (std::map<int, Points>::iterator i = zylinderArray.begin(); i != zylinderArray.end(); ++i)
	{
		for (std::map<int, int>::iterator j = parentArray.begin(); j != parentArray.end(); ++j)
		{
			if (i->first == j->second)
			{
				childArrayIDs[count] = j->first;
				++count;
				//std::cout << count << " ";
			}
		}		
	}

	for (std::map<int, int>::iterator i = childArrayIDs.begin(); i != childArrayIDs.end(); ++i)
	{
		childArrayIDsBuf[i->first] = i->second;
	}

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_BUFFER, childIDsTex_); // Bind the texture fbo_texture 

	glBindBuffer(GL_TEXTURE_BUFFER, childIDsBuf_);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(childArrayIDsBuf), childArrayIDsBuf, GL_DYNAMIC_COPY);

	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, childIDsBuf_);


	std::cout << std::endl << "Inhalt ChildArrayIDs:" << std::endl;
	for (std::map<int, int>::iterator i = childArrayIDs.begin(); i != childArrayIDs.end(); ++i)
	{
		std::cout << i-> first << " | ";
	}
	std::cout << std::endl << "------------------------------------" << std::endl;
	for (std::map<int, int>::iterator i = childArrayIDs.begin(); i != childArrayIDs.end(); ++i)
	{
		std::cout << i->second << " | ";
	}
	std::cout << std::endl;
//




//
// ki_1
//
	float kiArrayBuf[80] = {1.0};
	//std::cout << "kisize" << sizeof(kiArrayBuf) << std::endl;

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_BUFFER, kiTex_[0]); // Bind the texture fbo_texture 

	glBindBuffer(GL_TEXTURE_BUFFER, kiBuf_[0]);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(kiArrayBuf), kiArrayBuf, GL_DYNAMIC_COPY);

	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, kiBuf_[0]);
//



//
// ki_2
//
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_BUFFER, kiTex_[1]);

	glBindBuffer(GL_TEXTURE_BUFFER, kiBuf_[1]);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(kiArrayBuf), kiArrayBuf, GL_DYNAMIC_COPY);

	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, kiBuf_[1]);
//



//
// thickness
//

	float thickArray[10];

	for(int i = 0; i < 10; ++i)
	{
		thickArray[i] = 1.0f;
	}

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_BUFFER, thickTex_);

	glBindBuffer(GL_TEXTURE_BUFFER, thickBuf_);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(thickArray), thickArray, GL_STATIC_DRAW);

	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, thickBuf_);
//



//
// position (neue, nach drehung(
//
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_BUFFER, posTex_);
	glBindBuffer(GL_TEXTURE_BUFFER, tfID_[5]);

	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, tfID_[5]);
//



//
// angleTex
//




}
//