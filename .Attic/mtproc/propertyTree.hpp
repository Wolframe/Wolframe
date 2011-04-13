#ifndef _Wolframe_GENERATOR_PROPERTYTREE_HPP_INCLUDED
#define _Wolframe_GENERATOR_PROPERTYTREE_HPP_INCLUDED
#include "protocol/generator.hpp"
#include <boost/property_tree/ptree.hpp>

namespace _Wolframe {
namespace mtproc {

class PropertyTreeGenerator :public protocol::Generator
{
public:
	PropertyTreeGenerator( const boost::property_tree::ptree* pt);

private:
	struct Closure;
	Closure* m_closure;

	static bool get( Generator* this_, ElementType* type, void* buf, size_type, size_type* bufpos);
};

}}//namespace
#endif
