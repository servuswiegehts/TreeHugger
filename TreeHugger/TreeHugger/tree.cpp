#include "tree.h"




//--------------------------------C'tor D'tor----------------------------
tree::tree():

	alpha_			((10*PI)/180),
	height_			(3),
	num_			(2),
	flag_			(0),
	flags_			(),
	radius_			(1),
	radius2_		(1),
	itera			(0)

{}


tree::tree( int num , float alpha , float laenge ):

	num_			(num),
	flag_			(0),
	flags_			(),
	alpha_			((alpha*PI)/180),
	radius_			(0.1),
	radius2_		(0.1),
	height_			(laenge),
	itera			(0)

{}
	

tree::~tree()
{}

//-----------------------------------------------------------------------

//Erstellt aus dem übergebenen String einen Baum 
void tree::drawTree(std::string a)
{

	Cylinder cyl , temp;

	float radius_temp1 , radius_temp2;
	glm::vec4 richtung( 0 , height_ , 0 , 0 );
	glm::vec4 richtung_alt( 0 , height_ , 0 , 0 );

	glm::vec4 von(0,0,0,1);
	glm::vec4 nach(0,0,0,1);
	glm::vec4 von_temp(0,0,0,1);
	glm::vec4 nach_temp(0,0,0,1);
	glm::vec4 richtung_temp(0,0,0,1);
	glm::vec4 richtung_alt_temp(0,0,0,1);
	glm::vec4 richtung_norm(0,0,0,1);

	glm::vec4 richtung_t1;
	glm::vec4 richtung_ta1;


	std::map<std::string,RadDir>::iterator rad1;
	std::map<std::string,RadDir>::iterator rad2;
	
	RadDir t1;
	t1.rad=0.1;
	t1.dir= glm::vec4( 0 , height_ , 0 , 0 );


	oldradius_.insert( std::pair<std::string,RadDir> ( "RADIUS" , t1 ) );


	std::string ID_1,ID_2,id1,id2;
	bool in_cyl=0, in_rad=0,flag_1=1;

	Daten data;
	std::stack<Daten> Datas;
	int daten_counter=-1;
	++itera;
	
	bool first=0;

	for(int i=0;i<a.length();++i)
	{

		float x,y,z;

		switch(a[i])
		{
			case 'F':

				flags_[0]=0;
				flags_[1]=0;
				flags_[2]=0;
				flags_[3]=0;
				flags_[4]=0;
				flags_[5]=0;

				
				
				nach+=richtung;
				nach.w=1;
				
						
				ID_1=cylinderID( von , glm::normalize( richtung ) );
				ID_2=vertexID( von );

				it_1_=oldcylinder_.find( ID_1 );
				it_2_=oldradius_.find( ID_2 );

				if( it_1_ != oldcylinder_.end() )
					in_cyl=1;

				if( it_2_ != oldradius_.end() )
					in_rad=1;

				if( in_cyl )
				{
						temp = it_1_ -> second;

						std::string p1 = vertexID( temp.getVon() );
						std::string p2 = vertexID( temp.getNach() );
						
						rad1 = oldradius_.find( p1 );
						rad2 = oldradius_.find( p2 );
						
						if( first == 0 )
						{
							rad1 -> second.rad += 0.18;
							rad2 -> second.rad += 0.1;
							first = 1;
						}
						it_1_ -> second.setRad_1_Pointer( rad1 );
						it_1_ -> second.setRad_2_Pointer( rad2 );
						rad1 -> second.rad += 0.005;
					
				}
				else if ( !in_cyl && in_rad )
				{
								
					rad1 = oldradius_.find( ID_2 );
					rad2 = oldradius_.find( "RADIUS" );
					
								
					if( flag_ == 0 )
					{
						cyl = Cylinder( von , nach , rad1 -> second.dir , richtung , rad1 , rad2 , num_ );
						richtung_norm = richtung;
					}
					else
					{
						cyl = Cylinder( von , nach , richtung , richtung , rad1 , rad2 , num_ );
						richtung_norm = richtung;
					}

					id1 = cylinderID( cyl.getVon() , glm::normalize( richtung_norm ) );
					oldcylinder_.insert( std::pair<std::string , Cylinder>( id1 , cyl ) );

					id1 = vertexID( cyl.getVon() );
					id2 = vertexID( cyl.getNach() );
					
					it_2_ = oldradius_.find( id1 );
											
					it_2_ = oldradius_.find( id2 );

					RadDir v1;
					v1.rad = rad2 -> second.rad;
					v1.dir = richtung;

					if( it_2_ == oldradius_.end() )
								oldradius_.insert( std::pair<std::string , RadDir>( id2 , v1 ) );
					else
					{
						it_2_ -> second.rad = rad2 -> second.rad;
						it_2_ -> second.dir = rad2 -> second.dir;
					}
														
				}
				else
				{

					std::map<std::string , float>::iterator tmp;

					rad1 = oldradius_.find( "RADIUS" );
					rad2 = oldradius_.find( "RADIUS" );
										
					if( flag_ == 0 )
						cyl=Cylinder( von , nach , richtung , richtung_alt , rad1 , rad2 , num_ );
					else
						cyl=Cylinder( von , nach , richtung , richtung , rad1 , rad2 , num_ );
				
					id1 = vertexID( cyl.getVon() );
					id2 = vertexID( cyl.getNach() );
					ID_1 = cylinderID( cyl.getVon() , glm::normalize( cyl.getRichtung1() ) );

					RadDir v1;
					v1.dir = richtung;
					v1.rad = rad1 -> second.rad;
					
					oldcylinder_.insert( std::pair<std::string , Cylinder>( ID_1 , cyl ) );
					oldradius_.insert( std::pair<std::string , RadDir>( id1 , v1 ) );

					v1.rad = rad2 -> second.rad;
					oldradius_.insert ( std::pair<std::string , RadDir>( id2 , v1 ) );
				
				}
				
				
				von = nach;
			
				richtung *= 0.825;
							
				in_cyl = 0;
				in_rad = 0;
				++flag_;
								
				break;


			case 'f':

				nach += richtung;
				nach[3] = 1;
				von = nach;
			
				break;

			case'+':

				if( flags_[0] == 0 )
					richtung_alt += richtung;

				x = richtung.x;
				richtung.x = ( cos( alpha_ ) * x ) + ( - ( sin( alpha_ ) ) * richtung.y );
				richtung.y = ( sin( alpha_ ) * x) + ( cos( alpha_ ) * richtung.y );
				flag_ = 0;
				++flags_[0];
				
				break;

			case'-':
				
				if( flags_[1] == 0 )
					richtung_alt += richtung;

				x = richtung.x;
				richtung.x = ( cos( -alpha_ ) * x ) + ( -( sin( -alpha_ ) ) * richtung.y );
				richtung.y = ( sin( -alpha_ ) * x ) + ( cos( -alpha_ ) * richtung.y);
				flag_ = 0;
				++flags_[1];
				
				break;

			case'M':
				
				if( flags_[2] == 0 )
					richtung_alt += richtung;

				x = richtung.x;
				richtung.x = ( cos( -alpha_ ) * richtung.z ) + ( -( sin( alpha_ ) ) * x );
				richtung.z = ( sin( -alpha_ ) * richtung.z ) + ( cos( alpha_ ) * x );
				flag_ = 0;
				++flags_[2];
				
				break;

			case'N':
				
				if( flags_[3] == 0 )
					richtung_alt+=richtung;
					
				x = richtung.x;
				richtung.x = ( cos( -alpha_ ) ) * x + ( -( sin( -alpha_ ) ) * richtung.z );
				richtung.z = ( sin( -alpha_ ) * x ) + ( cos( -alpha_ ) * richtung.z );
				flag_ = 0;
				++flags_[3];
				
				break;

			case'&':
				
				if( flags_[4] == 0 )
					richtung_alt += richtung;

				y = richtung.y;
				richtung.y = ( cos( -alpha_ ) * y ) + ( -( sin( -alpha_ ) ) * richtung.z );
				richtung.z = ( sin( -alpha_ ) * y ) + ( cos( -alpha_ ) * richtung.z );
				flag_ = 0;
				++flags_[4];
				
				break;
			
			case'^':
				if( flags_[5] == 0 )
					richtung_alt += richtung;
				
				y = richtung.y;
				richtung.y = ( cos( alpha_ ) * y ) + ( -( sin( alpha_ ) ) * richtung.z );
				richtung.z = ( sin( alpha_ ) * y ) + ( cos( alpha_ ) * richtung.z );
				flag_ = 0;
				++flags_[5];
				
				break;

			case'|':
				richtung *= (-1);

				break;

			case '[':
				
				data.von_temp = von;
				data.nach_temp = nach;
				data.richtung_temp = richtung;
				data.richtung_alt_temp = richtung_alt;
				Datas.push(data);
				++daten_counter;


				break;

			case ']':
				von = Datas.top().von_temp;
				nach = Datas.top().nach_temp;
				richtung = Datas.top().richtung_temp;
				richtung_alt = Datas.top().richtung_alt_temp;
				Datas.pop();
				--daten_counter;
				
				break;

			default:
				std::cout<<"Unbekanntes Zeichen:"<<a[i]<<std::endl;
				
				break;
		}

	}
	
}

//Übersetzt die Radienzeiger in Float-Werte
void tree::set()
{
	int count = 0;
	Cylinder cyl;
	int v;
	for( std::map<std::string,Cylinder>::iterator k = oldcylinder_.begin() ; k != oldcylinder_.end() ; ++k )
	{
		cyl = k -> second;
			cyl.set_vertex();
			radArray_.push_back( cyl.getRadius1() );
			radArray_.push_back( cyl.getRadius2() );

			k -> second.setRadius1( cyl.getRadius1() );
			k -> second.setRadius2( cyl.getRadius2() );

				for ( int j = 0 ; j < 2*4 ; ++j )
				{
					vertexArray_.push_back( cyl.getVertexVector()[j] );
					colorArray_.push_back( cyl.getColorVector()[j] );
				}
			
				v = count * 2;
				for ( int k = 0 ; k < (cyl.getIndexArray().size() ) ; ++k )
				{
					indexArray_.push_back( cyl.getIndexArray()[k] + v );
				}
				++count;				
				vertex_ = indexArray_.size();

				dirArray_.push_back( cyl.getRichtung1().x );
				dirArray_.push_back( cyl.getRichtung1().y );
				dirArray_.push_back( cyl.getRichtung1().z );
				dirArray_.push_back( 0 );
				dirArray_.push_back( cyl.getRichtung2().x );
				dirArray_.push_back( cyl.getRichtung2().y );
				dirArray_.push_back( cyl.getRichtung2().z );
				dirArray_.push_back( 0 );
	}
}

//Leert die Tree-Vektoren 
void  tree::flush()
{
	vertexArray_	= std::vector<float>();
	radArray_		= std::vector<float>();
	colorArray_		= std::vector<float>();
	textureArray_	= std::vector<float>();
	indexArray_		= std::vector<unsigned int>();
	dirArray_		= std::vector<float>();
}

//Setzt den gesamten Baum auf Anfang zurück
void  tree::killTree()
{
	vertexArray_	= std::vector<float>();
	colorArray_		= std::vector<float>();
	textureArray_	= std::vector<float>();
	indexArray_		= std::vector<unsigned int>();
	oldcylinder_	= std::map<std::string,Cylinder>();
	oldradius_		= std::map<std::string,RadDir>();
	dirArray_		= std::vector<float>();
	radArray_		= std::vector<float>();
	radius_			= 0.1;
	radius2_		= 0.1;

}

//Erstellt aus Startpunkt und Richtung eine eindeutige Zylinder-ID
std::string  tree::cylinderID(glm::vec4 von, glm::vec4 nach)
{
	int a_1 = ( unsigned int ) ( von.x * 100000 );
	int b_1 = ( unsigned int ) ( von.y * 100000 );
	int c_1 = ( unsigned int ) ( von.z * 100000 );

	int a_2 = ( unsigned int ) ( nach.x * 100000 );
	int b_2 = ( unsigned int ) ( nach.y * 100000 );
	int c_2 = ( unsigned int ) ( nach.z * 100000 );

	std::stringstream ss ( std::stringstream::in | std::stringstream::out );
	
	ss << a_1;
	ss << b_1;
	ss << c_1;
	ss << a_2;
	ss << b_2;
	ss << c_2;

	std::string back;

	ss >> back;

	return back;

}

//Erstellt aus einem Punkt eine eindeutige Vertex-ID
std::string  tree::vertexID(glm::vec4 nach)
{
	
	int a_2 = ( unsigned int ) ( nach.x * 100000 );
	int b_2 = ( unsigned int ) ( nach.y * 100000 );
	int c_2 = ( unsigned int ) ( nach.z * 100000 );

	std::stringstream ss ( std::stringstream::in | std::stringstream::out );
	
	ss << a_2;
	ss << b_2;
	ss << c_2;

	std::string back;

	ss >> back;

	return back;

}


//-------------------------------Getter----------------------------------

int  tree::getNumVertices() const 
{
	return vertex_;
}


std::vector<unsigned int> tree::getIndices() const
{
		return indexArray_;
}


int tree::getNumCylinders() const
{
	return oldcylinder_.size();
}


std::vector<float> tree::getVertices() const
{
		return vertexArray_;
}


std::vector<float> tree::getColors() const
{
		return colorArray_;
}


std::vector<float> tree::getTexture() const
{
		return textureArray_;
}


int tree::getIndeces() const
{
	return indexArray_.size();
}


std::vector<float> tree::getRadien() const
{
		return radArray_;
}


std::vector<float> tree::getDirection() const
{
		return dirArray_;
}


std::map<int, Cylinder*>const* tree::getCylinder() 
{
	int zaehler = 0;
	std::map<int , Cylinder*>* cylmap = new std::map<int , Cylinder*>();
	for(std::map<std::string , Cylinder>::iterator i = oldcylinder_.begin() ; i != oldcylinder_.end(); ++i )
	{
		cylmap -> insert( std::pair<int , Cylinder*>( zaehler , &( i -> second ) ) );
		++zaehler;
	}

	return cylmap;
}

//-----------------------------------------------------------------------

//---------------------------Setter--------------------------------------


void tree::setLength(float const& length_new)
{
	height_ = length_new;
}


void tree::setAngle(float const& angle_new)
{
	alpha_ = ( angle_new * PI ) / 180;
}

//-----------------------------------------------------------------------

//---------------------------Operatoren----------------------------------

tree tree::operator = ( tree const& other)
{
	

	this -> alpha_	 = other.alpha_;
	this -> height_	 = other.height_;
	this -> num_	 = other.num_;
	this -> radius_	 = other.radius_;
	this -> radius2_ = other.radius2_;
	this -> flag_	 = other.flag_;

	return *this;

}

//-----------------------------------------------------------------------