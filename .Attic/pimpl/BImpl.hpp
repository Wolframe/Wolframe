#ifndef BIMPL_H
#define BIMPL_H

#include "AImpl.hpp"

#include <iostream>

template<class impl>
class BImpl : public impl {
	public:
		virtual void print( const int x ) const {
			std::cout << "a (derived) integer: " << x << std::endl;
		}
};

#endif
