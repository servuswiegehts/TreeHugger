#include "externalFunc.h"

//
// loadTex
//
float* loadTex ( std::string imageName, unsigned int imgWidth, unsigned int imgHeight)
{
	ilInit();

	std::string type ("abc");
	int j = 0;
	for (std::string::iterator i = imageName.end()-3; i != imageName.end(); ++i )
	{
		type[j++] = *i;
	}

	unsigned char * imageData;
	float * imageDataf = 0;

	if (type == "raw")
	{
		FILE *fp;
		imageDataf = new float [imgWidth*imgHeight*3];		
		fp = fopen(imageName.c_str(), "rb");

		imageData = new unsigned char [imgWidth*imgHeight];
	
		fread(imageData, 1, imgWidth * imgHeight*3, fp);

		fclose(fp);
	
		for (unsigned int i = 0; i < 3*imgWidth*imgHeight; ++i )
		{
			imageDataf[i] = imageData[i]/255.0f;
		}
		delete imageData;
	}
	else
	{
		ILuint imageID;
		ilGenImages(1,&imageID);
		ilBindImage(imageID);
		ILboolean ret = ilLoadImage ( imageName.c_str() );

		if( !ret ) 
		{
			ilDeleteImages(1,&imageID);
			std::cout << "Fehler beim Laden einer Bild-Datei: "<< imageName << std::endl;
			return NULL;
		}
		imageData = ilGetData();

		if( imageData == NULL ) 
		{
			ilDeleteImages(1,&imageID);
			std::cout << "Fehler beim Laden der Bild-Daten von: "<< imageName << std::endl;
			return NULL;
		}
		else 
		{
			unsigned width		= ilGetInteger( IL_IMAGE_WIDTH );
			unsigned height		= ilGetInteger( IL_IMAGE_HEIGHT );	
			unsigned components = ilGetInteger( IL_IMAGE_BYTES_PER_PIXEL );
			unsigned format		= ilGetInteger( IL_IMAGE_FORMAT );	

			imageDataf = new float [imgWidth*imgHeight*components];
			GLint texSize; 
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);
			if( width > (unsigned)texSize || height > (unsigned)texSize ) 
			{
				ilDeleteImages(1,&imageID);
				std::cout << "Fehler, Maße überschreiten Maximum: "<< imageName << std::endl;
				return NULL;
			}
		
			for (unsigned int i = 0; i < width*height*components; ++i )
			{
				imageDataf[i] = float(imageData[i]) / 255.0f;
			}
		}
	}

	return imageDataf;
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

	} while (error != GL_NO_ERROR);

	return res;
}
//


//
// Überladener Operator zur Ausgabe von Matrizen
//
std::ostream& operator<<(std::ostream& stream, glm::mat4 const& mat)
{
	// 
	//for (unsigned int i = 0; i < 4; ++i )
	//{
	//	for (unsigned int j = 0; j < 4; ++j )
	//	{
	//		std::cout << mat[i][j] << " ";
	//	}
	//	std::cout << std::endl;
	//}
	//return stream;

	// XML-Style:
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