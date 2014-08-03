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
/// \file textwolf/xmlpathautomaton.hpp
/// \brief Automaton to select path expressions from an XML iterator

#ifndef __TEXTWOLF_XML_PATH_AUTOMATON_HPP__
#define __TEXTWOLF_XML_PATH_AUTOMATON_HPP__
#include "textwolf/char.hpp"
#include "textwolf/charset.hpp"
#include "textwolf/exception.hpp"
#include "textwolf/xmlscanner.hpp"
#include "textwolf/staticbuffer.hpp"
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cstddef>
#include <stdexcept>

namespace textwolf {

///\class XMLPathSelectAutomaton
///\tparam CharSet_ character set of the token defintions of the automaton
///\brief Automaton to define XML path expressions and assign types (int values) to them
template <class CharSet_=charset::UTF8>
class XMLPathSelectAutomaton :public throws_exception
{
public:
	enum
	{
		defaultMemUsage=3*1024,		//< default memory usage of the XML path select process, if not specified else
		defaultMaxDepth=32		//< default max tag stack depth, if not specified else
	};
	std::size_t memUsage;			//< total memory usage
	unsigned int maxDepth;			//< max tag stack depth
	std::size_t maxScopeStackSize;		//< max scope stack depth
	unsigned int maxFollows;		//< maximum number of tokens searched in depth
	unsigned int maxTriggers;		//< maximum number of open triggers
	unsigned int maxTokens;			//< maximum number of open tokens

public:
	///\brief Constructor
	XMLPathSelectAutomaton()
		:memUsage(defaultMemUsage)
		,maxDepth(defaultMaxDepth)
		,maxScopeStackSize(0)
		,maxFollows(0)
		,maxTriggers(0)
		,maxTokens(0)
	{
		if (!setMemUsage( memUsage, maxDepth)) throw exception( DimOutOfRange);
	}
	typedef CharSet_ CharSet;
	typedef int Hash;
	typedef XMLPathSelectAutomaton<CharSet> ThisXMLPathSelectAutomaton;

	virtual ~XMLPathSelectAutomaton(){}

public:
	///\enum Operation
	///\brief Enumeration of operation types in the automaton definition
	enum Operation
	{
		Content,			//< searching content token
		Tag,				//< searching a tag
		Attribute,			//< searching an attribute
		ThisAttributeValue,		//< checking the value of the attribute just parsed (not an arbitrary but this one)
		AttributeValue,			//< searching a value of an attribute
		ContentStart			//< looking for the start of content (to signal the end of the XML header)
	};

	///\brief Get the name of the operation as string
	///\return the operation as string
	static const char* operationName( Operation op)
	{
		static const char* name[ 6] = {"Content", "Tag", "Attribute", "ThisAttributeValue", "AttributeValue", "ContentStart"};
		return name[ (unsigned int)op];
	}

	///\class Mask
	///\brief Mask to query for element types, if they match or not
	struct Mask
	{
		unsigned short pos;			//< positively selected elements bitmask
		unsigned short neg;			//< negatively selected elements bitmask that determines when a search pattern is given up copletely

		///\brief Tells if mask does not select anything anymore
		///\return true if it is not active anymore
		bool empty() const								{return (pos==0);}

		///\brief Constructor by values
		///\param [in] p_pos positively selected elements bitmask
		///\param [in] p_neg negatively selected elements bitmask that determines when a search pattern is given up copletely
		Mask( unsigned short p_pos=0, unsigned short p_neg=0):pos(p_pos),neg(p_neg) {}

		///\brief Copy constructor
		///\param[in] orig mask to copy
		Mask( const Mask& orig)								:pos(orig.pos),neg(orig.neg) {}

		///\brief Constructor by operation type
		Mask( Operation op)								:pos(0),neg(0) {this->match(op);}

		///\brief Reset operation (deactivate)
		void reset()									{pos=0; neg=0;}

		///\brief Deactivate operation for a certain element type
		void reject( XMLScannerBase::ElementType e)					{neg |= (1<<(unsigned short)e);}
		bool hasReject( XMLScannerBase::ElementType e) const				{return (neg & (1<<(unsigned short)e)) != 0;}

		///\brief Declare an operation to match on an element type
		void match( XMLScannerBase::ElementType e)					{pos |= (1<<(unsigned short)e);}
		bool hasMatch( XMLScannerBase::ElementType e) const				{return (pos & (1<<(unsigned short)e)) != 0;}

		///\brief Declare an operation as seek operation
		void seekop( Operation op)
		{
			switch (op)
			{
				case Tag:
					this->match( XMLScannerBase::OpenTag);
					this->match( XMLScannerBase::HeaderStart);
					break;
				case Attribute:
					this->match( XMLScannerBase::TagAttribName);
					this->match( XMLScannerBase::HeaderAttribName);
					this->reject( XMLScannerBase::Content);
					break;
				case ThisAttributeValue:
					this->match( XMLScannerBase::TagAttribValue);
					this->match( XMLScannerBase::HeaderAttribValue);
					this->reject( XMLScannerBase::TagAttribName);
					this->reject( XMLScannerBase::HeaderAttribName);
					this->reject( XMLScannerBase::Content);
					this->reject( XMLScannerBase::OpenTag);
					break;
				case AttributeValue:
					this->match( XMLScannerBase::TagAttribValue);
					this->match( XMLScannerBase::HeaderAttribValue);
					this->reject( XMLScannerBase::Content);
					break;
				case Content:
					this->match( XMLScannerBase::Content);
					break;
				case ContentStart:
					this->match( XMLScannerBase::HeaderEnd);
					break;
			}
		}

		const char* seekopName() const
		{
			if (this->hasMatch( XMLScannerBase::OpenTag)
			&&  this->hasMatch( XMLScannerBase::HeaderStart))
				return "Tag";

			if (this->hasMatch( XMLScannerBase::TagAttribName)
			&&  this->hasMatch( XMLScannerBase::HeaderAttribName)
			&&  this->hasReject( XMLScannerBase::Content))
				return "Attribute";

			if (this->hasMatch( XMLScannerBase::TagAttribValue)
			&&  this->hasMatch( XMLScannerBase::HeaderAttribValue)
			&&  this->hasReject( XMLScannerBase::Content))
				return "AttributeValue";

			if (this->hasMatch( XMLScannerBase::TagAttribValue)
			&&  this->hasMatch( XMLScannerBase::HeaderAttribValue)
			&&  this->hasReject( XMLScannerBase::TagAttribName)
			&&  this->hasReject( XMLScannerBase::HeaderAttribName)
			&&  this->hasReject( XMLScannerBase::Content)
			&&  this->hasReject( XMLScannerBase::OpenTag))
				return "ThisAttributeValue";

			if (this->hasMatch( XMLScannerBase::Content))
				return "Content";

			if (this->hasMatch( XMLScannerBase::HeaderEnd))
				return "ContentStart";

			if (pos == 0 && neg == 0)
				return "None";

			return "";
		}

		///\brief Join two mask definitions
		///\param[in] mask definition of mask to join this with
		void join( const Mask& mask)				{pos |= mask.pos; neg |= mask.neg;}

		///\brief Check if an element type matches the mask
		///\param[in] e element type to check
		bool matches( XMLScannerBase::ElementType e) const	{return (0 != (pos & (1<<(unsigned short)e)));}

		///\brief Check if an element type should reset a mask
		///\param[in] e element type to check
		bool rejects( XMLScannerBase::ElementType e) const	{return (0 != (neg & (1<<(unsigned short)e)));}
	};

	///\class Core
	///\brief Core of an automaton state definition that is used during XML processing
	struct Core
	{
		Mask mask;			//< mask definiting what tokens are matching this state
		bool follow;			//< true, if the state is seeking tokens in all follow scopes in the XML tree
		int typeidx;			//< type of the element emitted by this state on a match
		int cnt_start;			//< lower bound of the element index matching (for index ranges)
		int cnt_end;			//< upper bound of the element index matching (for index ranges)

		///\brief Constructor
		Core()			:follow(false),typeidx(0),cnt_start(0),cnt_end(-1) {}
		///\brief Copy constructor
		///\param [in] o element to copy
		Core( const Core& o)	:mask(o.mask),follow(o.follow),typeidx(o.typeidx),cnt_start(o.cnt_start),cnt_end(o.cnt_end) {}
	};

	///\class State
	///\brief State of an automaton in its definition
	struct State
	{
		Core core;			//< core of the state (the part used in processing)
		unsigned int keysize;		//< key size of the element
		char* key;			//< key of the element
		char* srckey;			//< key of the element as in source (for debugging or reporting, etc.)
		int next;			//< follow state
		int link;			//< alternative state to check

		///\brief Constructor
		State()
			:keysize(0),key(0),srckey(0),next(-1),link(-1) {}

		///\brief Copy constructor
		///\param [in] orig element to copy
		State( const State& orig)
			:core(orig.core),keysize(orig.keysize),key(0),srckey(0),next(orig.next),link(orig.link)
		{
			defineKey( orig.keysize, orig.key, orig.srckey);
		}

		///\brief Destructor
		~State()
		{
			if (key) delete [] key;
			if (srckey) delete [] srckey;
		}

		///\brief Check it the state definition is empty
		///\return true for an empty state
		bool isempty()				{return key==0&&core.typeidx==0;}

		///\brief Define the matching key of this state
		///\param[in] p_keysize size of the key in bytes
		///\param[in] p_key pointer to the key
		///\param[in] p_srckey the source form of the key (ASCII with encoded entities for everything else)
		void defineKey( unsigned int p_keysize, const char* p_key, const char* p_srckey)
		{
			unsigned int ii;
			if (key)
			{
				delete [] key;
				key = 0;
			}
			if (srckey)
			{
				delete [] srckey;
				srckey = 0;
			}
			if (p_key)
			{
				key = new char[ keysize=p_keysize];
				for (ii=0; ii<keysize; ii++) key[ii]=p_key[ii];
			}
			if (p_srckey)
			{
				for (ii=0; p_srckey[ii]!=0; ii++);
				srckey = new char[ ii+1];
				for (ii=0; p_srckey[ii]!=0; ii++) srckey[ii]=p_srckey[ii];
				srckey[ ii] = 0;
			}
		}

		///\brief Define a state transition by key and operation
		///\param[in] op operation type
		///\param[in] p_keysize size of the key in bytes
		///\param[in] p_key pointer to the key
		///\param[in] p_srckey the source form of the key (ASCII with encoded entities for everything else)
		///\param[in] p_next follow state on a match
		///\param[in] p_follow true if the search reaches all included follow scopes of the definition scope
		void defineNext( Operation op, unsigned int p_keysize, const char* p_key, const char* p_srckey, int p_next, bool p_follow=false)
		{
			core.mask.seekop( op);
			defineKey( p_keysize, p_key, p_srckey);
			next = p_next;
			core.follow = p_follow;
		}

		///\brief Define an element output operation
		///\param[in] mask mask defining the element types to output
		///\param[in] p_typeidx the type of the element produced
		///\param[in] p_follow true if the output reaches all included follow scopes of the definition scope
		///\param[in] p_start start index of the element range produced
		///\param[in] p_end upper bound index of the element range produced
		void defineOutput( const Mask& mask, int p_typeidx, bool p_follow, int p_start, int p_end)
		{
			core.mask = mask;
			core.typeidx = p_typeidx;
			core.cnt_end = p_end;
			core.cnt_start = p_start;
			core.follow = p_follow;
		}

		///\brief Link another state to check to the current state
		///\param[in] p_link the index of the state to link
		void defLink( int p_link)
		{
			link = p_link;
		}

		std::string tostring() const
		{
			std::ostringstream rt;
			if (next >= 0) rt << " ->" << next;
			if (link >= 0) rt << " ~" << link;
			rt << ' ';
			if (core.follow)
			{
				rt << '/';
			}
			rt << '/';
			rt << core.mask.seekopName();
			if (srckey)
			{
				rt << " '" << srckey << "'";
			}
			else
			{
				rt << " (null)";
			}
			if (core.cnt_end > 0)
			{
				rt << '[' << core.cnt_start << ',' << rt << core.cnt_end << ']';
			}
			if (core.typeidx)
			{
				rt << " =>" << core.typeidx;
			}
			return rt.str();
		}
	};
	std::vector<State> states;				//< the states of the statemachine

	///\brief Returns the content of the automaton as pretty printed string for debug output
	std::string tostring() const
	{
		std::ostringstream rt;
		typename std::vector<State>::const_iterator ii=states.begin(), ee=states.end();
		for (; ii != ee; ++ii)
		{
			rt << (int)(ii-states.begin()) << ": " << ii->tostring() << std::endl;
		}
		return rt.str();
	}

	///\class Token
	///\brief Active or passive but still valid token of the XML processing (this is a trigger waiting to match)
	struct Token
	{
		Core core;					//< core of the state
		int stateidx;					//< index into the automaton, poiting to the state

		///\brief Constructor
		Token()						:stateidx(-1) {}
		///\brief Copy constructor
		Token( const Token& orig)			:core(orig.core),stateidx(orig.stateidx) {}
		///\brief Constructor by value
		///\param [in] state state that generated this token
		///\param [in] p_stateidx index of the state that generated this token
		Token( const State& state, int p_stateidx)	:core(state.core),stateidx(p_stateidx) {}
	};

	///\class Scope
	///\brief Tag scope definition
	struct Scope
	{
		Mask mask;					//< joined mask of all tokens active in this scope
		Mask followMask;				//< joined mask of all tokens active in this and all sub scopes of this scope

		///\class Range
		///\brief Range on the token stack with all tokens that belong to this scope
		struct Range
		{
			unsigned int tokenidx_from;		//< lower bound token index
			unsigned int tokenidx_to;		//< upper bound token index
			unsigned int followidx;			//< pointer to follow token stack with tokens active in this and all sub scopes of this scope

			///\brief Constructor
			Range()				:tokenidx_from(0),tokenidx_to(0),followidx(0) {}
			///\brief Copy constructor
			///\param[in] orig scope to copy
			Range( const Scope& orig)	:tokenidx_from(orig.tokenidx_from),tokenidx_to(orig.tokenidx_to),followidx(orig.followidx) {}
		};
		Range range;							//< valid (active) token range of this scope (on the token stacks)

		///\brief Copy constructor
		///\param[in] orig scope to copy
		Scope( const Scope& orig)		:mask(orig.mask),followMask(orig.followMask),range(orig.range) {}
		///\brief Assignement operator
		///\param[in] orig scope to copy
		Scope& operator =( const Scope& orig)	{mask=orig.mask; followMask=orig.followMask; range=orig.range; return *this;}
		///\brief Constructor
		Scope()					{}
	};

	///\brief Defines the usage of memory
	///\param [in] p_memUsage size of the memory block in bytes
	///\param [in] p_maxDepth maximum depht of the scope stack
	///\return true, if everything is OK
	bool setMemUsage( std::size_t p_memUsage, unsigned int p_maxDepth)
	{
		memUsage = p_memUsage;
		maxDepth = p_maxDepth;
		maxScopeStackSize = maxDepth;
		if (p_memUsage < maxScopeStackSize * sizeof(Scope))
		{
			maxScopeStackSize = 0;
		}
		else
		{
			p_memUsage -= maxScopeStackSize * sizeof(Scope);
		}
		maxFollows = (p_memUsage / sizeof(std::size_t)) / 32 + 2;
		maxTriggers = (p_memUsage / sizeof(std::size_t)) / 32 + 3;
		p_memUsage -= sizeof(std::size_t) * maxFollows + sizeof(std::size_t) * maxTriggers;
		maxTokens = p_memUsage / sizeof(Token);
		return (maxScopeStackSize != 0 && maxTokens != 0 && maxFollows != 0 && maxTriggers != 0);
	}

private:
	///\brief Defines a state transition
	///\param [in] stateidx from what source state
	///\param [in] op operation firing the state transition
	///\param [in] keysize length of the key firing the state transition in bytes
	///\param [in] key the key string firing the state transition in bytes
	///\param [in] srckey the ASCII encoded representation in the source
	///\param [in] follow true, uf the state transition is active for all sub scopes of the activation state
	///\return the target state of the transition defined
	int defineNext( int stateidx, Operation op, unsigned int keysize, const char* key, const char* srckey, bool follow=false) throw(exception)
	{
		try
		{
			State state;
			if (states.size() == 0)
			{
				stateidx = states.size();
				states.push_back( state);
			}
			for (int ee=stateidx; ee != -1; stateidx=ee,ee=states[ee].link)
			{
				if (states[ee].key != 0 && keysize == states[ee].keysize && states[ee].core.follow == follow)
				{
					unsigned int ii;
					for (ii=0; ii<keysize && states[ee].key[ii]==key[ii]; ii++);
					if (ii == keysize) return states[ee].next;
				}
			}
			if (!states[stateidx].isempty())
			{
				stateidx = states[stateidx].link = states.size();
				states.push_back( state);
			}
			states.push_back( state);
			unsigned int lastidx = states.size()-1;
			states[ stateidx].defineNext( op, keysize, key, srckey, lastidx, follow);
			return stateidx=lastidx;
		}
		catch (std::bad_alloc)
		{
			throw exception( OutOfMem);
		}
		catch (...)
		{
			throw exception( Unknown);
		}
	}

	///\brief Defines an output print action and output type for a state
	///\param [in] stateidx from what source state
	///\param [in] printOpMask mask for elements printed
	///\param [in] typeidx type identifier
	///\param [in] follow true, uf the state transition is active for all sub scopes of the activation state
	///\param [in] start start of index range where this state transition fires
	///\param [in] end end of index range where this state transition fires
	///\return index of the state where this output action was defined
	int defineOutput( int stateidx, const Mask& printOpMask, int typeidx, bool follow, int start, int end) throw(exception)
	{
		try
		{
			State state;
			if (states.size() == 0)
			{
				stateidx = states.size();
				states.push_back( state);
			}
			if ((unsigned int)stateidx >= states.size()) throw exception( IllegalParam);

			if (!states[stateidx].isempty())
			{
				stateidx = states[stateidx].link = states.size();
				states.push_back( state);
			}
			states[ stateidx].defineOutput( printOpMask, typeidx, follow, start, end);
			return stateidx;
		}
		catch (std::bad_alloc)
		{
			throw exception( OutOfMem);
		}
		catch (...)
		{
			throw exception( Unknown);
		}
	}

public:
	///\class PathElement
	///\brief Defines one node in the XML Path element tree in the construction phase.
	///\remark This is just a construct for building the tree with cascading operators forming a path representation
	struct PathElement :throws_exception
	{
	private:
		XMLPathSelectAutomaton* xs;		//< XML Path select automaton where this node is an element of
		int stateidx;				//< state of this element in the automaton

		///\class Range
		///\brief Element counting range defining what are indices of valid elements
		struct Range
		{
			int start;			//< index of starting element starting with 0
			int end;			//< index of upper boundary element (not belonging to range anymore). -1 if undefined (unlimited)

			///\brief Copy constructor
			///\param [in] o range element to copy
			Range( const Range& o)		:start(o.start),end(o.end){}
			///\brief Constructor by value
			///\param [in] p_start index of starting element
			///\param [in] p_end index of upper boundary element (not belonging to range anymore). -1 if undefined (unlimited)
			Range( int p_start, int p_end)	:start(p_start),end(p_end){}
			///\brief Constructor by value
			///\param [in] count number of elements starting with the first one (with index 0)
			Range( int count)		:start(0),end(count){}
			///\brief Constructor
			Range()				:start(0),end(-1){}
		};
		Range range;			//< Index range of this XML path element
		bool follow;			//< true, if this element is active (firing) for all sub scopes of the activation scope
		Mask pushOpMask;		//< mask for firing element actions
		Mask printOpMask;		//< mask for printing element actions

	private:
		///\brief Define an output operation for a certain element type in this state
		///\param [in] op XML operation type of this output
		///\return *this
		PathElement& defineOutput( Operation op)
		{
			printOpMask.reset();
			printOpMask.seekop( op);
			return *this;
		}

		///\brief Define a state transition operation for a token of a certain element type in this state
		///\param [in] op XML operation type of this state transition
		///\param [in] value key value as ASCII with encoded entities for higher unicode characters of this state transition
		///\return *this
		PathElement& doSelect( Operation op, const char* value) throw(exception)
		{
			static XMLScannerBase::IsTagCharMap isTagCharMap;
			if (xs != 0)
			{
				if (value)
				{
					char buf[ 1024];
					StaticBuffer pb( buf, sizeof(buf));
					char* itr = const_cast<char*>(value);
					typedef XMLScanner<char*,CharSet,CharSet,StaticBuffer> StaticXMLScanner;
					if (!StaticXMLScanner::parseStaticToken( isTagCharMap, itr, pb))
					{
						throw exception( IllegalAttributeName);
					}
					stateidx = xs->defineNext( stateidx, op, pb.size(), pb.ptr(), value, follow);
				}
				else
				{
					stateidx = xs->defineNext( stateidx, op, 0, 0, 0, follow);
				}
			}
			return *this;
		}

		///\brief Define this element as active (firing,printing) for all sub scopes of the activation scope
		///\return *this
		PathElement& doFollow()
		{
			follow = true;
			return *this;
		}

		///\brief Define a valid range of token count for this element to be active
		///\param [in] p_start index of starting element starting with 0
		///\param [in] p_end index of upper boundary element (not belonging to range anymore). -1 if undefined (unlimited)
		///\return *this
		PathElement& doRange( int p_start, int p_end)
		{
			if (range.end == -1)
			{
				range = Range( p_start, p_end);
			}
			else if (p_end < range.end)
			{
				range.end = p_end;
			}
			else if (p_start > range.start)
			{
				range.start = p_start;
			}
			return *this;
		}

		///\brief Define a valid range of token count for this element to be active by the number of elements
		///\param [in] p_count number of elements starting with 0
		///\return *this
		PathElement& doCount( int p_count)
		{
			return doRange( 0, p_count);
		}

		///\brief Define the start of the range of token count for this element to be active
		///\param [in] p_start index of starting element starting with 0
		///\return *this
		PathElement& doStart( int p_start)
		{
			return doRange( p_start, std::numeric_limits<int>::max());
		}

		///\brief Define the output of the current element
		///\param [in] typeidx type of the element produced
		///\return *this
		PathElement& push( int typeidx) throw(exception)
		{
			if (xs != 0) stateidx = xs->defineOutput( stateidx, printOpMask, typeidx, follow, range.start, range.end);
			return *this;
		}

	public:
		///\brief Constructor
		PathElement()							:xs(0),stateidx(0),follow(false),pushOpMask(0),printOpMask(0){}
		///\brief Constructor by values
		///\param [in] p_xs automaton of this element
		///\param [in] p_si state index of this element in the automaton definition
		PathElement( XMLPathSelectAutomaton* p_xs, int p_si=0)		:xs(p_xs),stateidx(p_si),follow(false),pushOpMask(0),printOpMask(0){}
		///\brief Copy constructor
		///\param [in] orig element to copy
		PathElement( const PathElement& orig)				:xs(orig.xs),stateidx(orig.stateidx),range(orig.range),follow(orig.follow),pushOpMask(orig.pushOpMask),printOpMask(orig.printOpMask) {}

		///\brief Corresponds to "//" in abbreviated syntax of XPath
		///\return *this
		PathElement& operator --(int)							{return doFollow();}
		///\brief Find tag by name
		///\param [in] name name of the tag
		///\return *this
		///\remark same as selectTag(const char*)
		PathElement& operator []( const char* name) throw(exception)			{return doSelect( Tag, name);}
		///\brief Find tag by name
		///\param [in] name name of the tag
		///\return *this
		PathElement& selectTag( const char* name) throw(exception)			{return doSelect( Tag, name);}

		///\brief Find tag with one attribute
		///\param [in] name name of the attribute
		///\return *this
		///\remark same as selectAttribute(const char*)
		PathElement& operator ()( const char* name) throw(exception)			{return doSelect( Attribute, name).defineOutput( ThisAttributeValue);}
		///\brief Find tag with one attribute
		///\param [in] name name of the attribute
		///\return *this
		PathElement& selectAttribute( const char* name) throw(exception)		{return doSelect( Attribute, name).defineOutput( ThisAttributeValue);}

		///\brief Find tag with one attribute,value condition
		///\remark same as ifAttribute(const char*,const char*)
		///\param [in] name name of the attribute
		///\param [in] value value of the attribute
		///\return *this
		PathElement& operator ()( const char* name, const char* value) throw(exception)	{return doSelect( Attribute, name).doSelect( ThisAttributeValue, value);}

		///\brief Find tag with one attribute,value condition
		///\param [in] name name of the attribute
		///\param [in] value value of the attribute
		///\return *this
		PathElement& ifAttribute( const char* name, const char* value) throw(exception)	{return doSelect( Attribute, name).doSelect( ThisAttributeValue, value);}

		///\brief Define maximum element index to push
		///\param [in] idx maximum element index
		///\return *this
		PathElement& TO(int idx) throw(exception)					{return doCount((idx>=0)?(idx+1):-1);}
		///\brief Define minimum element index to push
		///\param [in] idx minimum element index
		///\return *this
		PathElement& FROM(int idx) throw(exception)					{return doStart(idx); return *this;}
		///\brief Define minimum and maximum element index to push
		///\param [in] idx1 minimum element index
		///\param [in] idx2 maximum element index
		///\return *this
		PathElement& RANGE(int idx1, int idx2) throw(exception)				{return doRange(idx1,(idx2>=0)?(idx2+1):-1); return *this;}
		///\brief Define index of the element index to push
		///\param [in] idx element index
		///\return *this
		PathElement& INDEX(int idx) throw(exception)					{return doRange(idx,idx+1); return *this;}

		///\brief Define element type to push
		///\param [in] type element type
		///\return *this
		///\remark same as assignType(int)
		PathElement& operator =(int type) throw(exception)				{return push( type);}
		///\brief Define element type to push
		///\param [in] type element type
		///\return *this
		PathElement& assignType(int type) throw(exception)				{return push( type);}

		///\brief Define grab content
		///\remark same as selectContent()
		///\return *this
		PathElement& operator ()()  throw(exception)					{return defineOutput(Content);}
		///\brief Define grab content
		///\return *this
		PathElement& selectContent()  throw(exception)					{return defineOutput(Content);}
	};

	///\brief Get automaton root element to start an XML path definition
	///\return the automaton root element
	PathElement operator*()
	{
		return PathElement( this);
	}
};

} //namespace
#endif
