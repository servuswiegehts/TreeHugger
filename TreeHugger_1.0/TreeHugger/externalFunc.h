#ifndef __EXTERNAL_FUNC_H__
#define __EXTERNAL_FUNC_H__

#include <iostream>
#include <string>

#include <GL/glew.h>
#include "IL/il.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>


//
// Funktion zum Laden von Texturen
//
float* loadTex ( std::string imageName, unsigned int imgWidth, unsigned int imgHeight);
//


//
// Funktion zum Testen auf OpenGL-Fehler
//
// Mit einem Debug-Context (Opengl 4.0) überflüssig
//
bool	checkForError (std::string const& command);
//


//
// Überladener Operator zur Ausgabe von Matrizen
//
std::ostream& operator<<(std::ostream& stream, glm::mat4 const& mat);
//

#endif // EXTERNAL_FUNC__