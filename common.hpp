#pragma once
#include <vector>
#include <iterator>
#include <algorithm>
//magic vector stuff
template<typename T>
struct initlist : public std::vector<T>
{
	initlist(const T& t)
	{
		(*this)(t);
	}
	initlist& operator()(const T& t)
	{
		this->push_back(t);
		return *this;
	}
};
