#include "Tokenizer.hpp"
#include <iostream>
#include <fstream>
#include <boost/tokenizer.hpp>

using namespace asch_input;
using namespace std;
using namespace boost;

Tokenizer::Tokenizer(string& filename, const char* separator) : sep(separator ){
  cout << "LOG: Tokenizer" << endl;
  file = new ifstream(filename.c_str(), ifstream::in);
}

Tokenizer::~Tokenizer(){
  cout << "LOG: ~Tokenizer" << endl;
  if( file != NULL )
    delete file;
}

/** Reads a line from file and decomposes it according to the indicated separator.
  * @return A vector of strings, where each one is an entry in the line.
  * NOTE: Needs to free returned vector.
  */
  
list<std::string>* Tokenizer::getNextLineOfTokens(){
		list<string>* tokens = new list<string>();
		string s;
		
		do{
				getline( *file, s);
		}while(s.size() > 0 && s[0] == '#' );
		
		if( file->eof() )
				return NULL;
		boost::tokenizer< boost::char_separator<char> > tok(s,sep);
		for(tokenizer< boost::char_separator<char> >::iterator beg=tok.begin(); beg!=tok.end();++beg){
				tokens->push_back( *beg );
		}
		return tokens;
}

void Tokenizer::printTokens(list<string>* tokens){
  for( list<string>::const_iterator it = tokens->begin(); it != tokens->end(); ++it ) {
      cout << *it << endl;
  }
}
