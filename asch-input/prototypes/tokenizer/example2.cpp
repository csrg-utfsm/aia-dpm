// simple_example_2.cpp
#include<iostream>
#include<boost/tokenizer.hpp>
#include<string>

int main(){

	using namespace std;
	using namespace boost;

	string s = "This\tis,\t\ta\ttest";
	char_delimiters_separator<char> sep(false, "", "\t");
	tokenizer<> tok(s,sep);
	for(tokenizer<>::iterator beg=tok.begin(); beg!=tok.end();++beg){
		cout << *beg << "\n";
	}
}

