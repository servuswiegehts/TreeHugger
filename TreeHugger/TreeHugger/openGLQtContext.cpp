#include "openGLQtContext.h"



//Prototypen
//---------------------------------------------------
float* loadTex (std::string imageName, unsigned int imgWidth, unsigned int imgHeight);
bool	checkForError (std::string const& command);
std::ostream& operator<< (std::ostream& stream, glm::mat4 const& mat);
//---------------------------------------------------



//Globale Variablen
//-------------------------------------------------

//-------------------------------------------------





#ifdef _WIN32
	static void CALLBACK amdCallback(unsigned int id, GLenum category, GLenum severity, int length, const GLchar* message,  void* userParam)
	{
			std::cout << std::endl << "Debug-CallBack:\n" << "ID: " << id << "\nCategory: " << category << "\nSeverity: " << severity << "\nLaenge: " << length << "\nMessage: " << message << std::endl << std::endl;
	}

	static void CALLBACK arbCallback(unsigned int id, unsigned int type, unsigned int category, unsigned int severity, int length, const char* message, void* userParam)
	{
			std::cout << std::endl << "Debug-CallBack:\n" << "ID: " << id << "\nCategory: " << category << "\nSeverity: " << severity << "\nLaenge: " << length << "\nMessage: " << message << std::endl << std::endl;
	}


	void setupAmdDebugPrinter()
	{
			glDebugMessageEnableAMD(GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
			glDebugMessageCallbackAMD(&amdCallback, NULL);
			//glDebugMessageInsertAMD(GL_DEBUG_CATEGORY_APPLICATION_AMD, GL_DEBUG_SEVERITY_HIGH_AMD, 1, 4, "test");
	}
	void setupArbDebugPrinter()
	{
			glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
			glDebugMessageCallbackARB(&arbCallback, NULL);
			//glDebugMessageInsertARB(GL_DEBUG_SOURCE_APPLICATION_ARB, GL_DEBUG_TYPE_OTHER_ARB, 1, GL_DEBUG_SEVERITY_HIGH_ARB, 4, "test");
	}
#endif


//
//Konstruktoren, Destruktoren
//
OpenGLQtContext::OpenGLQtContext(QGLFormat* context, QWidget *parent) :
		QGLWidget 		(*context, parent) ,
		
		// Stuff
		clWindOnOff_        (0),
		frameCount_         (0),
		lastPos_            (),
		timerFPS_           (),
		timerSinceLastFrame_(),
		timerWind_          (),
		//
		
		//
		// clWind-Variablen
		//
		oclObject  (),
		oclvbo     (),
		ocltexture (),
		source     (),
		dest       (),
		X          (),
		Y          (),
		Z          (),
		size       (),
		force      (),
		visc       (),
		//

		//
		// aktueller Baum
		//
		filename_          (),
		currentTreeString_ (),
		currentTree_       (),
		TreeHugger_        (),
		//
		calcTreeGrowth_    (1),
		iteration_         (0),
		maxIteration_      (0),
		//
		vertexArrayVector_ (),
		radArrayVector_    (),
		indexArrayVector_  (),
		dirArrayVector_    (),
		//
		parentArrayVector_        (),
		childCountArrayVector_    (),
		childIDsPlaceArrayVector_ (),
		childIDsArrayVector_      (),
		thicknessArrayVector_     (),
		rootVector_               (),
		//

		//
		// Texturen
		//
		rindeTex_   (),
		bodenTex_   (),
		blattTex_   (),
		grasTex_    (),
		// Texturdaten
		barkdata_   (),
		leafdata_   (),
		tellerdata_ (),
		grasdata_   (),	
		//

		//
		// Shader
		//
		calcRotProgram_  (),
		applyRotProgram_ (),
		tellerProgram_   (),
		//

		//
		// Shader-Uniforms
		//
		windCount_  (0),
		windOnOff_  (0),
		leafOnOff_  (1),
		texOnOff_   (1),
		lightPos_   (),
		//

		//
		// Shader-Transformationsmatrizen
		//
		projectionMatrix_ (),
		viewMatrix_       (),
		modelMatrix_      (),
		// Transformationsmatrizen-Locations
		projectionMatrixLocationCalc_   (),
		viewMatrixLocationCalc_         (),
		modelMatrixLocationCalc_        (),
		//
		projectionMatrixLocationApp_    (),
		viewMatrixLocationApp_          (),
		modelMatrixLocationApp_         (),
		//
		projectionMatrixLocationTeller_ (),
		viewMatrixLocationTeller_       (),
		modelMatrixLocationTeller_      (),
		//

		//
		// Uniform-Locations
		frameTimeLocationCalc_          (),
		frameCountLocationCalc_         (),
		windOnOffLocationCalc_          (),
		//		
		windOnOffLocationApp_           (),
		leafOnOffLocApp_                (),
		texturOnOffLocationApp_         (),
		lightPosLocApp_                 (),
		//

		//
		// VertexArrayObjects/IndexBuffer/VertexBuffer/sonstiges
		//
		// Baum
		baumVAO_          (),
		baumVerticesBuf_  (),
		radBuf_           (),
		dirBuf_           (),
		baumIBO_          (),
		//
		vertexCount_      (0),
		indexCount_       (0),
		//
		// Transform-FeedBack-Buffer/Texturen
		angleVeloBufTF_   (),
		posBufTF_         (),
		kiBufTF_          (),
		//
		posTexTF_         (),
		angleVeloTexTF_   (),
		kiTexTF_          (),
		//
		// Teller
		tellerVAO_        (),
		teller_ver_Buf_   (),
		teller_tex_Buf_   (),
		teller_ind_Buf_   (),
		//
		kreisVAO_         (),
		kreis_ver_Buf_    (),
		kreis_tex_Buf_    (),
		kreis_ind_Buf_    (),
		//
		indexTellerCount_ (),
		indexKreisCount_  (),
		//

		//
		// Eltern-Kind-Buffer/Texturen
		//
		parentTex_        (),
		childCountTex_    (),
		childIDsTex_      (),
		childIDsPlaceTex_ (),
		thickTex_         (),
		//
		parentBuf_        (),
		childCountBuf_    (),
		childIDsBuf_      (),
		childIDsPlaceBuf_ (),
		thickBuf_         ()
		//
{
	// Qt kann in mehrere Contexte zeichnen, dies setzt unseren neu erstellten als aktuellen 
	makeCurrent();
	
	
	if (!isValid())
		std::cout << "QGL: oGL context invalid" << std::endl;	
	if (!format().doubleBuffer())
		std::cout << "QGL: oGL doublebuffer deaktiviert" << std::endl;
	if ( !format().stencil() )
		qWarning( "QGL: Could not get stencil buffer; results will be suboptimal" );
	if ( !format().alpha() )
		qWarning( "QGL: Could not get alpha channel; results will be suboptimal" );
	if ( !format().depth() )
		qWarning( "QGL: Could not get depth buffer; results will be suboptimal" );	
		
	
	setFocusPolicy ( Qt::ClickFocus);

	timerFPS_ = new QTimer(this);
	connect(timerFPS_, SIGNAL(timeout()), this, SLOT(timeOutFPS()));
	timerFPS_->start(1000);
	
	timerWind_ = new QTimer(this);
	connect(timerWind_, SIGNAL(timeout()), this, SLOT(timeOutWind()));
	timerWind_->start(20);
}
//

OpenGLQtContext::~OpenGLQtContext()
{
	delete timerFPS_;
	delete timerWind_;

	delete source;
	delete dest;

	delete calcRotProgram_;
	delete applyRotProgram_;
	delete tellerProgram_;

	delete[] barkdata_;
	delete[] leafdata_;
	delete[] tellerdata_;
	delete[] grasdata_;
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
		std::cout << "QGL: ERROR "<<glewGetErrorString(glewInitResult) << std::endl;
		exit(EXIT_FAILURE);
	}			

	if (glewIsExtensionSupported("GL_ARB_debug_output"))
	{
		std::cout << "QGL: ARB debug output verfuegbar" << std::endl;
                //setupArbDebugPrinter();
	}
	else if (glewIsExtensionSupported("GL_AMD_debug_output"))
	{
		std::cout << "QGL: AMD debug output verfuegbar" << std::endl;
                //setupAmdDebugPrinter();
	}
	else
	{
		std::cout << "QGL: Kein debug output verfuegbar oder deaktiviert" << std::endl;
	}


	std::cout << "QGL: OpenGL Version: " << glGetString(GL_VERSION) << std::endl << std::endl;
	//int contflags=888;
	//glGetIntegerv(GL_CONTEXT_FLAGS, &contflags);
	//std::cout << "Context-Flags: " << contflags << std::endl;

	glClearDepth(1.0f);
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );


	glEnable( GL_DEPTH_TEST );
	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	X = 62;
	Y = 62;
	Z = 62;

	force = 1.0;
	visc = 0.0f;
	
	size = (X+2)*(Y+2)*(Z+2);

	source = new cl_float4[size];
	dest = new cl_float4[size];

	for(int i =0 ; i < size; ++i) 
	{
		source[i].s[0] = 0.5;
		source[i].s[1] = 0.5;
		source[i].s[2] = 0.5;
		source[i].s[3] = 0.0;
	}

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
	filename_ = "Baum_1.txt";
	calcRotProgram_  = new Shader("calcRot.vert", "calcRot.geom", "calcRot.frag");
	applyRotProgram_ = new Shader("applyRot.vert", "applyRot.geom", "applyRot.frag");
	tellerProgram_ = new Shader("teller.vert", "teller.frag");

	barkdata_ = loadTex("rinde_neu.jpg",2000,3008);
	leafdata_ = loadTex("blatt.png",100,75);
	tellerdata_ = loadTex("boden.jpg",395,230);
	grasdata_ = loadTex("gras.jpg", 600, 450);

	lightPos_ = glm::vec3 (50.0, 30.0, 3.0);

	static const char* transFeedbVar[] =
	{
		"outAngleVelo",
		"gl_Position",
		"outKiForce"

	};
	glTransformFeedbackVaryings(calcRotProgram_->getID(), 3, transFeedbVar, GL_SEPARATE_ATTRIBS);	
	glLinkProgram (calcRotProgram_->getID());

#ifdef _DEBUG
	printProgramInfoLog(applyRotProgram_->getID());
#endif

	// Projektionsmatrix und Viewport setzen
	projectionMatrix_ = glm::perspective(60.0f, 600.0f / 600.0f, 1.0f, 1000000.f);
	glViewport(0, 0, 600.0f, 600.0f);

	// Viewmatrix und ModelMatrix setzen
	viewMatrix_ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -8.0f, -30.0f));
	modelMatrix_ = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));


	//
	// Get Uniform-Locations
	//
	projectionMatrixLocationCalc_ = glGetUniformLocation(calcRotProgram_->getID(), "projectionMatrix");
	viewMatrixLocationCalc_ = glGetUniformLocation(calcRotProgram_->getID(), "viewMatrix");
	modelMatrixLocationCalc_ = glGetUniformLocation(calcRotProgram_->getID(), "modelMatrix");

	frameTimeLocationCalc_ = glGetUniformLocation(calcRotProgram_->getID(), "frameTime");
	frameCountLocationCalc_ = glGetUniformLocation(calcRotProgram_->getID(), "frameCount");
	windOnOffLocationCalc_ = glGetUniformLocation(calcRotProgram_->getID(), "windOnOff");
	//
	//
	projectionMatrixLocationApp_ = glGetUniformLocation(applyRotProgram_->getID(), "projectionMatrix");
	viewMatrixLocationApp_ = glGetUniformLocation(applyRotProgram_->getID(), "viewMatrix");
	modelMatrixLocationApp_ = glGetUniformLocation(applyRotProgram_->getID(), "modelMatrix");
	
	texturOnOffLocationApp_=glGetUniformLocation(applyRotProgram_->getID(),"texOnOff");
	leafOnOffLocApp_ = glGetUniformLocation(applyRotProgram_->getID(), "leafOnOff");
	lightPosLocApp_ = glGetUniformLocation(applyRotProgram_->getID(), "cameraSpaceLightPos");
	windOnOffLocationApp_ = glGetUniformLocation(applyRotProgram_->getID(), "windOnOff");
	//
	//
	projectionMatrixLocationTeller_ = glGetUniformLocation(tellerProgram_->getID(), "projectionMatrix");
	viewMatrixLocationTeller_ = glGetUniformLocation(tellerProgram_->getID(), "viewMatrix");
	modelMatrixLocationTeller_ = glGetUniformLocation(tellerProgram_->getID(), "modelMatrix");
	//

	//
	// Set Uniforms
	//
	calcRotProgram_->bind();

		glUniformMatrix4fv(projectionMatrixLocationCalc_, 1, GL_FALSE, &projectionMatrix_[0][0]); // Send our projection matrix to the shader[Shader::currentShader_]
		glUniformMatrix4fv(viewMatrixLocationCalc_, 1, GL_FALSE, &viewMatrix_[0][0]); // Send our view matrix to the shader[Shader::currentShader_]
		glUniformMatrix4fv(modelMatrixLocationCalc_, 1, GL_FALSE, &modelMatrix_[0][0]); // Send our model matrix to the shader[Shader::currentShader_]

		glUniform1i(windOnOffLocationCalc_, windOnOff_);

		int texChildCount = glGetUniformLocation(calcRotProgram_->getID(), "childCountTex");
		glUniform1i(texChildCount, 4);
		int texChildPlace = glGetUniformLocation(calcRotProgram_->getID(), "childIDsPlaceTex");
		glUniform1i(texChildPlace, 5);
		int texChildIDs = glGetUniformLocation(calcRotProgram_->getID(), "childIDsTex");
		glUniform1i(texChildIDs, 6);
		int texThickness = glGetUniformLocation(calcRotProgram_->getID(), "thicknessTex");
		glUniform1i(texThickness, 7);

	calcRotProgram_->unbind();
	//
	//
	applyRotProgram_->bind();

		glUniformMatrix4fv(projectionMatrixLocationApp_, 1, GL_FALSE, &projectionMatrix_[0][0]); // Send our projection matrix to the shader[Shader::currentShader_]
		glUniformMatrix4fv(viewMatrixLocationApp_, 1, GL_FALSE, &viewMatrix_[0][0]); // Send our view matrix to the shader[Shader::currentShader_]
		glUniformMatrix4fv(modelMatrixLocationApp_, 1, GL_FALSE, &modelMatrix_[0][0]); // Send our model matrix to the shader[Shader::currentShader_]
		
		glUniform1i(leafOnOffLocApp_, leafOnOff_);
		glUniform3fv(lightPosLocApp_, 1, &lightPos_[0]);
		glUniform1i(windOnOffLocationCalc_, windOnOff_);
		glUniform1i(texturOnOffLocationApp_, texOnOff_);

		int texParent = glGetUniformLocation(applyRotProgram_->getID(), "parent");
		glUniform1i(texParent, 3);		
		texChildCount = glGetUniformLocation(applyRotProgram_->getID(), "childCountTex");
		glUniform1i(texChildCount, 4);
		int texRinde = glGetUniformLocation(applyRotProgram_->getID(), "rinde");
		glUniform1i(texRinde, 20);
		int texBlatt = glGetUniformLocation(applyRotProgram_->getID(), "blatt");
		glUniform1i(texBlatt, 21);

	applyRotProgram_->unbind();
	//
	//
	tellerProgram_->bind();

		glUniformMatrix4fv(projectionMatrixLocationTeller_, 1, GL_FALSE, &projectionMatrix_[0][0]); // Send our projection matrix to the shader[Shader::currentShader_]
		glUniformMatrix4fv(viewMatrixLocationTeller_, 1, GL_FALSE, &viewMatrix_[0][0]); // Send our view matrix to the shader[Shader::currentShader_]
		glUniformMatrix4fv(modelMatrixLocationTeller_, 1, GL_FALSE, &modelMatrix_[0][0]); // Send our model matrix to the shader[Shader::currentShader_]
	
	tellerProgram_->unbind();
	//


	//
	//"Bild"-Texturen
	//
	glGenTextures(1, &rindeTex_);
	glGenTextures(1, &blattTex_);

	glGenTextures(1, &grasTex_);
	glGenTextures(1, &bodenTex_);
	//

	//
	//Eltern-Kind-Texturen
	//
	glGenTextures(1, &parentTex_);
	glGenTextures(1, &childCountTex_);
	glGenTextures(1, &childIDsTex_);
	glGenTextures(1, &childIDsPlaceTex_);
	//

	//
	//Baum-Texturen
	//
	glGenTextures(2, &posTexTF_[0]);
	glGenTextures(2, &kiTexTF_[0]);
	glGenTextures(1, &thickTex_);
	glGenTextures(2, &angleVeloTexTF_[0]);
	//

	//
	//Baum-Buffer
	//
	glGenVertexArrays(2, &baumVAO_);
	//
	glGenBuffers(2, &baumVerticesBuf_);
	glGenBuffers(2, &angleVeloBufTF_[0]);
	glGenBuffers(2, &posBufTF_[0]);
	glGenBuffers(1, &baumIBO_);
	glGenBuffers(1, &radBuf_);
	glGenBuffers(1, &dirBuf_);
	glGenBuffers(2, &kiBufTF_[0]);
	glGenBuffers(1, &thickBuf_);
	//

	//
	//Eltern-Kind-Buffer
	//
	glGenBuffers(1, &parentBuf_);
	glGenBuffers(1, &childCountBuf_);
	glGenBuffers(1, &childIDsPlaceBuf_);
	glGenBuffers(1, &childIDsBuf_);
	//



	//
	// Bind Transform-Feedback Buffer
	//
	// Winkelgeschwindigkeit
	//
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, angleVeloBufTF_[0]);
	glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, 10, NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, angleVeloBufTF_[0]);

	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, angleVeloBufTF_[1]);
	glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, 10, NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, angleVeloBufTF_[1]);
	//
	// neue Position
	//
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, posBufTF_[0]);
	glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, 10, NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, posBufTF_[0]);

	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, posBufTF_[1]);
	glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, 10, NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, posBufTF_[1]);
	//
	// Restoration-Force
	//
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, kiBufTF_[0]);
	glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, 10, NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, kiBufTF_[0]);
		
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, kiBufTF_[1]);
	glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, 10, NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, kiBufTF_[1]);
	//	



	TreeHugger_.parse_lsystem(filename_.c_str(),5);
	currentTree_=tree(2,TreeHugger_.get_angle(),TreeHugger_.get_length());
	
	
	//
	// Setup openCL-Context
	//
	cl_int3 N = {X,Y,Z};

	oclObject.init();
	oclObject.setN(N);
	oclObject.setSize(size);

	glGenBuffers(1, &oclvbo[0]);
	glGenTextures(1, &ocltexture[0]);

	glBindBuffer(GL_ARRAY_BUFFER, oclvbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cl_float4) * size, &source[0], GL_DYNAMIC_COPY);

	glBindTexture(GL_TEXTURE_BUFFER, ocltexture[0]);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, oclvbo[0]);

	glBindBuffer(GL_ARRAY_BUFFER,0);

	oclObject.allocateMemory(oclvbo);
	//


	//
	// Baum erstellen
	//
	currentTreeString_ = TreeHugger_.get_axiom();
	initTree(currentTreeString_);
	//


	//
	// "Boden" erstellen
	//
	make_teller();
	//


	//
	// Sicherstellen, dass alle Texturen gebunden und aktiviert sind
	//
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_BUFFER, parentTex_);	

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_BUFFER, childCountTex_);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_BUFFER, childIDsPlaceTex_);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_BUFFER, childIDsTex_);	

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_BUFFER, thickTex_);	
	//
	//
	glActiveTexture(GL_TEXTURE20);
	glBindTexture(GL_TEXTURE_2D, rindeTex_);
		
	glActiveTexture(GL_TEXTURE21);
	glBindTexture(GL_TEXTURE_2D, blattTex_);

	glActiveTexture(GL_TEXTURE22);
	glBindTexture(GL_TEXTURE_2D, bodenTex_);
	
	glActiveTexture(GL_TEXTURE23);
	glBindTexture(GL_TEXTURE_2D, grasTex_);
	//
	//
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, angleVeloTexTF_[0]);

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_BUFFER, angleVeloTexTF_[1]);
	//


	// Timer für FPS starten
	timerSinceLastFrame_.start();
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

	//
	// oCL updaten
	//
	if (clWindOnOff_)
	{
		oclObject.addSourceVelocity(source,0,0,visc,1.2,0);

		memset(source,0,sizeof(cl_float4)*(X+2)*(Y+2)*(Z+2));
		glBindBuffer(GL_ARRAY_BUFFER,oclvbo[0]);

		glActiveTexture(GL_TEXTURE30);
		glBindTexture(GL_TEXTURE_BUFFER, ocltexture[0]);
	}
	//


//
//--------------------------------------------------------
// Berechne Kräfte auf einzelne Zylinder
// Verwende Ki vom letzten Durchlauf
//--------------------------------------------------------
//
	calcRotProgram_->bind();

	float time = timerSinceLastFrame_.elapsed();
	timerSinceLastFrame_.restart();
	glUniform1f(frameTimeLocationCalc_, time);
	glUniform1i(frameCountLocationCalc_, frameCount_);
	
	int texWind = glGetUniformLocation(calcRotProgram_->getID(), "windTex");
	glUniform1i(texWind, 12);


	glBindVertexArray(baumVAO_);

	if (frameCount_ % 2 == 0)
	{
		// AngleVelo out
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, angleVeloBufTF_[1]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, angleVeloBufTF_[1]);
		// AngleVelo in
		glUniform1i(glGetUniformLocation(calcRotProgram_->getID(), "angleVeloTex"), 0);

		// Pos out
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, posBufTF_[1]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, posBufTF_[1]);
		// Pos in
		glUniform1i(glGetUniformLocation(calcRotProgram_->getID(), "posTex"), 1);


		// Ki-Force out
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, kiBufTF_[1]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, kiBufTF_[1]);
		// Ki-Force in
		glUniform1i(glGetUniformLocation(calcRotProgram_->getID(), "kiTex"), 2);
	}
	else
	{		
		// AngleVelo out
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, angleVeloBufTF_[0]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, angleVeloBufTF_[0]);
		// AngleVelo in	
		glUniform1i(glGetUniformLocation(calcRotProgram_->getID(), "angleVeloTex"), 10);

		// Pos out
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, posBufTF_[0]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, posBufTF_[0]);
		// Pos in
		glUniform1i(glGetUniformLocation(calcRotProgram_->getID(), "posTex"), 11);


		// Ki-Force out
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, kiBufTF_[0]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, kiBufTF_[0]);
		// Ki-Force in
		glUniform1i(glGetUniformLocation(calcRotProgram_->getID(), "kiTex"), 12);
	}

	glBeginTransformFeedback(GL_LINES);
		glEnable(GL_RASTERIZER_DISCARD);
			

			glDrawElements(GL_LINES, indexCount_, GL_UNSIGNED_INT, 0);
		
			
		glDisable(GL_RASTERIZER_DISCARD);
	glEndTransformFeedback();


	calcRotProgram_->unbind();
//
//----------------------------------------------------------------
//



//---------------------------------
// Bufferinhalt ausgeben
//---------------------------------
	//{
	//glBindBuffer(GL_ARRAY_BUFFER, posBufTF_[0]);
	//float *test = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	//std::cout << "framecount: " << frameCount_ << std::endl;
	//for (int i = 0; i < 200; i++)
	//{
	//	std::cout << test[i] << " ";
	//	
	//
	//	if (i % 4 == 3)
	//		std::cout << std::endl;

	//
	//}
	//std::cout << std::endl;

	//glUnmapBuffer(GL_ARRAY_BUFFER);
	//}
//-----------------------------------





//
//--------------------------------------------------------
// Addiert Kräfte auf Zylinder und Kinder auf
//--------------------------------------------------------
//
	applyRotProgram_->bind();
	
	glBindVertexArray(baumVAO_);

	if (frameCount_ % 2 == 0)
	{
		glUniform1i(glGetUniformLocation(applyRotProgram_->getID(), "posTex"), 11);
	}
	else
	{
		glUniform1i(glGetUniformLocation(applyRotProgram_->getID(), "posTex"), 1);
	}


			glDrawElements(GL_LINES, indexCount_, GL_UNSIGNED_INT, 0);


	applyRotProgram_->unbind();
//
//----------------------------------------------------------------
//




//
//--------------------------------------------------------
// Zeichnet Teller
//--------------------------------------------------------
//
	tellerProgram_->bind();


	int texBoden = glGetUniformLocation(tellerProgram_->getID(), "boden");
	glUniform1i(texBoden, 22);			
	
	glBindVertexArray(tellerVAO_);
	
		glDrawElements(GL_TRIANGLES, indexTellerCount_, GL_UNSIGNED_INT, 0);
	


	glUniform1i(texBoden, 23);	

	glBindVertexArray(kreisVAO_);
	
		glDrawElements(GL_TRIANGLES, indexKreisCount_, GL_UNSIGNED_INT, 0);


	tellerProgram_->unbind();
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
	resize(width, height);
	projectionMatrix_ = glm::perspective(60.0f, float(width) / float(height), 0.1f, 100.f);
}
//


void OpenGLQtContext::rotTree(bool dx)
{
		if (dx)
			modelMatrix_ = glm::rotate(modelMatrix_, float(M_PI), glm::vec3(0.0f, 1.0f, 0.0f));
		else
			modelMatrix_ = glm::rotate(modelMatrix_, float(-M_PI), glm::vec3(0.0f, 1.0f, 0.0f));

		calcRotProgram_->bind();
			glUniformMatrix4fv(projectionMatrixLocationCalc_, 1, GL_FALSE, &projectionMatrix_[0][0]);
			glUniformMatrix4fv(viewMatrixLocationCalc_, 1, GL_FALSE, &viewMatrix_[0][0]);
			glUniformMatrix4fv(modelMatrixLocationCalc_, 1, GL_FALSE, &modelMatrix_[0][0]);
		calcRotProgram_->unbind();

		applyRotProgram_->bind();
			glUniformMatrix4fv(projectionMatrixLocationApp_, 1, GL_FALSE, &projectionMatrix_[0][0]);
			glUniformMatrix4fv(viewMatrixLocationApp_, 1, GL_FALSE, &viewMatrix_[0][0]);
			glUniformMatrix4fv(modelMatrixLocationApp_, 1, GL_FALSE, &modelMatrix_[0][0]);
		applyRotProgram_->unbind();

		tellerProgram_->bind();
			glUniformMatrix4fv(projectionMatrixLocationTeller_, 1, GL_FALSE, &projectionMatrix_[0][0]);
			glUniformMatrix4fv(viewMatrixLocationTeller_, 1, GL_FALSE, &viewMatrix_[0][0]);
			glUniformMatrix4fv(modelMatrixLocationTeller_, 1, GL_FALSE, &modelMatrix_[0][0]);
		tellerProgram_->unbind();
}
//
// event, touchEvent
//
// legacy, wurde für Touchevent von TUIO verwendet, ersetzt durch Signal/Slot-System
//
bool OpenGLQtContext::event(QEvent* event)
{

	switch(event->type())
	{
		case QEvent::TouchBegin:
		case QEvent::TouchUpdate:
		case QEvent::TouchEnd:
		{
			QList<QTouchEvent::TouchPoint> touchPoints = static_cast<QTouchEvent*> (event)->touchPoints();
			foreach (const QTouchEvent::TouchPoint &touchPoint, touchPoints)
			{
				int a = touchPoint.state();
				switch (touchPoint.state())
				{
				case Qt::TouchPointStationary:
					continue;
				default:
					{
						touchEvent (static_cast<QTouchEvent*> (event));
					}
				}
			}
			return true;
		}
	}
	return QGLWidget::event(event);
}

void OpenGLQtContext::touchEvent(QTouchEvent* event)
{
	std::cout << "QGL: Touchevent angekommen!";
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
		std::cout << "QGL: keinen Focus!" << std::endl;
	}
	
	
	int i=5;
	std::string temp;
	switch (key)
	{
		case Qt::Key_Escape:
					qApp->quit();
					break;


		case Qt::Key_Left:
					modelMatrix_ = glm::translate (glm::mat4(1.0f), glm::vec3 (1.0f, 0.0f, 0.0f)) * modelMatrix_;
					break;
					
		case Qt::Key_Right:
					modelMatrix_ = glm::translate (glm::mat4(1.0f), glm::vec3 (-1.0f, 0.0f, 0.0f)) * modelMatrix_;
					break;

		case Qt::Key_Up:
					modelMatrix_ = glm::translate (glm::mat4(1.0f), glm::vec3 (0.0f, 1.0f, 0.0f)) * modelMatrix_;
					break;
					
		case Qt::Key_Down:
					modelMatrix_ = glm::translate (glm::mat4(1.0f), glm::vec3 (0.0f, -1.0f, 0.0f)) * modelMatrix_;
					break;
					
		case Qt::Key_PageUp:
					modelMatrix_ = glm::translate (glm::mat4(1.0f), glm::vec3 (0.0f, 0.0f, 1.0f)) * modelMatrix_;
					break;
					
		case Qt::Key_PageDown:
					modelMatrix_ = glm::translate (glm::mat4(1.0f), glm::vec3 (0.0f, 0.0f, -1.0f)) * modelMatrix_;
					break;

		//case Qt::Key_Plus:
		//			modelMatrix_ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, 3.0f)) * modelMatrix_;
		//			
		//			break;

		//case Qt::Key_Minus:	
		//			modelMatrix_ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, 3.0f)) * modelMatrix_;
					
		//			break;

		//case Qt::Key_D:
		//			modelMatrix_ = glm::rotate(modelMatrix_, float(-M_PI), glm::vec3(0.0f, 1.0f, 0.0f));
		//			break;
		//			

		//case Qt::Key_A:
		//			modelMatrix_ = glm::rotate(modelMatrix_, float(M_PI), glm::vec3(0.0f, 1.0f, 0.0f));
		//			break;

		//case Qt::Key_W:
		//			viewMatrix_ = glm::rotate(viewMatrix_, float(M_PI), glm::vec3(1.0f, 0.0f, 0.0f));
		//			break;

		//case Qt::Key_E:	
		//			modelMatrix_ = glm::rotate(modelMatrix_, float(-M_PI), glm::vec3(0.0f, 0.0f, 1.0f));
		//			break;

		//case Qt::Key_Q:
		//			modelMatrix_ = glm::rotate(modelMatrix_, float(M_PI), glm::vec3(0.0f, 0.0f, 1.0f));
		//			break;

		//case Qt::Key_S:
		//			modelMatrix_ = glm::rotate(modelMatrix_, float(-M_PI), glm::vec3(1.0f, 0.0f, 0.0f));
		//			break;


		case Qt::Key_0:
					++iteration_;
					currentTreeString_= TreeHugger_.parse_lsystem(filename_.c_str(), iteration_);
					initTree(currentTreeString_);
					break;

		case Qt::Key_F:
					currentTreeString_+="F";
					initTree(currentTreeString_);
					break;

		case Qt::Key_N:
					std::cout<<"QGL: Anzahl Zylinder: " << currentTree_.get_num_cylinders()<<std::endl;
					std::cout<<"QGL: Laenge String: " << currentTreeString_.length()<<std::endl;
					std::cout<<"QGL: Groesse String: " << float(currentTreeString_.length())/1024.0f/1024.0f <<" MB"<<std::endl;
					break;

		case Qt::Key_K:
					killTree(filename_);
					break;
					

		case Qt::Key_W:
					windOnOff_ = !windOnOff_;
					calcRotProgram_->bind();
						glUniform1i(windOnOffLocationCalc_, windOnOff_);
					calcRotProgram_->unbind();
					applyRotProgram_->bind();
						glUniform1i(windOnOffLocationApp_, windOnOff_);
					applyRotProgram_->unbind();
					break;

		case Qt::Key_X:
					if (windOnOff_)
					{
						makeWind();
					}
					break;


		case Qt::Key_L:
					leafOnOff_ = !leafOnOff_;
					applyRotProgram_->bind();
						glUniform1i(leafOnOffLocApp_, leafOnOff_);
					applyRotProgram_->unbind();
					break;


		case Qt::Key_T:
					applyRotProgram_->bind();
					texOnOff_ = !texOnOff_;
						glUniform1i(texturOnOffLocationApp_, texOnOff_);
					applyRotProgram_->unbind();
					break;



		case Qt::Key_H:		
					std::cout << std::endl << "QGL: (c)ontinue, (q)uit: " << std::endl;
					char k;
					do
					{
						std::cout << "QGL: Bitte c oder q eingeben: ";
						std::cin >> k;
						std::cout << k << std::endl;
					} while ( (k != 'c') && (k != 'q') );
					
					if (k == 'q')
					{
						qApp->quit();
					}
					break;
		
		default:		
					std::cout << "QGL: Key nicht belegt" << std::endl;
	}

	calcRotProgram_->bind();
		glUniformMatrix4fv(projectionMatrixLocationCalc_, 1, GL_FALSE, &projectionMatrix_[0][0]);
		glUniformMatrix4fv(viewMatrixLocationCalc_, 1, GL_FALSE, &viewMatrix_[0][0]);
		glUniformMatrix4fv(modelMatrixLocationCalc_, 1, GL_FALSE, &modelMatrix_[0][0]);
	calcRotProgram_->unbind();

	applyRotProgram_->bind();
		glUniformMatrix4fv(projectionMatrixLocationApp_, 1, GL_FALSE, &projectionMatrix_[0][0]);
		glUniformMatrix4fv(viewMatrixLocationApp_, 1, GL_FALSE, &viewMatrix_[0][0]);
		glUniformMatrix4fv(modelMatrixLocationApp_, 1, GL_FALSE, &modelMatrix_[0][0]);
	applyRotProgram_->unbind();

	tellerProgram_->bind();
		glUniformMatrix4fv(projectionMatrixLocationTeller_, 1, GL_FALSE, &projectionMatrix_[0][0]);
		glUniformMatrix4fv(viewMatrixLocationTeller_, 1, GL_FALSE, &viewMatrix_[0][0]);
		glUniformMatrix4fv(modelMatrixLocationTeller_, 1, GL_FALSE, &modelMatrix_[0][0]);
	tellerProgram_->unbind();

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

	int i,j,k, win_x, win_y;

	win_x = width();
	win_y = height();
	
	if (event->y() > 800)
	{
		if (dx < 0)
			modelMatrix_ = glm::rotate(modelMatrix_, float(-M_PI), glm::vec3(0.0f, 1.0f, 0.0f));
		else
			modelMatrix_ = glm::rotate(modelMatrix_, float(M_PI), glm::vec3(0.0f, 1.0f, 0.0f));

		calcRotProgram_->bind();
			glUniformMatrix4fv(projectionMatrixLocationCalc_, 1, GL_FALSE, &projectionMatrix_[0][0]);
			glUniformMatrix4fv(viewMatrixLocationCalc_, 1, GL_FALSE, &viewMatrix_[0][0]);
			glUniformMatrix4fv(modelMatrixLocationCalc_, 1, GL_FALSE, &modelMatrix_[0][0]);
		calcRotProgram_->unbind();

		applyRotProgram_->bind();
			glUniformMatrix4fv(projectionMatrixLocationApp_, 1, GL_FALSE, &projectionMatrix_[0][0]);
			glUniformMatrix4fv(viewMatrixLocationApp_, 1, GL_FALSE, &viewMatrix_[0][0]);
			glUniformMatrix4fv(modelMatrixLocationApp_, 1, GL_FALSE, &modelMatrix_[0][0]);
		applyRotProgram_->unbind();

		tellerProgram_->bind();
			glUniformMatrix4fv(projectionMatrixLocationTeller_, 1, GL_FALSE, &projectionMatrix_[0][0]);
			glUniformMatrix4fv(viewMatrixLocationTeller_, 1, GL_FALSE, &viewMatrix_[0][0]);
			glUniformMatrix4fv(modelMatrixLocationTeller_, 1, GL_FALSE, &modelMatrix_[0][0]);
		tellerProgram_->unbind();
	}
	else if (clWindOnOff_)
	{
		i = (int)((       dx /(cl_float)win_x)*X+1);
		j = (int)(((win_y-dy)/(cl_float)win_y)*Y+1);
		k = (int)(((win_y-dy)/(cl_float)win_y)*Z+1);

		if (event->buttons() & Qt::LeftButton) 
		{
			if(!( i<1 || i>X || j<1 || j>Y || k<1 || k >Z))
			{
				source[IX3(i,j,k)].s[0] = force * (dx - lastPos_.x());
				source[IX3(i,j,k)].s[1] = force * (lastPos_.y() - dy);
				source[IX3(i,j,k)].s[2] = force * (lastPos_.y() - dy);
			}
		} 
		//else if (event->buttons() & Qt::RightButton) 
		//{
		//	modelMatrix_ = glm::rotate(modelMatrix_, float(dy)/2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		//	modelMatrix_ = glm::rotate(modelMatrix_, float(dx)/2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		//}
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
//void OpenGLQtContext::mouseReleaseEvent(QMouseEvent *event)
//{
//
//}
//


//
// timeOut-Funktionen
//
// wird aufgerufen, wenn zugehöriger Timer abläuft
//
void OpenGLQtContext::timeOutFPS()
{
	QString v;
	v.setNum(frameCount_);
	frameCount_=0;
	setWindowTitle(tr("TreeHugger_ @")+v+tr(" FPS"));
	std::cout << "QGL: " << v.toStdString() << " FPS\n";
}
//
void OpenGLQtContext::timeOutWind()
{
	int windCountLocationCalc_ = glGetUniformLocation(calcRotProgram_->getID(), "windCount");
	if (windCount_)
	{	calcRotProgram_->bind();
			windCount_ -=5;
			glUniform1i(windCountLocationCalc_, windCount_);
		calcRotProgram_->unbind();
	}
}
//


//
//initTree
//
void OpenGLQtContext::initTree(std::string treeString)
{

	if(calcTreeGrowth_==1)
	{
		currentTree_.drawTree(treeString);
		currentTree_.set();
	}



//------------
// Rinde
//------------
	glActiveTexture(GL_TEXTURE20);
	glBindTexture(GL_TEXTURE_2D, rindeTex_);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2000, 3008, 0, GL_RGB, GL_FLOAT, barkdata_);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
//

//------------
// Blätter
//------------
	glActiveTexture(GL_TEXTURE21);
	glBindTexture(GL_TEXTURE_2D, blattTex_);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 100, 75, 0, GL_RGBA, GL_FLOAT, leafdata_);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
//


//----------------------------
//Füllen der Buffer
//----------------------------
	if(calcTreeGrowth_ == 1)
	{
		vertexArrayVector_.push_back(currentTree_.get_vertices());
		indexArrayVector_.push_back(currentTree_.get_indices());
		
		radArrayVector_.push_back(currentTree_.get_radien());
		dirArrayVector_.push_back(currentTree_.get_dir());
	}

	std::vector<float> const& vertexArray = vertexArrayVector_[iteration_];
	std::vector<unsigned int> const& indexArray = indexArrayVector_[iteration_];

	std::vector<float> const& radArray = radArrayVector_[iteration_];
	std::vector<float> const& dirArray = dirArrayVector_[iteration_];

	std::cout << "QGL: Die Zylinder wurden berechnet, Anzahl Zylinder: " << vertexArray.size()/8 << "/" << currentTree_.get_num_cylinders() << std::endl << "QGL: Berechne Eltern-Kind-Beziehung...\n";
	
	vertexCount_ = vertexArray.size();
	indexCount_ = indexArray.size();


//
// angleVeloTex
//
	glBindBuffer(GL_ARRAY_BUFFER, angleVeloBufTF_[0]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount_*sizeof(float), NULL, GL_DYNAMIC_COPY);

	glBindBuffer(GL_ARRAY_BUFFER, angleVeloBufTF_[1]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount_*sizeof(float), NULL, GL_DYNAMIC_COPY);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, angleVeloTexTF_[0]);
	glBindBuffer(GL_TEXTURE_BUFFER, angleVeloBufTF_[0]);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, angleVeloBufTF_[0]);

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_BUFFER, angleVeloTexTF_[1]);
	glBindBuffer(GL_TEXTURE_BUFFER, angleVeloBufTF_[1]);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, angleVeloBufTF_[1]);
//


//
// position (neue, nach bewegung)
//
	glBindBuffer(GL_ARRAY_BUFFER, posBufTF_[0]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount_*sizeof(float), NULL, GL_DYNAMIC_COPY);

	glBindBuffer(GL_ARRAY_BUFFER, posBufTF_[1]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount_*sizeof(float), NULL, GL_DYNAMIC_COPY);
	
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, posTexTF_[0]);
	glBindBuffer(GL_TEXTURE_BUFFER, posBufTF_[0]);

	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, posBufTF_[0]);

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_BUFFER, posTexTF_[1]);
	glBindBuffer(GL_TEXTURE_BUFFER, posBufTF_[1]);

	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, posBufTF_[1]);
//


	glBindBuffer(GL_ARRAY_BUFFER, 0);




	glBindVertexArray(baumVAO_);

		glBindBuffer(GL_ARRAY_BUFFER, baumVerticesBuf_);
		glBufferData(GL_ARRAY_BUFFER, vertexCount_*sizeof(float), &(vertexArray[0]), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);


		glBindBuffer(GL_ARRAY_BUFFER, radBuf_);
		glBufferData(GL_ARRAY_BUFFER, radArray.size()*sizeof(float), &(radArray[0]), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);


		glBindBuffer(GL_ARRAY_BUFFER, dirBuf_);
		glBufferData(GL_ARRAY_BUFFER, dirArray.size()*sizeof(float), &(dirArray[0]), GL_STATIC_DRAW);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(7);


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, baumIBO_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount_*sizeof(unsigned int), &(indexArray[0]), GL_STATIC_DRAW);

	glBindVertexArray(0);
//

//------------
// Berechnung Eltern-Kind Beziehung
//------------
	genTree();
	
	std::cout << "QGL: Eltern-Kind-Beziehung wurde berechnet\nQGL: Iterationsschritt: " << iteration_ << std::endl << std::endl;
//


	frameCount_ = 0;
	currentTree_.flush();
}
//



//
// genTree
//
void OpenGLQtContext::genTree()
{
	int z = 0;
	if(calcTreeGrowth_ == 1)
	{
		std::map<int, Cylinder*> const* cylMapCurrent = currentTree_.getCylinder();

		//
		// Parent
		//
		std::vector<int>* parentArrayBuf = new std::vector<int>(cylMapCurrent->size(), 0);

		bool parent = 0;
		for (std::map<int, Cylinder*>::const_iterator i = cylMapCurrent->begin(); i != cylMapCurrent->end(); ++i)
		{
			for (std::map<int, Cylinder*>::const_iterator j = cylMapCurrent->begin(); j != cylMapCurrent->end(); ++j)
			{
				if ( glm::length(i->second->von_ - j->second->nach_) <= 0.001 )
				{
					(*parentArrayBuf)[i->first] = j->first;
					parent = TRUE;
					break; 
				}
			}
			if (parent == FALSE)
			{
				(*parentArrayBuf)[i->first] = -1;
				rootVector_.push_back(i->first);
			}
			parent = FALSE;
		}
		parentArrayVector_.push_back(parentArrayBuf);

		// Ausgabe Eltern-Kind-Beziehung
		//
		//std::cout << std::endl << "Inhalt ParentArray: " << std::endl;
		//for (std::vector<int>::iterator i = parentArrayBuf.begin(); i != parentArrayBuf.end(); ++i)
		//{
		//	std::cout << z << " | ";
		//	++z;
		//}
		//std::cout << std::endl << "------------------------------------" << std::endl;
		//for (std::vector<int>::iterator i = parentArrayBuf.begin(); i != parentArrayBuf.end(); ++i)
		//{
		//	std::cout << *i << " | ";
		//}	
		//z = 0;
		//std::cout << std::endl;


		
		//
		// childCount
		//
		std::vector<int>* childArrayCountBuf = new std::vector<int> (cylMapCurrent->size(), 0);

		for (std::vector<int>::iterator i = parentArrayBuf->begin(); i != parentArrayBuf->end(); ++i)
		{	
			if( *i == -1)
				continue;
			++(*childArrayCountBuf)[*i];
		}
		childCountArrayVector_.push_back(childArrayCountBuf);

		// Ausgabe Child-Count
		//
		//std::cout << std::endl << "Inhalt ChildArrayCount: " << std::endl;
		//for (std::vector<int>::iterator i = childArrayCountBuf.begin(); i != childArrayCountBuf.end(); ++i)
		//{
		//	std::cout << z << " | ";
		//	++z;
		//}
		//std::cout << std::endl << "------------------------------------" << std::endl;
		//for (std::map<int, int>::iterator i = childArrayCountBuf.begin(); i != childArrayCountBuf.end(); ++i)
		//{
		//	std::cout << i->second << " | ";
		//}
		//z = 0;
		//std::cout << std::endl;
		//	



		//
		// childIDsPlace
		//
		std::vector<int>* childArrayIDsPlaceBuf = new std::vector<int> (cylMapCurrent->size(), 0);

		int childCount = 0;
		for (std::map<int,Cylinder*>::const_iterator i = cylMapCurrent->begin(); i != cylMapCurrent->end(); ++i)
		{
			for (std::map<int, Cylinder*>::const_iterator j = cylMapCurrent->begin(); j != cylMapCurrent->end(); ++j)
			{
				if (i->first != j->first)
				{
					childCount += (*childArrayCountBuf)[j->first];
				}
				else
				{
					(*childArrayIDsPlaceBuf)[i->first] = childCount;
					break;
				}
			}
			childCount = 0;
		}
		childIDsPlaceArrayVector_.push_back(childArrayIDsPlaceBuf);

		// Ausgabe Child-Place
		//
		//std::cout << std::endl << "Inhalt ChildIDsPlace: " << std::endl;
		//for (std::vector<int>::iterator i = childArrayIDsPlaceBuf.begin(); i != childArrayIDsPlaceBuf.end(); ++i)
		//{
		//	std::cout << z << " | ";
		//	++z;
		//}
		//std::cout << std::endl << "------------------------------------" << std::endl;
		//for (std::vector<int>::iterator i = childArrayIDsPlaceBuf.begin(); i != childArrayIDsPlaceBuf.end(); ++i)
		//{
		//	std::cout << i->second << " | ";
		//}
		//z = 0;
		//std::cout << std::endl;
		//



		//
		// childIDs
		//
		std::vector<int>* childArrayIDsBuf = new std::vector<int> (cylMapCurrent->size()-1, 0);
		int count = 0;
	
		for (std::map<int, Cylinder*>::const_iterator i = cylMapCurrent->begin(); i != cylMapCurrent->end(); ++i)
		{
			for (std::vector<int>::iterator j = parentArrayBuf->begin(); j != parentArrayBuf->end(); ++j)
			{
				if (i->first == *j)
				{
					(*childArrayIDsBuf)[count] = j - parentArrayBuf->begin();// = j->first;
					++count;
				}
			}		
		}
		childIDsArrayVector_.push_back(childArrayIDsBuf);

		//std::cout << std::endl << "Inhalt ChildArrayIDs:" << std::endl;
		//for (std::vector<int>::iterator i = childArrayIDsBuf.begin(); i != childArrayIDsBuf.end(); ++i)
		//{
		//	std::cout << i-> first << " | ";
		//	++z;
		//}
		//std::cout << std::endl << "------------------------------------" << std::endl;
		//for (std::vector<int>::iterator i = childArrayIDsBuf.begin(); i != childArrayIDsBuf.end(); ++i)
		//{
		//	std::cout << i->second << " | ";
		//}
		//z = 0;
		//std::cout << std::endl;
		//



		//
		// thickness
		//
		std::vector<float>* thickArrayBuf = new std::vector<float> (cylMapCurrent->size(), 0);

		for (std::map<int,Cylinder*>::const_iterator i = cylMapCurrent->begin(); i != cylMapCurrent->end(); ++i)
		{
			(*thickArrayBuf)[i->first] = (i->second->radius2_);
		}
		thicknessArrayVector_.push_back(thickArrayBuf);
		//

	}// if calcTreeGrowth_ == 1



	std::vector<int>* parentArray =	parentArrayVector_[iteration_];
	std::vector<int>* childCountArray = childCountArrayVector_[iteration_];
	std::vector<int>* childIDsPlaceArray = childIDsPlaceArrayVector_[iteration_];
	std::vector<int>* childIDsArray = childIDsArrayVector_[iteration_];
	std::vector<float>*	thicknessArray= thicknessArrayVector_[iteration_];
	int	root = rootVector_[iteration_];


	//root
	int parLoc = glGetUniformLocation(applyRotProgram_->getID(), "wurzelID");
	applyRotProgram_->bind();
		glUniform1i(parLoc, root);
	applyRotProgram_->unbind();

	parLoc = glGetUniformLocation(calcRotProgram_->getID(), "wurzelID");
	calcRotProgram_->bind();
		glUniform1i(parLoc, root);
	calcRotProgram_->unbind();



	//parent
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_BUFFER, parentTex_);
	glBindBuffer(GL_TEXTURE_BUFFER, parentBuf_);
	glBufferData(GL_TEXTURE_BUFFER, parentArray->size()*sizeof(int), &((*parentArray)[0]), GL_DYNAMIC_COPY);

	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, parentBuf_);


	//childCount
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_BUFFER, childCountTex_);
	glBindBuffer(GL_TEXTURE_BUFFER, childCountBuf_);
	glBufferData(GL_TEXTURE_BUFFER, childCountArray->size()*sizeof(int), &((*childCountArray)[0]), GL_DYNAMIC_COPY);

	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, childCountBuf_);


	//childPlace
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_BUFFER, childIDsPlaceTex_);
	glBindBuffer(GL_TEXTURE_BUFFER, childIDsPlaceBuf_);
	glBufferData(GL_TEXTURE_BUFFER, childIDsPlaceArray->size()*sizeof(int), &((*childIDsPlaceArray)[0]), GL_DYNAMIC_COPY);

	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, childIDsPlaceBuf_);


	//childIDs
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_BUFFER, childIDsTex_);
	glBindBuffer(GL_TEXTURE_BUFFER, childIDsBuf_);
	glBufferData(GL_TEXTURE_BUFFER, childIDsArray->size()*sizeof(int), &((*childIDsArray)[0]), GL_DYNAMIC_COPY);

	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, childIDsBuf_);
		


	//
	// ki
	//
	std::vector<float> kiArrayBuf(vertexCount_, 0.0);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_BUFFER, kiTexTF_[0]);
	glBindBuffer(GL_TEXTURE_BUFFER, kiBufTF_[0]);
	glBufferData(GL_TEXTURE_BUFFER, kiArrayBuf.size()*sizeof(float), &(kiArrayBuf[0]), GL_DYNAMIC_COPY);

	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, kiBufTF_[0]);


	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_BUFFER, kiTexTF_[1]);
	glBindBuffer(GL_TEXTURE_BUFFER, kiBufTF_[1]);
	glBufferData(GL_TEXTURE_BUFFER, kiArrayBuf.size()*sizeof(float), &(kiArrayBuf[0]), GL_DYNAMIC_COPY);

	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, kiBufTF_[1]);
	//


	//thickness
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_BUFFER, thickTex_);
	glBindBuffer(GL_TEXTURE_BUFFER, thickBuf_);
	glBufferData(GL_TEXTURE_BUFFER, thicknessArray->size()*sizeof(float), &((*thicknessArray)[0]), GL_STATIC_DRAW);

	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, thickBuf_);
	//



}
//

void OpenGLQtContext::make_teller()
{
	Cylinder c(glm::vec4(0,-2,0,1),glm::vec4(0,0.1,0,1),glm::vec4(0,1,0,0),glm::vec4(0,1,0,0),13.0f,13.0f,90);

	std::vector<float> vertexArray;
	std::vector<float> texArray;
	std::vector<unsigned int> indexArray;

	std::vector<float> vertexArray_kreis;
	std::vector<float> texArray_kreis;
	std::vector<unsigned int> indexArray_kreis;

	vertexArray=  c.vertex_;
	indexArray = c.indexArray_;
	texArray = c.tex_;
	indexTellerCount_ = indexArray.size();

	glActiveTexture(GL_TEXTURE13);
	glBindTexture(GL_TEXTURE_2D, bodenTex_);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 395, 230, 0, GL_RGB, GL_FLOAT, tellerdata_);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );



	glGenVertexArrays(1,&tellerVAO_);
	glGenBuffers(1, &teller_ver_Buf_);
	glGenBuffers(1, &teller_tex_Buf_);
	glGenBuffers(1,&teller_ind_Buf_);

	glBindVertexArray(tellerVAO_);
		
		glBindBuffer(GL_ARRAY_BUFFER,teller_ver_Buf_);
		glBufferData(GL_ARRAY_BUFFER,sizeof(float)*vertexArray.size(),&(vertexArray[0]),GL_STATIC_DRAW);
		glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,0,0);
		glEnableVertexAttribArray(0);
	
		glBindBuffer(GL_ARRAY_BUFFER,teller_tex_Buf_);
		glBufferData(GL_ARRAY_BUFFER,sizeof(float)*texArray.size(),&(texArray[0]),GL_STATIC_DRAW);
		glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,0);
		glEnableVertexAttribArray(1);
	
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,teller_ind_Buf_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(float)*indexArray.size(),&(indexArray[0]),GL_STATIC_DRAW);

	glBindVertexArray(0);


	glm::vec4 kreispunkt2 = glm::vec4(c.radius2_,0,c.radius2_,1);
	
	glm::vec4 nach = c.nach_;
	glm::vec4 nach2=-nach, bildpunkt;
	float angle = 360.0f/90.0f;

	for(int j = 0; j < 90; ++j)
	{
		glm::vec3 kreispunkt_temp(kreispunkt2.x,kreispunkt2.y,kreispunkt2.z);

		kreispunkt_temp=glm::rotate(kreispunkt_temp,angle,glm::vec3(c.richtung1_.x,c.richtung1_.y,c.richtung1_.z));
			
		kreispunkt2.x=kreispunkt_temp.x;
		kreispunkt2.y=kreispunkt_temp.y;
		kreispunkt2.z=kreispunkt_temp.z;

		bildpunkt=kreispunkt2-nach2;
			
		vertexArray_kreis.push_back(bildpunkt.x);
		vertexArray_kreis.push_back(bildpunkt.y);
		vertexArray_kreis.push_back(bildpunkt.z);
		vertexArray_kreis.push_back(1);

		bildpunkt = glm::normalize(bildpunkt);
		texArray_kreis.push_back(bildpunkt.x);
		texArray_kreis.push_back(bildpunkt.z);
	}

		vertexArray_kreis.push_back(nach.x);
		vertexArray_kreis.push_back(nach.y);
		vertexArray_kreis.push_back(nach.z);
		vertexArray_kreis.push_back(1);


	for(int i = 0; i < 90 ; ++i)
	{
		indexArray_kreis.push_back(i);
		indexArray_kreis.push_back(90);
		indexArray_kreis.push_back(i+1);
	}
		indexArray_kreis.push_back(89);
		indexArray_kreis.push_back(90);
		indexArray_kreis.push_back(0);

	indexKreisCount_ = indexArray_kreis.size();

	glActiveTexture(GL_TEXTURE14);
	glBindTexture(GL_TEXTURE_2D, grasTex_);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 600, 450, 0, GL_RGB, GL_FLOAT, grasdata_);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );


	glGenVertexArrays(1,&kreisVAO_);
	glGenBuffers(1, &kreis_ver_Buf_);
	glGenBuffers(1, &kreis_tex_Buf_);
	glGenBuffers(1,&kreis_ind_Buf_);

	glBindVertexArray(kreisVAO_);
	
		glBindBuffer(GL_ARRAY_BUFFER,kreis_ver_Buf_);
		glBufferData(GL_ARRAY_BUFFER,sizeof(float)*vertexArray_kreis.size(),&(vertexArray_kreis[0]),GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
	
		glBindBuffer(GL_ARRAY_BUFFER,kreis_tex_Buf_);
		glBufferData(GL_ARRAY_BUFFER,sizeof(float)*texArray_kreis.size(),&(texArray_kreis[0]),GL_STATIC_DRAW);
		glVertexAttribPointer(1 ,2 ,GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,kreis_ind_Buf_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(float)*indexArray_kreis.size(),&(indexArray_kreis[0]),GL_STATIC_DRAW);

	glBindVertexArray(0);
}


void OpenGLQtContext::killTree(std::string new_file)
{
	std::cout << "QGL: Baum wird geloescht...\n";
	filename_ = new_file;
	calcTreeGrowth_= 1;
	currentTree_.kill_tree();
	iteration_ = 0;
	TreeHugger_.erase_old();
	TreeHugger_.parse_lsystem(filename_.c_str(),5);
	currentTree_=tree(2,TreeHugger_.get_angle(),TreeHugger_.get_length());
	currentTreeString_=TreeHugger_.get_axiom();

    vertexArrayVector_ =std::vector<std::vector<float> >();
    indexArrayVector_ =std::vector<std::vector<unsigned int> >();
    dirArrayVector_ =std::vector<std::vector<float> >();
    radArrayVector_ =std::vector<std::vector<float> >();


	for (std::vector<std::vector<int>* >::iterator i = parentArrayVector_.begin(); i != parentArrayVector_.end(); ++i)
		delete *i;
	for (std::vector<std::vector<int>* >::iterator i = childCountArrayVector_.begin(); i != childCountArrayVector_.end(); ++i)
		delete *i;
	for (std::vector<std::vector<int>* >::iterator i = childIDsPlaceArrayVector_.begin(); i != childIDsPlaceArrayVector_.end(); ++i)
		delete *i;
	for (std::vector<std::vector<int>* >::iterator i = childIDsArrayVector_.begin(); i != childIDsArrayVector_.end(); ++i)
		delete *i;
	for (std::vector<std::vector<float>* >::iterator i = thicknessArrayVector_.begin(); i != thicknessArrayVector_.end(); ++i)
		delete *i;
	parentArrayVector_ = std::vector<std::vector<int>* > ();
	childCountArrayVector_ = std::vector<std::vector<int>* > ();
	childIDsPlaceArrayVector_ = std::vector<std::vector<int>* >	();
	childIDsArrayVector_ = std::vector<std::vector<int>* > ();
	thicknessArrayVector_ = std::vector<std::vector<float>* > ();
	rootVector_ = std::vector<unsigned int> ();
}


void OpenGLQtContext::setIteration(int new_iteration_)
{
	if(new_iteration_ == 0)
	{
		currentTreeString_= TreeHugger_.get_axiom();
		maxIteration_ = new_iteration_;
		iteration_ = new_iteration_;
		initTree(currentTreeString_);

	}
	else
	{
		if(new_iteration_ < maxIteration_)
		{
			calcTreeGrowth_ = 0;
		}
		else
		{
			calcTreeGrowth_ = 1;
			maxIteration_ = new_iteration_;
		}

		iteration_ = new_iteration_;
		currentTreeString_= TreeHugger_.parse_lsystem(filename_.c_str(), iteration_);
		
		initTree(currentTreeString_);
	}	

}

int OpenGLQtContext::getIteration(void)	const
{
	return iteration_;

}



void OpenGLQtContext::makeWind()
{
	windCount_ = 500;
}
