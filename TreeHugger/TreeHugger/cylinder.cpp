#include"cylinder.h"



//------------------------------------C'tor D'tor------------------------
Cylinder::Cylinder():
		von_				(),
		nach_				(),
		numVertices_		(),
		modelMatrix_		(glm::mat4(1.0f)),
		indexArray_			()
		{}

Cylinder::Cylinder(glm::vec4 von, glm::vec4 nach,glm::vec4 richtung,glm::vec4 richtung_alt,float radius1,float radius2,int num):

		von_				(von),
		nach_				(nach),
		numVertices_		(num),
		modelMatrix_		(glm::mat4(1.0f)),
		indexArray_			(6*num,0),
		radius1_			(radius1),
		radius2_			(radius2),
		richtung1_			(richtung),
		richtung2_			(richtung_alt)
		
{
	
		
	
	float angle=360/numVertices_;

	glm::vec4 kreispunkt1, kreispunkt2,bildpunkt,nach2,nach3,von2;

	

	kreispunkt1=glm::vec4(radius1_,0,radius1_,1);


		von2=-von_;

		for(int j=0;j<numVertices_;++j)
		{
			glm::vec3 kreispunkt_temp(kreispunkt1.x,kreispunkt1.y,kreispunkt1.z);

		
			kreispunkt_temp=glm::rotate(kreispunkt_temp,angle,glm::vec3(richtung2_.x,richtung2_.y,richtung2_.z));
			modelMatrix_ = glm::translate(modelMatrix_, glm::vec3(-0.5f, 2.0f, 0.0f));  // Create our model matrix
			modelMatrix_ = glm::scale(modelMatrix_, glm::vec3(0.5f, 0.5f, 0.5f));  // Create our model matrix
			modelMatrix_ = glm::rotate(modelMatrix_, 45.0f, glm::vec3(0.0f, 0.0f, 1.0f));  // Create our model matrix
			kreispunkt1.x=kreispunkt_temp.x;
			kreispunkt1.y=kreispunkt_temp.y;
			kreispunkt1.z=kreispunkt_temp.z;

			bildpunkt=kreispunkt1-von2;
			

			vertex_.push_back(bildpunkt.x);
			vertex_.push_back(bildpunkt.y);
			vertex_.push_back(bildpunkt.z);
			vertex_.push_back(1);
	
			color_.push_back(0.6);
			color_.push_back(0.3);
			color_.push_back(0);
			color_.push_back(1);

			tex_.push_back((float(j%6)/5));
			tex_.push_back(1);

			
	
		}
	


		kreispunkt2=glm::vec4(radius2_,0,radius2_,1);

		nach2=-nach_;
		for(int j=0;j<numVertices_;++j)
		{
			
			glm::vec3 kreispunkt_temp(kreispunkt2.x,kreispunkt2.y,kreispunkt2.z);

		
					kreispunkt_temp=glm::rotate(kreispunkt_temp,angle,glm::vec3(richtung1_.x,richtung1_.y,richtung1_.z));
			
		
			kreispunkt2.x=kreispunkt_temp.x;
			kreispunkt2.y=kreispunkt_temp.y;
			kreispunkt2.z=kreispunkt_temp.z;

			bildpunkt=kreispunkt2-nach2;
			
			vertex_.push_back(bildpunkt.x);
			vertex_.push_back(bildpunkt.y);
			vertex_.push_back(bildpunkt.z);
			vertex_.push_back(1);

			color_.push_back(0.0);
			color_.push_back(0.3);
			color_.push_back(0);
			color_.push_back(1);

			tex_.push_back((float(j%6)/5));
			tex_.push_back(0);
		
	
	
		}
	

		vertex_.push_back(nach_.x);
		vertex_.push_back(nach_.y);
		vertex_.push_back(nach_.z);
		vertex_.push_back(1);

		color_.push_back(0);
		color_.push_back(1);
		color_.push_back(0);
		color_.push_back(1);

	int j=0;
	for(int i=0;i<(numVertices_*3);i+=3)
	{
		indexArray_[i]=j%(numVertices_);
		indexArray_[i+1]=(j+numVertices_);
		indexArray_[i+2]=(j+1)%(numVertices_);
		++j;
	}

	j=0;

	for(int i=(numVertices_*3);i<((numVertices_*3)*2);i=i+3)
	{
		indexArray_[i]=(numVertices_+j)%(numVertices_*2);
		indexArray_[i+1]=(j+1)%(numVertices_+1);
		indexArray_[i+2]=(numVertices_+j+1)%((numVertices_*2));
		++j;
		
	}

		
}


Cylinder::Cylinder(glm::vec4 von, glm::vec4 nach,glm::vec4 richtung,glm::vec4 richtung_alt,std::map<std::string, RadDir>::iterator	rad1,std::map<std::string, RadDir>::iterator	rad2,int num):

		von_				(von),
		nach_				(nach),
		numVertices_		(num),
		modelMatrix_		(glm::mat4(1.0f)),
		indexArray_			(std::vector<unsigned int>(0,0)),
		rad1_				(rad1),
		rad2_				(rad2),
		richtung1_			(richtung),
		richtung2_			(richtung_alt)
		
{}

Cylinder::~Cylinder()
{}

//-----------------------------------------------------------------------


//Übersetzt die übergebenen Radien-Iteratoren in Float-Werte
void Cylinder::set_vertex()
{
		
	float angle=360/numVertices_;

	glm::vec4 kreispunkt1, kreispunkt2,bildpunkt,nach2,nach3,von2;

	radius1_=rad1_->second.rad;
	radius2_=rad2_->second.rad;

	vertex_.push_back(von_.x);
	vertex_.push_back(von_.y);
	vertex_.push_back(von_.z);
	vertex_.push_back(1);
	vertex_.push_back(nach_.x);
	vertex_.push_back(nach_.y);
	vertex_.push_back(nach_.z);
	vertex_.push_back(1);

			color_.push_back(1);
			color_.push_back(1);
			color_.push_back(1);
			color_.push_back(1);
			color_.push_back(1);
			color_.push_back(1);
			color_.push_back(1);
			color_.push_back(1);


			indexArray_.push_back(0);
			indexArray_.push_back(1);
		
		
		


}


//----------------------------Getter-------------------------------------
glm::vec4 Cylinder::getNach() const 
{
	return nach_;
}

glm::vec4 Cylinder::getVon() const 
{
	return von_;
}

glm::vec4 Cylinder::getRichtung1() const 
{
	return richtung1_;		
}

glm::vec4 Cylinder::getRichtung2() const 
{
	return richtung2_;
}

float Cylinder::getRadius1() const 
{
	return radius1_;
}

float Cylinder::getRadius2() const 
{
	return radius2_;
}

int	Cylinder::getNumVertices() const 
{
	return numVertices_;
}

std::vector<float> Cylinder::getVertexVector() const 
{
	return vertex_;
}

std::vector<float> Cylinder::getColorVector() const 
{
	return color_;
}

std::vector<float> Cylinder::getTexVector() const 
{
	return tex_;
}

std::vector<unsigned int> Cylinder::getIndexArray() const 
{
	return indexArray_;
}

//-----------------------------------------------------------------------




//-------------------------------Setter----------------------------------
void Cylinder::setRadius1(float const& to_set)
{
	radius1_=to_set;
}

void Cylinder::setRadius2(float const& to_set)
{
	radius2_=to_set;
}

void Cylinder::setRad_1_Pointer(std::map<std::string, RadDir>::iterator const& to_set)
{
	rad1_ = to_set;
}

void Cylinder::setRad_2_Pointer(std::map<std::string, RadDir>::iterator const& to_set)
{
	rad2_ = to_set;
}

//-----------------------------------------------------------------------