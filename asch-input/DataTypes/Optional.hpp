#ifndef __OPTIONAL_HPP__
#define __OPTIONAL_HPP__

template <class T>
class Optional {
	private:
	T value;
	bool exist;

	public:
	Optional(T val,bool ex):value(val),exist(ex){}

	bool present(){return exist;}
	T get(){return value;}
};

#endif
