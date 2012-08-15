/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
	void closeTag();
	void pushValue( const char* val, std::size_t valsize);

private:
	typedef std::pair<int,int> Node;
	types::TypedArrayDoublingAllocator<Node> m_nodemem;
	types::TypedArrayDoublingAllocator<char> m_strmem;
	std::vector< std::vector<Node> > m_data;
	const TagTable* m_tagmap;
};

class Path
{
public:
	enum ElementType
	{
		Next,
		Find,
		Current,
		Up
	};

	static const char* elementTypeName( ElementType i)
	{
		static const char* ar[] ={"Next","Find","Current","Up"};
		return ar[(int)i];
	}

	struct Element
	{
		ElementType m_type;
		int m_id;
	};

	Path(){}
	Path( const std::string& pt, TagTable* tagmap);
	Path( const Path& o);
	std::string tostring() const;

private:
	std::vector<Element> m_path;
};


class FunctionCall
{
public:
	FunctionCall(){}
	FunctionCall( const FunctionCall& o);
	FunctionCall( const std::string& name, const std::string& selector, const std::vector<std::string>& arg);
private:
	TagTable m_tagmap;
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
private:
	std::vector<FunctionCall> m_call;
};

}}//namespace
#endif

