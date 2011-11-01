#include "directmapCompile.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#define SPIRIT_VERSION 2050
#include <boost/spirit/include/qi.hpp>

using namespace _Wolframe;
using namespace directmap;
using namespace boost::spirit;

static bool readFile( const char* fn, std::string& out)
{
	char buf;
	std::fstream ff;
	ff.open( fn, std::ios::in | std::ios::binary);
	while (ff.read( &buf, sizeof(buf)))
	{
		out.push_back( buf);
	}
	bool rt = ((ff.rdstate() & std::ifstream::eofbit) != 0);
	ff.close();
	return rt;
}

///\remark The following example was inspired by the key value parser example of Hartmut Kaiser
///\link http://boost-spirit.com/home/articles/qi-example/parsing-a-list-of-key-value-pairs-using-spirit-qi/
template <typename Iterator>
struct Grammar
	: qi::grammar<Iterator, std::vector<Definition::Struct>()>
{
	Grammar() : Grammar::base_type(start)
	{
		start =		record >> *(record);
		record =	lit("form") >> recordname >> qi::eol >> elementlist >> qi::lit("end");
		elementlist =	element >> *(element);
		element =	type >> name >> -(defaultvalue || (qi::char_('[') >> ']')) >> qi::eol;
		type =		qi::char_("a-zA-Z_") >> *qi::char_("a-zA-Z_0-9");
		name =		qi::char_("a-zA-Z_") >> *qi::char_("a-zA-Z_0-9");
		recordname =	qi::char_("a-zA-Z_") >> *qi::char_("a-zA-Z_0-9");
		defaultvalue =	(qi::char_('\'') >> *(lexeme[ qi::char_ - qi::char_('\'')]) >> qi::char_('\''))
		||		(qi::char_('\"') >> *(lexeme[ qi::char_ - qi::char_('\"')]) >> qi::char_('\"'))
		||		(qi::char_("a-zA-Z_") >> *qi::char_("a-zA-Z_0-9"));
	}
	qi::rule< Iterator, std::vector< Definition::Struct>()> start;
	qi::rule< Iterator, Definition::Struct()> record;
	qi::rule< Iterator, std::vector< Definition::Element>()> elementlist;
	qi::rule< Iterator, Definition::Element()> element;
	qi::rule< Iterator, std::string()> type, name, recordname, defaultvalue;
};

std::size_t Definition::calcElementSize( std::size_t idx, std::size_t depht=0)
{
	std::size_t rt = 0;
	if (depht > m_ar.size()) return 0;
	std::vector<Definition::Element>::iterator eitr = m_ar[ idx].m_elements.begin(), eend = m_ar[ idx].m_elements.end();
	while (eitr != eend)
	{
		if (eitr->m_size == 0)
		{
			if (eitr->m_ref == -1) return 0;
			eitr->m_size = calcElementSize( eitr->m_ref, depht+1);
			if (eitr->m_size) return 0;
		}
		rt += eitr->m_size;
		++eitr;
	}
	return rt;
}

bool Definition::compile( const char* filename, std::string& errors_)
{
	bool rt = false;
	std::stringstream err;
	std::string src;
	if (!readFile( filename, src))
	{
		err << filename << ": failed to read file" << std::endl;
		rt = false;
	}
	else try
	{
		Grammar<std::string::iterator> p;
		rt = qi::parse(src.begin(), src.end(), p, m_ar);
		if (rt)
		{
			std::vector<Definition::Struct>::iterator itr = m_ar.begin(), end = m_ar.end();
			std::size_t ii = 0;
			while (itr != end)
			{
				if (m_linkmap.find( itr->name()) != m_linkmap.end())
				{
					err << filename << ": duplicate definition of struct '" << itr->name() << "'" << std::endl;
					rt = false;
				}
				m_linkmap[ itr->name()] = ii;
				++itr;
				++ii;
			}
			itr = m_ar.begin(), end = m_ar.end();
			while (itr != end)
			{
				std::vector<Definition::Element>::iterator eitr = itr->m_elements.begin(), eend = itr->m_elements.end();
				while (eitr != eend)
				{
					if (eitr->type() == Element::string_)
					{
						if (m_linkmap.find( eitr->name()) == m_linkmap.end())
						{
							err << filename << ": unresolved reference of struct '" << eitr->name() << "'" << std::endl;
							rt = false;
						}
						else
						{
							eitr->m_ref = m_linkmap[ itr->name()];
						}
					}
					++eitr;
				}
				++itr;
			}
			itr = m_ar.begin(), end = m_ar.end(), ii=0;
			while (itr != end)
			{
				if (itr->m_size == 0)
				{
					itr->m_size = calcElementSize( ii);
				}
				++itr;
				++ii;
			}
		}
		else
		{
			err << filename << ": unspecified syntax error" << std::endl;
		}
	}
	catch (std::exception& e)
	{
		err << filename << ": " << e.what() << std::endl;
		rt = false;
	}
	std::vector<std::string>::const_iterator ei=m_errors.begin(),ee=m_errors.end();
	while (ei != ee)
	{
		rt = false;
		err << filename << ": " << *ei << std::endl;
		++ei;
	}
	errors_ = err.str();
	return rt;
}

