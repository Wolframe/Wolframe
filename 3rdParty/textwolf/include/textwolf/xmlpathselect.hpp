/*
---------------------------------------------------------------------
    The template library textwolf implements an input iterator on
    a set of XML path expressions without backward references on an
    STL conforming input iterator as source. It does no buffering
    or read ahead and is dedicated for stream processing of XML
    for a small set of XML queries.
    Stream processing in this context refers to processing the
    document without buffering anything but the current result token
    processed with its tag hierarchy information.

    Copyright (C) 2010,2011,2012,2013,2014 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3.0 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of textwolf can be found at 'http://github.com/patrickfrey/textwolf'
	For documentation see 'http://patrickfrey.github.com/textwolf'

--------------------------------------------------------------------
*/
/// \file textwolf/xmlpathselect.hpp
/// \brief Context of running automaton selecting path expressions from an XML iterator

#ifndef __TEXTWOLF_XML_PATH_SELECT_HPP__
#define __TEXTWOLF_XML_PATH_SELECT_HPP__
#include "textwolf/char.hpp"
#include "textwolf/charset_interface.hpp"
#include "textwolf/exception.hpp"
#include "textwolf/xmlscanner.hpp"
#include "textwolf/staticbuffer.hpp"
#include "textwolf/xmlpathautomaton.hpp"
#include <limits>
#include <string>
#include <vector>
#include <map>
#include <cstddef>

namespace textwolf {

///\brief XML path select template
///\tparam CharSet_ character set encoding of the automaton elements
template <class CharSet_>
class XMLPathSelect :public throws_exception
{
public:
	typedef XMLPathSelectAutomaton<CharSet_> ThisXMLPathSelectAutomaton;
	typedef XMLPathSelect<CharSet_> ThisXMLPathSelect;

private:
	const ThisXMLPathSelectAutomaton* atm;		//< XML select automaton
	typedef typename ThisXMLPathSelectAutomaton::Mask Mask;
	typedef typename ThisXMLPathSelectAutomaton::Token Token;
	typedef typename ThisXMLPathSelectAutomaton::Hash Hash;
	typedef typename ThisXMLPathSelectAutomaton::State State;
	typedef typename ThisXMLPathSelectAutomaton::Scope Scope;

	///\class Array
	///\brief static array of POD types. I decided to implement it on my own though using boost::array would maybe be better.
	///\tparam Element element type of the array
	template <typename Element>
	class Array :public throws_exception
	{
		Element* m_ar;				//< pointer to elements
		std::size_t m_size;			//< fill size (number of elements inserted)
		std::size_t m_maxSize;			//< allocation size (space reserved for this number of elements)
	public:
		///\brief Constructor
		///\param [in] p_maxSize allocation size (number of elements) to reserve
		Array( std::size_t p_maxSize) :m_size(0),m_maxSize(p_maxSize)
		{
			m_ar = new (std::nothrow) Element[ m_maxSize];
			if (m_ar == 0) throw exception( OutOfMem);
		}

		///\brief Destructor
		~Array()
		{
			if (m_ar) delete [] m_ar;
		}

		///\brief Append one element
		///\param [in] elem element to append
		void push_back( const Element& elem)
		{
			if (m_size == m_maxSize) throw exception( OutOfMem);
			m_ar[ m_size++] = elem;
		}

		///\brief Remove one element from the end
		void pop_back()
		{
			if (m_size == 0) throw exception( NotAllowedOperation);
			m_size--;
		}

		///\brief Access element by index
		///\param [in] idx index of the element starting with 0
		///\return element reference
		Element& operator[]( std::size_t idx)
		{
			if (idx >= m_size) throw exception( ArrayBoundsReadWrite);
			return m_ar[ idx];
		}

		///\brief Get a reference of the element at the end of the array
		///\return element reference
		Element& back()
		{
			if (m_size == 0) throw exception( ArrayBoundsReadWrite);
			return m_ar[ m_size-1];
		}

		///\brief Resize of the array
		///\param [in] p_size new array size
		void resize( std::size_t p_size)
		{
			if (p_size > m_size) throw exception( ArrayBoundsReadWrite);
			m_size = p_size;
		}
		std::size_t size() const  {return m_size;}
		bool empty() const			{return m_size==0;}
	};

	///\class Context
	///\brief State variables without stacks of the automaton
	struct Context
	{
		XMLScannerBase::ElementType type;	//< element type processed
		const char* key;			//< string value of element processed
		unsigned int keysize;			//< size of string value in bytes of element processed
		Scope scope;				//< active scope
		unsigned int scope_iter;		//< position of currently visited token in the active scope

		///\brief Constructor
		Context()				:type(XMLScannerBase::Content),key(0),keysize(0) {}

		///\brief Initialization
		///\param [in] p_type type of the current element processed
		///\param [in] p_key current element processed
		///\param [in] p_keysize size of the key in bytes
		void init( XMLScannerBase::ElementType p_type, const char* p_key, int p_keysize)
		{
			type = p_type;
			key = p_key;
			keysize = p_keysize;
			scope_iter = scope.range.tokenidx_from;
		}
	};

	Array<Scope> scopestk;		//< stack of scopes opened
	Array<unsigned int> follows;	//< indices of tokens active in all descendant scopes
	Array<int> triggers;		//< triggered elements
	Array<Token> tokens;		//< list of waiting tokens
	Context context;		//< state variables without stacks of the automaton

	///\brief Activate a state by index
	///\param stateidx index of the state to activate
	void expand( int stateidx)
	{
		while (stateidx!=-1)
		{
			const State& st = atm->states[ stateidx];
			context.scope.mask.join( st.core.mask);
			if (st.core.mask.empty() && st.core.typeidx != 0)
			{
				triggers.push_back( st.core.typeidx);
			}
			else
			{
				if (st.core.follow)
				{
					context.scope.followMask.join( st.core.mask);
					follows.push_back( tokens.size());
				}
				tokens.push_back( Token( st, stateidx));
			}
			stateidx = st.link;
		}
	}

	///\brief Declares the currently processed element of the XMLScanner input. By calling fetch we get the output elements from it
	///\param [in] type type of the current element processed
	///\param [in] key current element processed
	///\param [in] keysize size of the key in bytes
	void initProcessElement( XMLScannerBase::ElementType type, const char* key, int keysize)
	{
		if (context.type == XMLScannerBase::OpenTag)
		{
			//last step of open scope has to be done after all tokens were visited,
			//e.g. with the next element initialization
			context.scope.range.tokenidx_from = context.scope.range.tokenidx_to;
		}
		context.scope.range.tokenidx_to = tokens.size();
		context.scope.range.followidx = follows.size();
		context.init( type, key, keysize);

		if (type == XMLScannerBase::OpenTag)
		{
			//first step of open scope saves the context context on stack
			scopestk.push_back( context.scope);
			context.scope.mask = context.scope.followMask;
			context.scope.mask.match( XMLScannerBase::OpenTag);
			//... we reset the mask but ensure that this 'OpenTag' is processed for sure
		}
		else if (type == XMLScannerBase::CloseTag || type == XMLScannerBase::CloseTagIm)
		{
			if (!scopestk.empty())
			{
				context.scope = scopestk.back();
				scopestk.pop_back();
				follows.resize( context.scope.range.followidx);
				tokens.resize( context.scope.range.tokenidx_to);
			}
		}
	}

	///\brief produce an element adressed by token index
	///\param [in] tokenidx index of the token in the list of active tokens
	///\param [in] st state from which the expand was triggered
	void produce( unsigned int tokenidx, const State& st)
	{
		const Token& tk = tokens[ tokenidx];
		if (tk.core.cnt_end == -1)
		{
			expand( st.next);
		}
		else
		{
			if (tk.core.cnt_end > 0)
			{
				if (--tokens[ tokenidx].core.cnt_end == 0)
				{
					tokens[ tokenidx].core.mask.reset();
				}
				if (tk.core.cnt_start <= 0)
				{
					expand( st.next);
				}
				else
				{
					--tokens[ tokenidx].core.cnt_start;
				}
			}
		}
	}

	///\brief check if an active token addressed by index matches to the currently processed element
	///\param [in] tokenidx index of the token in the list of active tokens
	///\return matching token type
	int match( unsigned int tokenidx)
	{
		int rt = 0;
		if (context.key != 0)
		{
			if (tokenidx >= context.scope.range.tokenidx_to) return 0;

			const Token& tk = tokens[ tokenidx];
			if (tk.core.mask.matches( context.type))
			{
				const State& st = atm->states[ tk.stateidx];
				if (st.key)
				{
					if (st.keysize == context.keysize)
					{
						unsigned int ii;
						for (ii=0; ii<context.keysize && st.key[ii] == context.key[ii]; ii++);
						if (ii==context.keysize)
						{
							produce( tokenidx, st);
						}
					}
				}
				else
				{
					produce( tokenidx, st);
				}
				if (tk.core.typeidx != 0)
				{
					if (tk.core.cnt_end == -1)
					{
						rt = tk.core.typeidx;
					}
					else if (tk.core.cnt_end > 0)
					{
						if (--tokens[ tokenidx].core.cnt_end == 0)
						{
							tokens[ tokenidx].core.mask.reset();
						}
						if (tk.core.cnt_start <= 0)
						{
							rt = tk.core.typeidx;
						}
						else
						{
							--tokens[ tokenidx].core.cnt_start;
						}
					}
				}
			}
			if (tk.core.mask.rejects( context.type))
			{
				//The token must not match anymore after encountering a reject item
				tokens[ tokenidx].core.mask.reset();
			}
		}
		return rt;
	}

	///\brief fetch the next matching element
	///\return type of the matching element
	int fetch()
	{
		int type = 0;

		if (context.scope.mask.matches( context.type))
		{
			while (!type)
			{
				if (context.scope_iter < context.scope.range.tokenidx_to)
				{
					type = match( context.scope_iter);
					++context.scope_iter;
				}
				else
				{
					unsigned int ii = context.scope_iter - context.scope.range.tokenidx_to;
					//we match all follows that are not yet been checked in the current scope
					if (ii < context.scope.range.followidx && context.scope.range.tokenidx_from > follows[ ii])
					{
						type = match( follows[ ii]);
						++context.scope_iter;
					}
					else if (!triggers.empty())
					{
						type = triggers.back();
						triggers.pop_back();
					}
					else
					{
						context.key = 0;
						context.keysize = 0;
						return 0; //end of all candidates
					}
				}
			}
		}
		else
		{
			context.key = 0;
			context.keysize = 0;
		}
		return type;
	}

public:
	///\brief Constructor
	///\param[in] p_atm read only ML path select automaton reference
	XMLPathSelect( const ThisXMLPathSelectAutomaton* p_atm)
		:atm(p_atm),scopestk(p_atm->maxScopeStackSize),follows(p_atm->maxFollows),triggers(p_atm->maxTriggers),tokens(p_atm->maxTokens)
	{
		if (atm->states.size() > 0) expand(0);
	}

	///\brief Copy constructor
	///\param [in] o element to copy
	XMLPathSelect( const XMLPathSelect& o)
		:atm(o.atm),scopestk(o.scopestk),follows(o.follows),triggers(o.triggers),tokens(o.tokens){}

	///\class iterator
	///\brief input iterator for the output of this XMLScanner
	class iterator
	{
	public:
		typedef int value_type;
		typedef std::size_t difference_type;
		typedef int* pointer;
		typedef int& reference;
		typedef std::input_iterator_tag iterator_category;

	private:
		int element;					//< currently visited element (type)
		ThisXMLPathSelect* input;			//< producing XML path selection stream

		///\brief Skip to next element
		///\return *this
		iterator& skip() throw(exception)
		{
			if (input != 0)
			{
				element = input->fetch();
			}
			else
			{
				element = 0;
			}
			return *this;
		}

		///\brief Iterator compare
		///\param [in] iter iterator to compare with
		///\return true, if the elements are equal
		bool compare( const iterator& iter) const
		{
			return (element == iter.element);
		}

	public:
		///\brief Assign iterator
		///\param [in] orig iterator to copy
		void assign( const iterator& orig)
		{
			input = orig.input;
			element = orig.element;
		}

		///\brief Copy constructor
		///\param [in] orig iterator to copy
		iterator( const iterator& orig)
		{
			assign( orig);
		}

		///\brief Constructor by values
		///\param [in] p_input XML path selection stream to iterate through
		iterator( ThisXMLPathSelect& p_input, XMLScannerBase::ElementType type, const char* key, int keysize)
				:input( &p_input)
		{
			input->initProcessElement( type, key, keysize);
			skip();
		}

		///\brief Constructor
		///\param [in] et end of input tag
		iterator()
			:element(0),input(0) {}

		///\brief Assignement
		///\param [in] orig iterator to copy
		///\return *this
		iterator& operator = (const iterator& orig)
		{
			assign( orig);
			return *this;
		}

		///\brief Element acceess
		///\return read only element reference
		int operator*() const
		{
			return element;
		}

		///\brief Element acceess
		///\return read only element reference
		const int* operator->() const
		{
			return &element;
		}

		///\brief Preincrement
		///\return *this
		iterator& operator++()		{return skip();}

		///\brief Postincrement
		///\return *this
		iterator operator++(int)	{iterator tmp(*this); skip(); return tmp;}

		///\brief Compare elements for equality
		///\return true, if they are equal
		bool operator==( const iterator& iter) const	{return compare( iter);}

		///\brief Compare elements for inequality
		///\return true, if they are not equal
		bool operator!=( const iterator& iter) const	{return !compare( iter);}
	};

	///\brief Feed the path selector with the next token and get the start iterator for the results
	///\return iterator pointing to the first of the selected XML path elements
	iterator push( XMLScannerBase::ElementType type, const char* key, int keysize)
	{
		return iterator( *this, type, key, keysize);
	}

	///\brief Get the end of results returned by 'push(XMLScannerBase::ElementType,const char*, int)'
	///\return the end iterator
	iterator end()
	{
		return iterator();
	}
};

}//namespace
#endif
