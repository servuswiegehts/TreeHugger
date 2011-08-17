#ifndef __CYLINDER_CLASS__
#define __CYLINDER_CLASS__
#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <cmath>
#include<cstdlib>
#include <vector>
#include <map>

struct RadDir
{

		glm::vec4									dir;
		float										rad;

};



class Cylinder
{
public:
	Cylinder();
	Cylinder(glm::vec4, glm::vec4,glm::vec4,glm::vec4,float,float,int);
	Cylinder(glm::vec4, glm::vec4,glm::vec4,glm::vec4,std::map<std::string,RadDir>::iterator,std::map<std::string,RadDir>::iterator,int);

	void set_vertex();




//private:

	glm::vec4					nach_;
	glm::vec4					von_;
	glm::vec4					richtung1_;
	glm::vec4					richtung2_;
	float						radius1_;
	float						radius2_;
	int							numVertices_;
	glm::mat4					modelMatrix_;
	std::vector<float>			vertex_;
	std::vector<float>			color_;
	std::vector<float>			tex_;
	std::vector<unsigned int>	indexArray_;
	std::map<std::string, RadDir>::iterator	rad1_;
	std::map<std::string, RadDir>::iterator	rad2_;




};


#endif