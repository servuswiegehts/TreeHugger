#ifndef _LSYSTEM_HPP
#define	_LSYSTEM_HPP

#include <stdlib.h>
#define  BOOST_SPIRIT_NO_REGEX_LIB

#include "lsystem.hpp"
#include <boost/spirit.hpp>
//#include <boost/spirit/actor.hpp>
//#include <boost/spirit/include/qi.hpp>
//#include <boost/spirit/iterator/file_iterator.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

using namespace std;
using namespace boost::spirit;

enum pos {
    _name, _length, _angle, _axiom, _production
};

typedef struct {                    //Struct zur Verwaltung des eingelesenen Inhalts
        string Name;
        string Length;
        string Angle;
        string Axiom;
        string Production;
} LSTRUCT;

//globaler Vektor mit Structs vom Typ LSTRUCT für den Fall mehrerer Produktionen
static vector<LSTRUCT> gl_lsys_file;
static LSTRUCT gl_act_item;


struct getData {

    //Konstruktor
    getData(pos switcher) :
    switcher_(switcher){
    }

    //Funktionsoperator überladen
    template<typename IteratorT >
            void operator()(IteratorT begin, IteratorT end) const {
        string str(begin, end);

        switch (switcher_) {
            case _name:
                gl_act_item.Name = str;
                gl_lsys_file.push_back(gl_act_item);
                break;

            case _length:
                gl_act_item.Length = str;
                gl_lsys_file.push_back(gl_act_item);
                break;

            case _angle:
                gl_act_item.Angle = str;
                gl_lsys_file.push_back(gl_act_item);
                break;

            case _axiom:
                gl_act_item.Axiom = str;
                gl_lsys_file.push_back(gl_act_item);
                break;

            case _production:
                gl_act_item.Production = str;
                gl_lsys_file.push_back(gl_act_item);
                break;
        }
    }

    pos switcher_;
};

struct lsystem_grammer
: public grammar<lsystem_grammer>{

    struct print {

        void operator()(const char *begin, const char *end) const {
            std::cout << std::string(begin, end) << std::endl;
        }

        void operator()(const double d) const {
            std::cout << d << std::endl;
        }

        template<typename IteratorT >
            void operator()(IteratorT begin, IteratorT end) const {
            std::cout << string(begin, end) << std::endl;
        }

    };

    template<typename Scanner>
    struct definition {
        rule<Scanner> name, length, angle, axiom, production, system;

        definition(const lsystem_grammer & self) {
            //Regeln
            name = +(anychar_p - space_p - "{");
            length = nocase_d["length"] >> ":" >> real_p;
            angle = nocase_d["angle"] >> ":" >> real_p;
            axiom = nocase_d["axiom"] >> ":" >> +(anychar_p - space_p - ";");
            production = nocase_d["production"] >> ":" >> +(anychar_p - "=") >> "=" >> +(anychar_p - space_p - ";");

            system = name[getData(_name)] >> "{" >> length[getData(_length)] >> ";" >> angle[getData(_angle)] >> ";" >> axiom[getData(_axiom)] >> ";" >> +(production[getData(_production)]  >> ";") >> "}";
        }

        const rule<Scanner> &start() {
            return system;
        }
    };
};

////////////////////////////////////////////////////////////////////////////////

class Lsystem
{
public:

    Lsystem();

    file_iterator<> load(char const* filename);
    string parse_lsystem(char const* filename, int iteration);


    double get_length() ;
    double get_angle() ;
    string get_axiom() const;
    vector<string> get_production() const;
    void erase_old();


private:
    vector<string> lsystem_;
    bool stochastic_;
};

#endif	/* _LSYSTEM_HPP */

