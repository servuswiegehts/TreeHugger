#ifndef __TREE_CLASS__
#define __TREE_CLASS__

#define PI 3.141592654

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <string>
#include <map>
#include "cylinder.h"
#include <cmath>
#include <sstream>
#include <stack>



struct Daten
{
		glm::vec4									von_temp;
		glm::vec4									nach_temp;
		glm::vec4									richtung_temp;
		glm::vec4									richtung_alt_temp;
};





class tree {

	public:

//--------------------------------C'tor D'tor---------------------------------------------

		tree										();
		tree										( int , float , float );
		~tree										();

//----------------------------------------------------------------------------------------

	
		void										drawTree( std::string );
		void										set();
		void										flush();
		void										killTree();
		std::string									cylinderID( glm::vec4 , glm::vec4 );
		std::string									vertexID( glm::vec4 );
		
		

//--------------------------------------Getter--------------------------------------------

		int											getNumVertices()		const;
		int											getIndeces()			const;
		int											getNumCylinders()		const;
		std::vector<float>							getVertices()			const;
		std::vector<float>							getColors()				const;
		std::vector<float>							getTexture()			const;
		std::vector<unsigned int>					getIndices()			const;
		std::vector<float>							getRadien()				const;
		std::vector<float>							getDirection()			const;
		std::map<int, Cylinder*> const*				getCylinder()			;

//-----------------------------------------------------------------------------------------


//----------------------------------------Setter-------------------------------------------

		void										setLength( float const& );
		void										setAngle(  float const& );

//-----------------------------------------------------------------------------------------

//--------------------------------------Operatoren-----------------------------------------
		
		tree										operator=(  tree const& );	
//----------------------------------------------------------------------------------------

//--------------------------------------Member Variablen-----------------------------------

	private:
		int											itera;
		int											vertex_;
		int											num_;
		int											flag_;
		int											flags_[6];
		float										alpha_;
		float										radius_;
		float										radius2_;
		float										height_;
		std::vector<float>							vertexArray_;
		std::vector<float>							radArray_;
		std::vector<float>							dirArray_;
		std::vector<float>							colorArray_;
		std::vector<float>							textureArray_;
		std::vector<unsigned int>					indexArray_;
		std::map<std::string,Cylinder>				oldcylinder_;
		std::map<std::string,RadDir>				oldradius_;
		std::map<std::string,Cylinder>::iterator	it_1_;
		std::map<std::string,RadDir>::iterator		it_2_;

//-----------------------------------------------------------------------------------------


};






#endif