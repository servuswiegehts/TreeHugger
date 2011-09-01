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

//-----------------------C'tor D'tor -----------------------------------------
	
	Cylinder();
	Cylinder( glm::vec4 , glm::vec4 , glm::vec4 , glm::vec4 , float , float , int );
	Cylinder( glm::vec4 , glm::vec4 , glm::vec4 , glm::vec4 , std::map<std::string,RadDir>::iterator , std::map<std::string,RadDir>::iterator , int );
	~Cylinder();

//-----------------------------------------------------------------------------
	
	
	void											set_vertex();

//-------------------------Getter----------------------------------------------

	glm::vec4 										getNach()				const;			
	glm::vec4										getVon()				const;
	glm::vec4										getRichtung1()			const;
	glm::vec4										getRichtung2()			const;
	float											getRadius1()			const;
	float											getRadius2()			const;
	int												getNumVertices()		const;
	std::vector<float>								getVertexVector()		const;
	std::vector<float>								getColorVector()		const;
	std::vector<float>								getTexVector()			const;
	std::vector<unsigned int>						getIndexArray()			const;

//-----------------------------------------------------------------------------

//-------------------------Setter----------------------------------------------
    
	void											setRadius1( float const& );
	void											setRadius2( float const& );
	void											setRad_1_Pointer( std::map<std::string , RadDir>::iterator const& );
	void											setRad_2_Pointer( std::map<std::string , RadDir>::iterator const& );

//-----------------------------------------------------------------------------


//-------------------------Member Variblen-------------------------------------
private:

	glm::vec4										nach_;
	glm::vec4										von_;
	glm::vec4										richtung1_;
	glm::vec4										richtung2_;
	float											radius1_;
	float											radius2_;
	int												numVertices_;
	glm::mat4										modelMatrix_;
	std::vector<float>								vertex_;
	std::vector<float>								color_;
	std::vector<float>								tex_;
	std::vector<unsigned int>						indexArray_;
	std::map<std::string, RadDir>::iterator			rad1_;
	std::map<std::string, RadDir>::iterator			rad2_;

//-----------------------------------------------------------------------------



};


#endif