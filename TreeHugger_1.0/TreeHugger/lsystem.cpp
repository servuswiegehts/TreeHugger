#include <stdlib.h>
#define  BOOST_SPIRIT_NO_REGEX_LIB

#include "lsystem.hpp"
#include <boost/spirit.hpp>
//#include <boost/spirit/actor.hpp>
//#include <boost/spirit/include/qi.hpp>
//#include <boost/spirit/iterator/file_iterator.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/find.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

Lsystem::Lsystem()
{
}

file_iterator<>
Lsystem::load(char const* filename)
{
    file_iterator<> temp(filename);

    if(!temp)
    {
        std::cout << "LSystem: Unable to open file! \n";
    }
    return temp;
}

string
Lsystem::parse_lsystem(char const* filename, int iteration) {
    //Falls Datei noch nicht geparsed


    if(lsystem_.empty() == true){
        //Datei einlesen
        file_iterator<> first = load(filename);
        file_iterator<> last = first.make_end();

        typedef char char_t;
        typedef file_iterator <char_t> iterator_t;

        string input(first, last);
//        std::cout << "Eingabe:" << "\n" << input << "\n" << std::endl;

        lsystem_grammer lsys;

        //Parsevorgang
        parse_info<iterator_t> pi = parse(first, last, lsys, space_p);
        if (pi.hit) {
            if (pi.full) {
                std::cout << "LSystem: file reading successfully" << std::endl;
                std::cout << "LSystem: " << pi.length << "characters read" << std::endl;
				std::cout << "LSystem: parsing ..." << std::endl;
//                std::cout << "Vektor:" << gl_lsys_file.size() << "\n" << std::endl;

                //Eingabe splitten
                typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
                //Axiom
                //string s1 = get_axiom();
                //boost::char_separator<char> sep1(": ");
                //tokenizer tok(s1, sep1);
                //tokenizer::iterator it = tok.begin();
                //string start = *(++it);
                boost::char_separator<char> sep1(": ");
				string start = get_axiom();
				std::string  ret;

                //Produkion(en)
                vector<string> s2 = get_production();
               // 
                    int last_pos = 0;
                    int left = 1;

                    //Zufallszahlen bis 100
                   // srand(j * time(0));
                    //int prob = rand() % 100 + 1;
                //  std::cout << "ZZ:" << prob << std::endl;
				//  std::cout<<s2.size()<<std::endl;

				std::map<int, string> mapProbToProd;
				
				int zaehler=0;

				for (int i = 0; i < s2.size(); i++) 
				{

					int position;

                    tokenizer tok2(s2[i], sep1);
                    tokenizer::iterator it = tok2.begin();
                    string temp = *(++it);                                                              //Alles nach dem Doppelpunkt

                        boost::char_separator<char> sep2("=");
                        tokenizer tok3(temp, sep2);
                        tokenizer::iterator it2 = tok3.begin();
                        string temp1 = *(it2);                                                              //Alles vor dem Gleichheitszeichen
                        string temp2 = *(++it2);                                                            //Alles nach dem Gleichheitszeichen

                        if (temp2.find(",") == string::npos) 
						{                                              //DOL
                            stochastic_ = false;
                            //Ersetzung
                            boost::replace_all(start, temp1, temp2);
							lsystem_.push_back(start);
							lsystem_.push_back(start);                                                      //DOL mit meherere Produktionen existieren
                        } 
						else if (temp2.find(",") != string::npos) 
						{                                       //stochastische L-Systeme
                            stochastic_ = true;
                            boost::char_separator<char> sep3(",");
                            tokenizer tok4(temp2, sep3);
                            tokenizer::iterator it3 = tok4.begin();
                            string sto = *(it3);                                                            //Alles vor Komma (Wahrscheinlichkeit)
                            string temp3 = *(++it3);                                                        //Alles nach Komma (Regel)

                            int cur_pos = atof(sto.data()) * 100;                                           //aktuelle Wahrscheinlichkeit
                            int right = last_pos + cur_pos; //Bereich rechts
                       
							for(int k = 0; k < cur_pos ; ++k )
							{
								mapProbToProd.insert ( std::pair<int, string>(zaehler,temp3));
								++zaehler;
							}

//                       
                        }
                    }
				for (int j = 1; j <= iteration * 2; j++) 
				{

					std::string result;
				//	result.reserve(14*start.size());

					for(int i = 0 ; i < start.size() ; ++i)
					{
						srand(i*time(0));
						int k = rand() % 100;
						if(start[i]=='F')
							result += mapProbToProd[k];
						else
							result += start[i];
					}

					lsystem_.push_back(result);
					start=result;

				}
				std::cout << "LSystem: parsing finished\n\n";
                //}
                if (stochastic_ == false) {
//                    std::cout << "Ergebnis: " << lsystem_[(s2.size()) * iteration - 1] << std::endl;
                    return lsystem_[(s2.size() * iteration - 1)];
                } else if (stochastic_ == true) {
//                    std::cout << "Ergebnis: " << lsystem_[iteration - 1] << std::endl;
                    return lsystem_[iteration - 1];
                }

            } else {
                std::cout << "LSystem: ERROR parsing data partially" << std::endl;
                std::cout << "LSystem: ERROR " << pi.length << "characters parsed \n\n" << std::endl;
            }
        } else
            std::cout << "LSystem: ERROR parsing failed; stopped at '" << pi.stop << "'\n\n" << std::endl;
    }
    //erneutes Parsen verhindern
    else if(iteration <= lsystem_.size()) 
	{
        if (stochastic_ == false) 
		{
//            std::cout << "Ergebnis: " << lsystem_[(get_production().size()) * iteration - 1] << std::endl;
            return lsystem_[(get_production().size() * iteration - 1)];
        } 
		else if (stochastic_ == true) 
{
//            std::cout << "Ergebnis: " << lsystem_[iteration - 1] << std::endl;
            return lsystem_[iteration - 1];
        }
    }
    //Vektor vergrößern
    else if(iteration > lsystem_.size()){
        erase_old();
        parse_lsystem(filename, iteration);
        if (stochastic_ == false) {
            //            std::cout << "Ergebnis: " << lsystem_[(get_production().size()) * iteration - 1] << std::endl;
            return lsystem_[(get_production().size() * iteration - 1)];
        } else if (stochastic_ == true) {
            //            std::cout << "Ergebnis: " << lsystem_[iteration - 1] << std::endl;
            return lsystem_[iteration - 1];
        }
    }
	return std::string();
}

double
Lsystem::get_length(){
    string l = gl_lsys_file[1].Length;
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(": ");
    tokenizer ltok(l, sep);
    tokenizer::iterator lit = ltok.begin();
    string lv = *(++lit);
    double lvalue = atof(lv.data());                                                                        //Wandelt String in Double um (LÃ¤nge)
    return lvalue;
}

double
Lsystem::get_angle(){
    string a = gl_lsys_file[2].Angle;
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(": ");
    tokenizer atok(a, sep);
    tokenizer::iterator ait = atok.begin();
    string av = *(++ait);
    double avalue = atof(av.data());
    return avalue;
}


string
Lsystem::get_axiom() const{
	string a = gl_lsys_file[3].Axiom;
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(": ");
    tokenizer atok(a, sep);
    tokenizer::iterator ait = atok.begin();
    return *(++ait);
}

vector<string>
Lsystem::get_production() const{
    vector<string> Prod;
    for(int i = 4; i <= gl_lsys_file.size()-1; i++){
        Prod.push_back(gl_lsys_file[i].Production);
    }
    return Prod;
}

void
Lsystem::erase_old(){
    lsystem_ = vector<string>();
	gl_lsys_file=vector<LSTRUCT>();
}