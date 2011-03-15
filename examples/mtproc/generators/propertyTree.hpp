#ifndef _Wolframe_GENERATOR_PROPERTYTREE_HPP_INCLUDED
#define _Wolframe_GENERATOR_PROPERTYTREE_HPP_INCLUDED
#include <boost/property_tree.hpp>
#include "protocol/generator.hpp"

namespace _Wolframe {
namespace mtproc {

class PropertyTreeGenerator :public protocol::Generator
{
public:
	PropertyTreeGenerator( const boost::property_tree::ptree& pt_) :protocol::Generator(&function),m_pt(pt_),m_itr(pt.begin()) {};

private:
	const boost::property_tree::ptree m_pt;
	boost::property_tree::ptree::const_iterator m_itr;
	static bool function( Generator* this_, ElementType* type, void* buf, unsigned int bufsize, unsigned int* bufpos);
};
