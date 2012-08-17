/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\brief Interface of the processor of database commands
///\file database/processor.hpp
#ifndef _DATABASE_PROCESSOR_HPP_INCLUDED
#define _DATABASE_PROCESSOR_HPP_INCLUDED
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <stdexcept>
#include "types/allocators.hpp"

namespace _Wolframe {
namespace db {

struct DatabaseInterface
{
	virtual bool begin()=0;
	virtual bool commit()=0;
	virtual bool rollback()=0;
	virtual bool start( const std::string& stmname)=0;
	virtual bool bind( unsigned int idx, const std::string& value)=0;
	virtual bool exec()=0;
	virtual unsigned int nofColumns()=0;
	virtual char* getLastError()=0;
	virtual bool get( unsigned int idx, const char*& tagname, std::string& value)=0;
	virtual bool next()=0;
};

class TagTable
{
public:
	TagTable()
		:m_size(0){}
	TagTable( const TagTable& o)
		:m_size(o.m_size),m_map(o.m_map){}

	int find( const char* tag, std::size_t tagsize) const;
	int find( const std::string& tagstr) const;
	int get( const std::string& tagstr);
	int get( const char* tag, std::size_t tagsize);
	int unused() const;

private:
	int m_size;
	std::map< std::string, int> m_map;
};

class Structure
{
public:
	Structure( const TagTable* tagmap);
	Structure( const Structure& o);

	void openTag( const char* tag, std::size_t tagsize);
	void openTag( const std::string& tag);
	void closeTag();
	void pushValue( const char* val, std::size_t valsize);
	void pushValue( const std::string& val);

	struct Node
	{
		int m_parent;
		int m_tag;
		int m_elementsize;
		int m_element;

		Node()
			:m_parent(0)
			,m_tag(0)
			,m_elementsize(0)
			,m_element(0){}

		Node( const Node& o)
			:m_parent(o.m_parent)
			,m_tag(o.m_tag)
			,m_elementsize(o.m_elementsize)
			,m_element(o.m_element){}

		Node( int p, int t, int size, int e)
			:m_parent(p)
			,m_tag(t)
			,m_elementsize(size)
			,m_element(e){}

		static int ref_element( std::size_t idx)			{if (idx >= (std::size_t)std::numeric_limits<int>::max()) throw std::bad_alloc(); return -(int)idx;}
		static int val_element( std::size_t idx)			{if (idx >= (std::size_t)std::numeric_limits<int>::max()) throw std::bad_alloc(); return (int)idx;}

		std::size_t childidx() const					{return (m_element < 0)?(std::size_t)-m_element:0;}
		std::size_t nofchild() const					{return (m_element < 0)?(std::size_t)m_elementsize:0;}
		std::size_t valueidx() const					{return (m_element > 0)?(std::size_t)m_element:0;}
		std::size_t valuesize() const					{return (m_element > 0)?(std::size_t)m_elementsize:0;}
	};

	std::size_t root() const						{if (m_data.size() != 1) throw std::runtime_error( "accessing root of incomplete structure"); return m_rootidx;}
	bool next( const Node& nd, int tag, std::vector<Node>& rt) const;
	bool find( const Node& nd, int tag, std::vector<Node>& rt) const;
	bool up( const Node& nd, std::vector<Node>& rt) const;

private:
	void setParentLinks( std::size_t mi);

private:
	types::TypedArrayDoublingAllocator<Node> m_nodemem;
	types::TypedArrayDoublingAllocator<char> m_strmem;
	std::vector< std::vector<Node> > m_data;
	const TagTable* m_tagmap;
	std::size_t m_rootidx;
};


class Path
{
public:
	enum ElementType
	{
		Next,
		Find,
		Current,
		Up,
		Result
	};

	static const char* elementTypeName( ElementType i)
	{
		static const char* ar[] ={"Next","Find","Current","Up","Result"};
		return ar[(int)i];
	}

	struct Element
	{
		ElementType m_type;
		int m_tag;
	};

	Path(){}
	Path( const std::string& pt, TagTable* tagmap);
	Path( const Path& o);
	std::string tostring() const;

	std::vector<Element>::const_iterator begin() const	{return m_path.begin();}
	std::vector<Element>::const_iterator end() const	{return m_path.end();}
	std::size_t size() const				{return m_path.size();}
private:
	std::vector<Element> m_path;
	std::string m_resultvar;
};


class FunctionCall
{
public:
	FunctionCall(){}
	FunctionCall( const FunctionCall& o);
	FunctionCall( const std::string& name, const Path& selector, const std::vector<Path>& arg);

	const Path& selector() const			{return m_selector;}
	const std::vector<Path>& arg() const		{return m_arg;}

private:
	std::string m_name;
	Path m_selector;
	std::vector<Path> m_arg;
};


class TransactionProgram
{
public:
	TransactionProgram(){}
	TransactionProgram( const TransactionProgram& o);
	TransactionProgram( const std::string& src);

	bool execute( DatabaseInterface* dbinterf, const std::string& content) const;
private:
	std::vector<FunctionCall> m_call;
	TagTable m_tagmap;
};

}}//namespace
#endif

