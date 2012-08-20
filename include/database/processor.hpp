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
#include "types/allocators.hpp"
#include "langbind/transactionFunction.hpp"
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <stdexcept>

namespace _Wolframe {
namespace db {

struct DatabaseInterface
{
	virtual ~DatabaseInterface(){}
	///\brief Begin transaction
	virtual bool begin()=0;
	///\brief Commit current transaction
	virtual bool commit()=0;
	///\brief Rollback current transaction
	virtual bool rollback()=0;
	///\brief Start new command statement
	virtual bool start( const std::string& stmname)=0;
	///\brief Bind parameter value on current command statement
	virtual bool bind( std::size_t idx, const char* value)=0;
	///\brief Execute instance of current statement
	virtual bool execute()=0;
	///\brief Get the number of columns of the last result
	virtual unsigned int nofColumns()=0;
	///\brief Get a column title of the last result
	virtual const char* columnName( std::size_t idx)=0;
	///\brief Get the last database error as string
	virtual const char* getLastError()=0;
	///\brief Get a column of the last result
	virtual const char* get( std::size_t idx)=0;
	///\brief Skip to the next row of the last result
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

	const Node& root() const						{if (m_rootidx == 0) throw std::runtime_error( "accessing root of incomplete structure"); return m_nodemem[ m_rootidx];}

	bool next( const Node& nd, int tag, std::vector<Node>& rt) const;
	bool find( const Node& nd, int tag, std::vector<Node>& rt) const;
	bool up( const Node& nd, std::vector<Node>& rt) const;
	const char* nodevalue( std::size_t idx) const				{return &m_strmem[idx];}

	const std::string tostring() const;

protected:
	void setParentLinks( std::size_t mi);
	void openTag( const char* tag, std::size_t tagsize);
	void openTag( const std::string& tag);
	void closeTag();
	void pushValue( const char* val, std::size_t valsize);
	void pushValue( const std::string& val);
	void check() const;

private:
	types::TypedArrayDoublingAllocator<Node> m_nodemem;
	types::TypedArrayDoublingAllocator<char> m_strmem;
	const TagTable* m_tagmap;
	std::size_t m_rootidx;
	typedef std::vector< std::vector<Node> > BuildNodeStruct;
	BuildNodeStruct m_data;
};


class Path
{
public:
	enum ElementType
	{
		Next,
		Find,
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
	Path( const std::string& src, TagTable* tagmap);
	Path( const Path& o);
	std::string tostring() const;

	std::size_t resultReference() const;
	bool selectNodes( const Structure& st, const Structure::Node& nd, std::vector<Structure::Node>& ar) const;

	std::vector<Element>::const_iterator begin() const		{return m_path.begin();}
	std::vector<Element>::const_iterator end() const		{return m_path.end();}
	std::size_t size() const					{return m_path.size();}

private:
	std::vector<Element> m_path;
};


class FunctionCall
{
public:
	FunctionCall(){}
	FunctionCall( const FunctionCall& o);
	FunctionCall( const std::string& resname, const std::string& name, const Path& selector, const std::vector<Path>& arg);

	const Path& selector() const			{return m_selector;}
	const std::vector<Path>& arg() const		{return m_arg;}
	const std::string& name() const			{return m_name;}
	const std::string& resultname() const		{return m_resultname;}

	void resultname( const char* r)			{m_resultname = r;}
	bool hasResultReference() const;

private:
	std::string m_resultname;
	std::string m_name;
	Path m_selector;
	std::vector<Path> m_arg;
};


class TransactionResult
	:public langbind::TransactionResult
{
public:
	TransactionResult(){}
	TransactionResult( const TransactionResult& o);
	virtual ~TransactionResult(){}

	virtual bool getNext( ElementType& type, langbind::TypedFilterBase::Element& element);

private:
	friend class TransactionFunction;
	void openTag( const std::string& tag);
	void openTag( const char* tag, std::size_t tagsize);
	void closeTag();
	void pushValue( const std::string& val);
	void pushValue( const char* val, std::size_t valsize);
	void finalize();

private:
	typedef std::pair< langbind::InputFilter::ElementType, langbind::TypedFilterBase::Element> Item;
	typedef std::vector<Item> ItemArray;
	ItemArray m_itemar;
	ItemArray::const_iterator m_itemitr;
};


class TransactionInput
	:public langbind::TransactionInput
	,public Structure
{
public:
	explicit TransactionInput( const TagTable* tagmap);
	TransactionInput( const TransactionInput& o);
	virtual ~TransactionInput(){}

	virtual bool print( ElementType type, const Element& element);
private:
	langbind::TypedInputFilter::ElementType m_lasttype;
};


class TransactionFunction :public langbind::TransactionFunction
{
public:
	TransactionFunction( const TransactionFunction& o);
	TransactionFunction( const std::string& src);
	virtual ~TransactionFunction(){}

	virtual langbind::TransactionInputR getInput() const;
	virtual langbind::TransactionResultR execute( DatabaseInterface* dbi, const langbind::TransactionInput* input) const;

private:
	std::string m_resultname;
	std::vector<FunctionCall> m_call;
	TagTable m_tagmap;
};

}}//namespace
#endif

