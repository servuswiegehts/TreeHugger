#include "openGLQtContext.h"


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

		shader_			(NUMBER_OF_SHADERS, (Shader*)0),

		windowWidth_	(600),
		windowHeight_	(600),

		fbo_			(0),
		fboTex_			(0)
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
#ifdef _DEBUG
	checkForError("initializeGLbeg");
#endif
	GLenum glewInitResult = glewInit();

	if(GLEW_OK != glewInitResult)
	{
		std::cout << "ERROR: "<<glewGetErrorString(glewInitResult) << std::endl;
		exit(EXIT_FAILURE);
	}
#ifdef _DEBUG
	checkForError("glew");
#endif
	std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	
	glClearDepth(1.0f);
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
#ifdef _DEBUG
	checkForError("initializeGLmiddle");
#endif
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );

	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST );
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST );	  
	glHint(GL_FOG_HINT, GL_NICEST);

	glEnable( GL_DEPTH_TEST );
	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	initScene();
#ifdef _DEBUG
	checkForError("initializeGLende");
#endif	
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
#ifdef _DEBUG
	checkForError("initscenebeg");
#endif		
	// Shader initialisieren, entweder über die init-Funktion oder direkt über den Konstruktor
	//shader_[0] = new Shader(); 
	//shader_[0]->init("testshader.vert", "testshader.frag");

	shader_[1] = new Shader("testshader2.vert", "testshader2.frag");

	// Projektionsmatrix und Viewport setzen
	projectionMatrix_ = glm::perspective(60.0f, 600.0f / 600.0f, 0.1f, 100.f);
	glViewport(0, 0, 600.0f, 600.0f);

	// Viewmatrix setzen (= Kamera)
	viewMatrix_ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -3.0f, -14.f)); // Create our view matrix
	modelMatrix_ = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 2.0f, 0.5f));  // Create our model matrix
	modelMatrix2_ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, +10.0f));  // Create our model matrix
	
	//createCyl(0);
	//createQuad();
	//createTree();
	//createTree2();
#ifdef _DEBUG
	checkForError("initscenemiddle");
#endif	
	//initVertexbufferObjects();
	initVertexArrayObject();
	//initTransformFeedbackObject();

	//initFramebuffer();
	initTexture();

	timer_.start();

	Shader::currentShader_++;
	//std::cout << Shader::currentShader_ << std::endl;
	int a;
	glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS, &a);
	std::cout << "Anzahl TF-Comp: " << a << std::endl;

#ifdef _DEBUG
	checkForError("initEnde");
#endif
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
	int testi = 2;
#ifdef _DEBUG
	checkForError("paintGLbeg");
#endif

	if (frameCount_ % 2 == 0)
	{
		glBindVertexArray(vaoID_[0]);
#ifdef _DEBUG
	checkForError("binAr1");
#endif
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfID_[3]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, tfID_[3]);
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfID_[1]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfID_[1]);

//		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfoID_[0]);
#ifdef _DEBUG
	checkForError("binTF1");
#endif
		testi = 0;
	}
	else
	{
		glBindVertexArray(vaoID_[1]);
#ifdef _DEBUG
	checkForError("binAr2");
#endif
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfID_[2]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, tfID_[2]);
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfID_[0]);	
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfID_[0]);



//		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfoID_[1]);
#ifdef _DEBUG
	checkForError("binTF2");
#endif
		testi = 1;
	}



	// buffer leeren
   	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	shader_[Shader::currentShader_]->bind();
#ifdef _DEBUG
	checkForError("gl");
#endif

#ifdef _DEBUG
	checkForError("glBTfFb");
#endif
	// Holen der Position von zu setzenden Variablen aus dem aktuellen Shader 
	int projectionMatrixLocation = glGetUniformLocation(shader_[Shader::currentShader_]->getID(), "projectionMatrix");
	int viewMatrixLocation = glGetUniformLocation(shader_[Shader::currentShader_]->getID(), "viewMatrix");
	int modelMatrixLocation = glGetUniformLocation(shader_[Shader::currentShader_]->getID(), "modelMatrix");

	int frameTimeLocation = glGetUniformLocation(shader_[Shader::currentShader_]->getID(), "frameTime");

	int frameCountLoc = glGetUniformLocation(shader_[Shader::currentShader_]->getID(), "frameCount");

	int texLoc = glGetUniformLocation(shader_[Shader::currentShader_]->getID(), "tex");
	glUniform1i(texLoc, 0);

	glUniform1i(frameCountLoc, frameCount_);

//	float time = timer_.elapsed();
	//std::cout << time << " ";
	//glUniform1f(frameTimeLocation, time/1000.0f);
	glUniform1f(frameTimeLocation, frameCount_);
//	timer_.restart();


	// Setzen der Werte im Shader
	// GL_FALSE: in dem Fall, die Werte werden nicht normalisiert
	// Für mehr informationen zu den einzelnen Funktionen: http://wiki.delphigl.com/index.php/
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix_[0][0]); // Send our projection matrix to the shader[Shader::currentShader_]
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix_[0][0]); // Send our view matrix to the shader[Shader::currentShader_]
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix_[0][0]); // Send our model matrix to the shader[Shader::currentShader_]


	//std::cout << glGetAttribLocation(shader_[Shader::currentShader_+1]->getID(), "inTexCoord") << " " << glGetAttribLocation(shader_[Shader::currentShader_+1]->getID(), "texCoord") << std::endl;


//	std::cout << frameCount_ << std::endl;
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID_[0]);
#ifdef _DEBUG
	checkForError("binEle");
#endif



	//{
	//float *test = (float*)glMapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, GL_READ_WRITE);
	//#ifdef _DEBUG
	//	checkForError("map");
	//#endif
	//	int testt = -1;
	//	glGetIntegeri_v(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, 0, &testt);
	//	std::cout << testi << ":1" << std::endl;
	//	std::cout << testt << tfID_[0] << tfID_[1] <<  std::endl;
	//for (int i = 0; i < 40; i++)
	//{
	//	std::cout << test[i] << " ";
	//	if (i % 4 == 3)
	//		std::cout << std::endl;
	//}
	//	std::cout << std::endl;

	//glUnmapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);
	//#ifdef _DEBUG
	//	checkForError("unmap");
	//#endif
	//}




	//glEnable(GL_RASTERIZER_DISCARD);
		glBeginTransformFeedback(GL_LINES);
	#ifdef _DEBUG
		checkForError("begTF");
	#endif

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_BUFFER, texID_[0]);


		glDrawElements(GL_LINES, 18, GL_UNSIGNED_INT, 0);


		glBindTexture(GL_TEXTURE_BUFFER, 0);

	//	std::cout << "\nDRAW\n";
	#ifdef _DEBUG
		checkForError("draw");
	#endif
		glEndTransformFeedback();
	#ifdef _DEBUG
		checkForError("endTF");
	#endif


glFinish();


	//{	
	//float *test = (float*)glMapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, GL_READ_WRITE);
	//#ifdef _DEBUG
	//	checkForError("map2");
	//#endif
	//	int testt = -1;
	//	glGetIntegeri_v(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, 0, &testt);
	//	std::cout << testi << ":2" << std::endl;
	//	std::cout << testt << tfID_[0] << tfID_[1] <<  std::endl;
	//for (int i = 0; i < 40; i++)
	//{
	//	std::cout << test[i] << " ";
	//	if (i % 4 == 3)
	//		std::cout << std::endl;
	//}
	//	std::cout << std::endl;
	//glUnmapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);
	//#ifdef _DEBUG
	//	checkForError("unmap2");
	//#endif
	//}

		//glDisable(GL_RASTERIZER_DISCARD);

	//
	//glDrawElements(GL_LINES, 18, GL_UNSIGNED_INT, 0);
	//swapbuffer nicht nötig, wird automatisch aufgerufen
	//Vorsicht, wird nur neu gezeichnet wenn sich etwas verändert
	//erzwungene neuberechnung und neuzeichnen durch aufruf von update()
	shader_[Shader::currentShader_]->unbind();

	++frameCount_;
	//std::cout << glGetError() << std::endl;
	update();

#ifdef _DEBUG
	checkForError("paintGLend");
#endif
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
	if (!hasFocus())
	{
		std::cout << "keinen Focus!" << std::endl;
	}
	
	if (!keyboardGrabber())
	{
		std::cout << "keinen Tastaturzugriff!" << std::endl;
	}

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
// createCyl
//
// Erstellt Zylinder inkl. VAO und IBO
//
void OpenGLQtContext::createCyl(int count) 
{
	const int numberVerticesInCircle = 50;
	const int numberVerticesInHeight = 16;
	const int numberVertices = numberVerticesInCircle * numberVerticesInHeight;

//	float* vertexArray	= new float[4*numberVertices];
//	float* normalArray	= new float[numberVertices];
//	float* colorArray	= new float[4*numberVertices];
//	float* hierarchyArray  	= new float[numberVertices];

	float vertexArray[50*16*4];
	float normalArray[50*16];
	float colorArray[50*16*4];
	float hierarchyArray[50*16];

	float r1 = 1.0f;
	float r2 = 0.5f;

	float alpha = r2/r1;

	for (int i = 0; i < 16; i++)
	{
		float y = float(i)/3.0f;

		for (int j = 0; j < 50; ++j)
		{
		// Fill up the struct
		float theta = (float)(2 * 3.1415 * j) / 49;

		float a = 0.2f;
		float t = 2.f;
		float c = -a * y + t;

		vertexArray[4*(i*50+j)]	  	= c * std::sin(theta);
		vertexArray[4*(i*50+j)+1]	= y;
		vertexArray[4*(i*50+j)+2]	= c * std::cos(theta);
		vertexArray[4*(i*50+j)+3]	= 1.0f;


		//normalArray[i]	= glm::normalize (glm::vec4(0,y,0,1.0f) - vertexArray[i] );

		colorArray[4*(i*50+j)]	 = std::sin(theta);
		colorArray[4*(i*50+j)+1] = y;
		colorArray[4*(i*50+j)+2] = std::cos(theta);
		colorArray[4*(i*50+j)+3] = 1.0f;
		//hierarchyArray[i] = glm::vec4 ( y, 0, 0, 0);
		}
	}

	colorArray[0]   = 0.0f;
	colorArray[1]   = 1.0;
	colorArray[2]   = 0.0f;
	colorArray[3]   = 1.0f;


	GLenum ErrorCheckValue = glGetError();
	const size_t VaBufferSize = sizeof(vertexArray);
	const size_t CaBufferSize = sizeof(colorArray);
	const size_t NoBufferSize = sizeof(normalArray);
	const size_t HiBufferSize = sizeof(hierarchyArray);
	const size_t VertexSize = sizeof(vertexArray[0]);

	glGenVertexArrays(1, &vaoID_[0]); // Create our Vertex Array Object

	glBindVertexArray(vaoID_[0]); // Bind our Vertex Array Object so we can use it
	
	glGenBuffers(2, &vboID_[0]); // Generate our Vertex Buffer Object
	
	glBindBuffer(GL_ARRAY_BUFFER, vboID_[0]); // Bind our Vertex Buffer Object
	glBufferData(GL_ARRAY_BUFFER, VaBufferSize, vertexArray, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW

	glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
	glEnableVertexAttribArray(0); // Enable the second vertex attribute array 



	glBindBuffer(GL_ARRAY_BUFFER, vboID_[1]); // Bind our Vertex Buffer Object
	glBufferData(GL_ARRAY_BUFFER, CaBufferSize, colorArray, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW

	glVertexAttribPointer((GLuint)1, 4, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer

	glEnableVertexAttribArray(1); // Disable our Vertex Array Object
 
	//unsigned int* indexArray = new GLuint [50*15*2];
	unsigned int indexArray[50*16*2];

	for (int i = 0; i < 16; i+=1)
	{
		for (int j = 0; j < 50; ++j)
		{
			indexArray[2*i*50 + 2*j]   = i*50+j;
			indexArray[2*i*50 + 2*j+1] = (i+1)*50+j;
		}
	}
	//0, 50, 1, 51, 2, 52, 3, 53,..., 49, 99, 0, 50
	//50, 100, 51


	glGenBuffers(1, &iboID_[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID_[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexArray), indexArray, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	glBindVertexArray(0); // Disable our Vertex Buffer Object

	//delete [] vertexArray;
	//delete [] normalArray;
	//delete [] colorArray;
	//delete [] hierarchyArray;
	//delete [] indexArray;

}
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
// Überladener Operator zur Ausgabe von Matrizen
//
std::ostream& operator<<(std::ostream& stream, glm::mat4 const& mat)
{
	for (unsigned int i = 0; i < 4; ++i )
	{
		for (unsigned int j = 0; j < 4; ++j )
		{
			std::cout << mat[i][j] << " ";
		}
		std::cout << std::endl;
	}

	return stream;
}
//



//void OpenGLQtContext::initFrameBufferDepthBuffer(void) {  
//  
//glGenRenderbuffers(1, &fboDepth_); // Generate one render buffer and store the ID in fbo_depth  
//glBindRenderbuffer(GL_RENDERBUFFER, fbo_depth); // Bind the fbo_depth render buffer  
//  
//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height); // Set the render buffer storage to be a depth component, with a width and height of the window  
//  
//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo_depth); // Set the render buffer of this buffer to the depth buffer  
//  
//glBindRenderbuffer(GL_RENDERBUFFER, 0); // Unbind the render buffer  
//}  
  

void OpenGLQtContext::initFramebufferTexture(void) {  
glGenTextures(1, &fboTex_); // Generate one texture  
glBindTexture(GL_TEXTURE_2D, fboTex_); // Bind the texture fbo_texture  
  
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); // Create a standard texture with the width and height of our window  
  
// Setup the basic texture parameters  
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  
  
// Unbind the texture  
glBindTexture(GL_TEXTURE_2D, 0);  
}  
  
void OpenGLQtContext::initFramebuffer(void) {  
//initFrameBufferDepthBuffer(); // Initialize our frame buffer depth buffer  
  
initFramebufferTexture(); // Initialize our frame buffer texture  
  
glGenFramebuffers(1, &fbo_); // Generate one frame buffer and store the ID in fbo  
glBindFramebuffer(GL_FRAMEBUFFER, fbo_); // Bind our frame buffer  
  
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTex_, 0); // Attach the texture fbo_texture to the color buffer in our frame buffer  
  
//glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fbo_depth); // Attach the depth buffer fbo_depth to our frame buffer  
  
GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); // Check that status of our generated frame buffer  
  
if (status != GL_FRAMEBUFFER_COMPLETE) // If the frame buffer does not report back as complete  
{  
std::cout << "Couldn't create frame buffer" << std::endl; // Output an error to the console  
exit(0); // Exit the application  
}  
  
glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind our frame buffer  
} 


void OpenGLQtContext::createQuad()
{


	float vertexArray[16];
	float colorArray[16];
	float texCoordArray[8];



		vertexArray[0]	=  1.0f;
		vertexArray[1]	= -1.0f;
		vertexArray[2]	=  0.0f;
		vertexArray[3]	=  1.0f;

		vertexArray[4]	= -1.0f;
		vertexArray[5]	= -1.0f;
		vertexArray[6]	=  0.0f;
		vertexArray[7]	=  1.0f;

		vertexArray[8]	=  1.0f;
		vertexArray[9]	=  1.0f;
		vertexArray[10]	=  0.0f;
		vertexArray[11]	=  1.0f;

		vertexArray[12]	= -1.0f;
		vertexArray[13]	=  1.0f;
		vertexArray[14]	=  0.0f;
		vertexArray[15]	=  1.0f;


		texCoordArray[0] =  1.0f;
		texCoordArray[1] =  0.0f;
		
		texCoordArray[2] =  0.0f;
		texCoordArray[3] =  0.0f;

		texCoordArray[4] =  1.0f;
		texCoordArray[5] =  1.0f;

		texCoordArray[6] =  0.0f;
		texCoordArray[7] =  1.0f;

	colorArray[0]   = 1.0f;
	colorArray[1]   = 1.0;
	colorArray[2]   = 1.0f;
	colorArray[3]   = 1.0f;

	colorArray[4]   = 1.0f;
	colorArray[5]   = 1.0;
	colorArray[6]   = 1.0f;
	colorArray[7]   = 1.0f;

	colorArray[8]   = 1.0f;
	colorArray[9]   = 1.0;
	colorArray[10]   = 1.0f;
	colorArray[11]   = 1.0f;

	colorArray[12]   = 1.0f;
	colorArray[13]   = 1.0;
	colorArray[14]   = 1.0f;
	colorArray[15]   = 1.0f;

	GLenum ErrorCheckValue = glGetError();


	glGenVertexArrays(1, &vaoID_[1]); // Create our Vertex Array Object

	glBindVertexArray(vaoID_[1]); // Bind our Vertex Array Object so we can use it
	
	glGenBuffers(3, &vboID_[2]); // Generate our Vertex Buffer Object
	
	glBindBuffer(GL_ARRAY_BUFFER, vboID_[2]); // Bind our Vertex Buffer Object
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW

	glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
	glEnableVertexAttribArray(0); // Enable the second vertex attribute array 



	glBindBuffer(GL_ARRAY_BUFFER, vboID_[3]); // Bind our Vertex Buffer Object
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorArray), colorArray, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW

	glVertexAttribPointer((GLuint)1, 4, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer

	glEnableVertexAttribArray(1); // Disable our Vertex Array Object


	glBindBuffer(GL_ARRAY_BUFFER, vboID_[4]); // Bind our Vertex Buffer Object
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoordArray), texCoordArray, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW

	glVertexAttribPointer((GLuint)3, 2, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer

	glEnableVertexAttribArray(3); // Disable our Vertex Array Object
 
 
	//unsigned int* indexArray = new GLuint [50*15*2];
	unsigned int indexArray[4];


	indexArray[0] = 0;
	indexArray[1] = 1;
	indexArray[2] = 2;
	indexArray[3] = 3;


	glGenBuffers(1, &iboID_[1]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID_[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexArray), indexArray, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	glBindVertexArray(0); // Disable our Vertex Buffer Object

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
//createTree
//
void	OpenGLQtContext::createTree()
{

	const int size = 40;
	float vertexArray[size];
	float colorArray[size];



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

	vertexArray[20]	=  1.0f;
	vertexArray[21]	=  5.0f;
	vertexArray[22]	=  0.0f;
	vertexArray[23]	=  1.0f;

	vertexArray[24]	= -1.0f;
	vertexArray[25]	=  5.0f;
	vertexArray[26]	=  0.0f;
	vertexArray[27]	=  1.0f;

	vertexArray[28]	= -2.0f;
	vertexArray[29]	=  6.0f;
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


	GLenum ErrorCheckValue = glGetError();

	const unsigned int vertexArraySize = sizeof(vertexArray);

	glGenVertexArrays(1, &vaoID_[2]); // Create our Vertex Array Object

	glBindVertexArray(vaoID_[2]); // Bind our Vertex Array Object so we can use it
	
	glGenBuffers(2, &vboID_[5]); // Generate our Vertex Buffer Object
	
	glBindBuffer(GL_ARRAY_BUFFER, vboID_[5]); // Bind our Vertex Buffer Object
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW

	glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer

	glEnableVertexAttribArray(0); // Enable the second vertex attribute array 



	glBindBuffer(GL_ARRAY_BUFFER, vboID_[6]); // Bind our Vertex Buffer Object
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorArray), colorArray, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW

	glVertexAttribPointer((GLuint)1, 4, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer

	glEnableVertexAttribArray(1); // Disable our Vertex Array Object


	glBindVertexArray(0); // Disable our Vertex Buffer Object


	//unsigned int* indexArray = new GLuint [50*15*2];
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

	glGenBuffers(1, &iboID_[2]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID_[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexArray), indexArray, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);





	float tfArray[2*vertexArraySize];

	for (int i = 0; i < vertexArraySize*2; ++i)
	{
		tfArray[i] = 1;

		if(i % 4 == 1)
		tfArray[i] = 5;
	}


 	glGenBuffers(2, &tfID_[0]);
#ifdef _DEBUG
	checkForError("glGenBuffers1");
#endif

	glBindBuffer(GL_ARRAY_BUFFER, tfID_[0]); // Bind our Vertex Buffer Object
#ifdef _DEBUG
	checkForError("glBindBuffer1");
#endif
	
	glBufferData(GL_ARRAY_BUFFER, 2*sizeof(vertexArray), tfArray, GL_DYNAMIC_COPY); // Set the size and data of our VBO and set it to STATIC_DRAW
#ifdef _DEBUG
	checkForError("glBufferData1");
#endif

	glVertexAttribPointer((GLuint)5, 4, GL_FLOAT, GL_FALSE, 0,0); // Set up our vertex attributes pointer
#ifdef _DEBUG
	checkForError("glVertexAttribPointer1");
#endif

	glEnableVertexAttribArray(5); // Disable our Vertex Array Object
#ifdef _DEBUG
	checkForError("glEnableVertexAttribArray1");
#endif

	glVertexAttribPointer((GLuint)6, 4, GL_FLOAT, GL_FALSE, 16, BUFFER_OFFSET(16)); // Set up our vertex attributes pointer
#ifdef _DEBUG
	checkForError("glVertexAttribPointer3");
#endif

	glEnableVertexAttribArray(6); // Enable the second vertex attribute array 
#ifdef _DEBUG
	checkForError("glEnableVertexAttribArray");
#endif



	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfID_[1]);
#ifdef _DEBUG
	checkForError("glBindBuffer3");
#endif

	glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, 2*sizeof(vertexArray), NULL, GL_DYNAMIC_COPY);
#ifdef _DEBUG
	checkForError("glBufferData3");
#endif

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfID_[1]);
#ifdef _DEBUG
	checkForError("glBindBufferBase3");
#endif
}


//
//createTree2
//
void	OpenGLQtContext::createTree2()
{
	
	const int size = 40;
	float vertexArray[size];
	float colorArray[size];
	float tfArray[2*size];

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

	vertexArray[20]	=  1.0f;
	vertexArray[21]	=  5.0f;
	vertexArray[22]	=  0.0f;
	vertexArray[23]	=  1.0f;

	vertexArray[24]	= -1.0f;
	vertexArray[25]	=  5.0f;
	vertexArray[26]	=  0.0f;
	vertexArray[27]	=  1.0f;

	vertexArray[28]	= -2.0f;
	vertexArray[29]	=  6.0f;
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
		tfArray[i] = 1;

		if(i % 4 == 1)
		tfArray[i] = 5;
	}

	glGenVertexArrays(2, &vaoID_[0]);

	glGenBuffers(2, &vboID_[0]);
	glGenBuffers(2, &tfID_[0]);

	glBindVertexArray(vaoID_[0]);
	
	glBindBuffer(GL_ARRAY_BUFFER, vboID_[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vboID_[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorArray), colorArray, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, tfID_[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tfArray), tfArray, GL_DYNAMIC_COPY);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 16, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 16, BUFFER_OFFSET(16));
	glEnableVertexAttribArray(4);


	glBindBuffer(GL_ARRAY_BUFFER, tfID_[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tfArray), tfArray, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfID_[0]);

	glBindVertexArray(0);



	glBindVertexArray(vaoID_[1]);

	glBindBuffer(GL_ARRAY_BUFFER, vboID_[0]);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vboID_[1]);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfID_[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfID_[0]);

	glBindBuffer(GL_ARRAY_BUFFER, tfID_[1]);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 16, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 16, BUFFER_OFFSET(16));
	glEnableVertexAttribArray(4);

	glBindVertexArray(0);




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

	glGenBuffers(1, &iboID_[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID_[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexArray), indexArray, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

bool OpenGLQtContext::initVertexArrayObject()
{

	const int size = 40;
	float vertexArray[size];
	float colorArray[size];
	float tfArray1[size];
	float tfArray2[size];

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

	vertexArray[20]	=  1.0f;
	vertexArray[21]	=  5.0f;
	vertexArray[22]	=  0.0f;
	vertexArray[23]	=  1.0f;

	vertexArray[24]	= -1.0f;
	vertexArray[25]	=  5.0f;
	vertexArray[26]	=  0.0f;
	vertexArray[27]	=  1.0f;

	vertexArray[28]	= -2.0f;
	vertexArray[29]	=  6.0f;
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


	for (int i = 0; i < size; ++i)
	{
		tfArray1[i] = 0.0f;;

		
		if (i % 4 == 0)
			tfArray1[i] = 10.0f;
		else if (i % 4 == 2)
			tfArray1[i] = -15.0f;
	}
	for (int i = 0; i < size; ++i)
	{
		tfArray2[i] = 0.0f;;

		
		if (i % 4 == 0)
			tfArray2[i] = 10.0f;
		else if (i % 4 == 2)
			tfArray2[i] = -15.0f;
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
	glGenBuffers(4, &tfID_[0]);
	glGenBuffers(2, &iboID_[0]);


	glBindVertexArray(vaoID_[0]);
		glBindBuffer(GL_ARRAY_BUFFER, vboID_[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(0);


		glBindBuffer(GL_ARRAY_BUFFER, vboID_[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(colorArray), colorArray, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);


		glBindBuffer(GL_ARRAY_BUFFER, tfID_[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tfArray1), tfArray1, GL_DYNAMIC_COPY);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(3);

		glBindBuffer(GL_ARRAY_BUFFER, tfID_[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tfArray2), tfArray2, GL_DYNAMIC_COPY);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(4);


//		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfID_[1]);
//		glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(tfArray), 0, GL_DYNAMIC_COPY);

//		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, tfID_[1]);


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


		glBindBuffer(GL_ARRAY_BUFFER, tfID_[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tfArray1), tfArray1, GL_DYNAMIC_COPY);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(3);


		glBindBuffer(GL_ARRAY_BUFFER, tfID_[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tfArray2), tfArray2, GL_DYNAMIC_COPY);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(4);


//		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfID_[0]);
//		glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(tfArray), 0, GL_DYNAMIC_COPY);

//		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfID_[0]);


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID_[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexArray), indexArray, GL_STATIC_DRAW);

	glBindVertexArray(0);


	return checkForError("initVertexArrayObject");
}


bool OpenGLQtContext::initTransformFeedbackObject()
{
	const int size = 40;
	float tfArray[2*size];
	for (int i = 0; i < 2*size; ++i)
	{
		tfArray[i] = 1;

		if(i % 4 == 1)
		tfArray[i] = 5;
	}

	glGenTransformFeedbacks(1, &tfoID_[0]);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfoID_[0]);
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfID_[1]);
		glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(tfArray), tfArray, GL_DYNAMIC_COPY);

		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, tfID_[1]);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);


	glGenTransformFeedbacks(1, &tfoID_[1]);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfoID_[1]);

		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfID_[0]);
		glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(tfArray), tfArray, GL_DYNAMIC_COPY);

		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfID_[0]);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);


	return checkForError("initTransformFeedbackObject");
}


bool OpenGLQtContext::initVertexbufferObjects()
{
	const int size = 40;
	float vertexArray[size];
	float colorArray[size];
	float tfArray[2*size];

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

	vertexArray[20]	=  1.0f;
	vertexArray[21]	=  5.0f;
	vertexArray[22]	=  0.0f;
	vertexArray[23]	=  1.0f;

	vertexArray[24]	= -1.0f;
	vertexArray[25]	=  5.0f;
	vertexArray[26]	=  0.0f;
	vertexArray[27]	=  1.0f;

	vertexArray[28]	= -2.0f;
	vertexArray[29]	=  6.0f;
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
		tfArray[i] = 1;

		if(i % 4 == 1)
		tfArray[i] = 5;
	}


	glGenBuffers(2, &vboID_[0]);
		glBindBuffer(GL_ARRAY_BUFFER, vboID_[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vboID_[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(colorArray), colorArray, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	/*glGenBuffers(2, &tfID_[0]);
		glBindBuffer(GL_ARRAY_BUFFER, tfID_[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tfArray), tfArray, GL_DYNAMIC_COPY);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, tfID_[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tfArray), tfArray, GL_DYNAMIC_COPY);
		glBindBuffer(GL_ARRAY_BUFFER, 0);*/


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

	glGenBuffers(1, &iboID_[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID_[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexArray), indexArray, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return checkForError("initVertexBufferObjects");
}


bool OpenGLQtContext::initTexture() {  
	glGenTextures(1, &texID_[0]); // Generate one texture 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, texID_[0]); // Bind the texture fbo_texture 

	float texArray[36];

	texArray[0] = 0;
	texArray[1] = 9;
	texArray[2] = 0;
	texArray[3] = 0;

	texArray[4] = 2;
	texArray[5] = 0;
	texArray[6] = 0;
	texArray[7] = 0;

	texArray[8] = 3;
	texArray[9] = 0;
	texArray[10] = 0;
	texArray[11] = 0;

	texArray[12] = 4;
	texArray[13] = 0;
	texArray[14] = 0;
	texArray[15] = 0;

	texArray[16] = 5;
	texArray[17] = 6;
	texArray[18] = 8;
	texArray[19] = 0;
	
	texArray[20] = 0;
	texArray[21] = 0;
	texArray[22] = 0;
	texArray[23] = 0;

	texArray[24] = 7;
	texArray[25] = 0;
	texArray[26] = 0;
	texArray[27] = 0;

	texArray[28] = 0;
	texArray[29] = 0;
	texArray[30] = 0;
	texArray[31] = 0;

	texArray[32] = 9;
	texArray[33] = 0;
	texArray[34] = 0;
	texArray[35] = 0;



	glGenBuffers(1, &texBuf_[0]);
	glBindBuffer(GL_TEXTURE_BUFFER, texBuf_[0]);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(texArray), texArray, GL_DYNAMIC_COPY);

	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, texBuf_[0]);
  
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); // Create a standard texture with the width and height of our window  
  
	// Setup the basic texture parameters  
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  
	
	// Unbind the texture  
	glBindTexture(GL_TEXTURE_BUFFER, 0);  

	return checkForError("initTexture");
}  