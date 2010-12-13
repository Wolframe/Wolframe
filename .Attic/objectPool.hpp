//
// thread capable pool of objects
//

#ifndef _OBJECT_POOL_HPP_INCLUDED
#define _OBJECT_POOL_HPP_INCLUDED


#include <list>

namespace _SMERP	{

// the object pool
template < typename objectType >
class ObjectPool	{
public:
	ObjectPool( const unsigned timeout );
	~ObjectPool();

	size_t size();
	void add ( objectType& obj );
	void remove ( objectType& obj );
	objectType& get();
	void release ( objectType& obj );
private:
	unsigned	timeout_;
};

} // namespace _SMERP

#endif // _OBJECT_POOL_HPP_INCLUDED
