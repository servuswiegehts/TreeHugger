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

		tree										(void);
		tree										(int,float,float);
		
		int											get_num_vertices();
		int											get_indeces();
		void										drawTree(std::string);
		void										set();
		void										flush();
		void										kill_tree();
		void										set_length(float);
		void										set_angle(float);
		void										make_leaf(glm::vec4,glm::vec4,int);
		int											get_num_cylinders(void);
		int											get_leafs();
		std::string									cylinder_ID(glm::vec4, glm::vec4);
		std::string									vertex_ID(glm::vec4);
		std::vector<float>							get_vertices(void);
		std::vector<float>							get_colors(void);
		std::vector<float>							get_texture(void);
		std::vector<unsigned int>					get_indices(void);
		std::vector<float>							get_radien();
		std::vector<float>							get_dir();
		tree										operator=(const tree &);	
		std::map<int, Cylinder*> const*					getCylinder();
		
		



//	private:
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
		std::vector<float>							leaf_Vertex_Array_;
		std::vector<float>							leaf_Color_Array_;
		std::vector<unsigned int>					leaf_Index_Array_;
		std::vector<float>							leaf_Texture_Array_;
		std::vector<float>							colorArray_;
		std::vector<float>							textureArray_;
		std::vector<unsigned int>					indexArray_;
		std::map<std::string,Cylinder>				oldcylinder_;
		std::map<std::string,RadDir>				oldradius_;
		std::map<std::string,Cylinder>::iterator	it_1_;
		std::map<std::string,RadDir>::iterator		it_2_;

};






#endif