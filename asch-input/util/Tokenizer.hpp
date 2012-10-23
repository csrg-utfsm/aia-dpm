#ifndef __TOKENIZER_HPP__
#define __TOKENIZER_HPP__

#include <string>
#include <list>
#include <boost/tokenizer.hpp>


namespace asch_input{

class Tokenizer {
  
  public:
    Tokenizer(std::string& filename, const char* separator);
    ~Tokenizer();
    std::list<std::string>* getNextLineOfTokens();
    void printTokens(std::list<std::string>* tokens);
    
  private:
    boost::char_separator<char> sep;
    std::ifstream *file;
    
};

}

#endif
