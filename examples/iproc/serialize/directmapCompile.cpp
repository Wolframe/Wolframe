#include "directmapCompile.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <stdexcept>
#include <cstring>
#include <cstddef>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#define SPIRIT_VERSION 2050
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

using namespace _Wolframe;
using namespace directmap;

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

template<class Iterator>
class skipper : public boost::spirit::qi::grammar<Iterator>
{
	boost::spirit::qi::rule<Iterator> start;
public:
	skipper():skipper::base_type(start)
	{
		using boost::spirit::qi::eol;
		using boost::spirit::qi::char_;
		using boost::spirit::qi::space;
		start = space | char_('#') >> *(char_('\\') >> char_ | char_ - eol)>> eol;
	}
};


BOOST_FUSION_ADAPT_STRUCT(
	_Wolframe::directmap::SymbolTable::Element,
		(_Wolframe::directmap::SymbolTable::Element::Type, m_type)
		(std::string, m_name)
		(std::string, m_default)
		(int, m_ref)
		(std::size_t, m_size)
		(bool, m_isArray)
)

BOOST_FUSION_ADAPT_STRUCT(
	_Wolframe::directmap::SymbolTable::Struct,
		(std::string, m_name)
		(std::vector<_Wolframe::directmap::SymbolTable::Element>, m_elements)
		(std::size_t, m_size)
)

typedef std::map<std::string,std::size_t> LinkMap;	///< typedef for fix boost macro BOOST_FUSION_ADAPT_STRUCT expansion (C preprocessor inteprets commas !)
BOOST_FUSION_ADAPT_STRUCT(
	_Wolframe::directmap::SymbolTable,
		(LinkMap, m_linkmap)
		(std::vector<_Wolframe::directmap::SymbolTable::Struct>, m_ar)
)

bool SymbolTable::define( const std::string& name, const Struct& st)
{
	if (m_linkmap.find( name) != m_linkmap.end()) return false;
	m_linkmap[ name] = m_ar.size();
	m_ar.push_back( st);
	return true;
}

static void defineSymbol( SymbolTable& sy, const std::string& name, const SymbolTable::Struct& st)
{
	std::string msg( "duplicate definition of symbol '");
	msg.append( name);
	msg.append( "'");
	if (!sy.define( name, st)) sy.error( msg);
}

struct ElementType_ : public boost::spirit::qi::symbols<char, SymbolTable::Element::Type>
{
	ElementType_()
	{
		add
		("float", SymbolTable::Element::float_)
		("long", SymbolTable::Element::long_)
		("ulong", SymbolTable::Element::ulong_)
		("int", SymbolTable::Element::int_)
		("uint", SymbolTable::Element::uint_)
		("short", SymbolTable::Element::short_)
		("ushort", SymbolTable::Element::ushort_)
		("char", SymbolTable::Element::char_)
		("uchar", SymbolTable::Element::uchar_)
		("string", SymbolTable::Element::string_)
		("struct", SymbolTable::Element::struct_)
		;
	}
};

template<class Iterator>
class DirectmapGrammar :public boost::spirit::qi::grammar<Iterator,void(),skipper<Iterator> >
{
private:
	typedef boost::spirit::qi::grammar<Iterator,void(),skipper<Iterator> > Parent;
	SymbolTable* m_symtab;
	SymbolTable::Struct m_struct;
	SymbolTable::Element m_element;
	std::string m_refname;

	typedef skipper<Iterator> Skip;
	boost::spirit::qi::rule<Iterator,void(),Skip> start_;
	boost::spirit::qi::rule<Iterator,void(),Skip> element_;
	boost::spirit::qi::rule<Iterator,void(),Skip> struct_;
	boost::spirit::qi::rule<Iterator,void(),Skip> structheader_;
	boost::spirit::qi::rule<Iterator,void(),Skip> value_;
public:
	DirectmapGrammar( SymbolTable* s)
		:Parent(start_),m_symtab(s)
	{
		using boost::spirit::qi::void_
		using boost::spirit::qi::eol;
		using boost::spirit::qi::lit;
		using boost::spirit::qi::char_;
		using boost::spirit::qi::alpha;
		using boost::spirit::qi::digit;
		using boost::spirit::qi::lexeme;
		using boost::spirit::qi::_1;
		using boost::phoenix::ref;

		start_ = *struct_;
		struct_ = (
				structheader_ >> *element_ >> lit("end")
			)[void_[ defineSymbol]( *m_symtab, m_refname, m_struct)];

		structheader_ = (
					(lit("struct") >> lit(m_struct.m_name))[ ref(m_refname) = m_struct.m_name]
					>> -( lit("as") >> lit(m_refname) )
					>> eol
				);

		element_ = (
				(
					ElementType_[ ref(m_element.m_type) = _1] >> lit(m_element.m_name)
					>> -( value_)
						| (char_('[') >> ']')[ ref(m_element.m_isArray)=true]
				)
				>> eol
			)[ void_[ m_struct.m_elements.push_back](m_element)];

		value_ = (char_('\'') >> (*(lexeme[ char_ - char_('\'')]))[ ref( m_element.m_default) = _1] >> char_('\''))
			| (char_('\"') >> (*(lexeme[ char_ - char_('\"')]))[ ref( m_element.m_default) = _1] >> char_('\"'))
			| (lexeme[+(alpha|digit | char_('_'))])[ ref( m_element.m_default) = _1];
			;
)
	}
};

template <typename Iterator>
static bool read_symboltable( Iterator first, Iterator last, SymbolTable& symtab)
{
	DirectmapGrammar G(&symtab);
	return boost::spirit::qi::phrase_parse( first, last, (G)) && (first == last);
}

bool SymbolTable::compile( const char* filename, std::string& errors_)
{
	bool rt = false;
	std::string src;
	if (!readFile( filename, src))
	{
		std::stringstream err;
		err << "failed to read input file " << filename << std::endl;
		error_ = err.str();
		return false;
	}
	try
	{
		rt = read_symboltable( src.begin(), src.end(), *this);
	}
	catch (std::exception& e)
	{
		std::stringstream err;
		err << filename << /*" [" << src.line(e.where) << "," << src.pos(e.where) << "]: " <<*/ e.what() << std::endl;
		error_ = err.str();
		rt = false;
	}
	std::vector<std::string>::const_iterator ei=m_errors.begin(),ee=m_errors.end();
	while (ei != ee)
	{
		rt = false;
		errors_.append( *ei);
		errors_.append( "\n");
		++ei;
	}
	return rt;
}

