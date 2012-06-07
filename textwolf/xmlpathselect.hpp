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

    Copyright (C) 2010 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

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
#ifndef __TEXTWOLF_XML_PATH_SELECT_HPP__
#define __TEXTWOLF_XML_PATH_SELECT_HPP__
#include "textwolf/char.hpp"
#include "textwolf/charset_interface.hpp"
#include "textwolf/exception.hpp"
#include "textwolf/xmlscanner.hpp"
#include "textwolf/staticbuffer.hpp"
#include <limits>
#include <string>
#include <vector>
#include <map>
#include <cstddef>

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
			:memUsage(defaultMemUsage),maxDepth(defaultMaxDepth),maxScopeStackSize(0),maxFollows(0),maxTriggers(0),maxTokens(0)
	{
		if (!setMemUsage( memUsage, maxDepth)) throw exception( DimOutOfRange);
	}
	typedef CharSet_ CharSet;
	typedef int Hash;
	typedef XMLPathSelectAutomaton<CharSet> ThisXMLPathSelectAutomaton;

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

		///\brief Declare an operation to match on an element type
		void match( XMLScannerBase::ElementType e)					{pos |= (1<<(unsigned short)e);}

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
		State( const State& orig)		:core(orig.core),keysize(orig.keysize),key(0),srckey(0),next(orig.next),link(orig.link)
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
	};
	std::vector<State> states;							//< the states of the statemachine

	///\class Token
	///\brief Active or passive but still valid token of the XML processing (this is a trigger waiting to match)
	struct Token
	{
		Core core;											//< core of the state
		int stateidx;										//< index into the automaton, poiting to the state

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
		int stateidx;							//< state of this element in the automaton

		///\class Range
		///\brief Element counting range defining what are indices of valid elements
		struct Range
		{
			int start;							//< index of starting element starting with 0
			int end;								//< index of upper boundary element (not belonging to range anymore). -1 if undefined (unlimited)

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
		PathElement& operator []( const char* name) throw(exception)			{return doSelect( Tag, name);}
		///\brief Find tag by name
		///\remark same as selectTag(const char*)
		///\param [in] name name of the tag
		///\return *this
		PathElement& selectTag( const char* name) throw(exception)			{return doSelect( Tag, name);}
		///\brief Find tag with one attribute
		///\param [in] name name of the attribute
		///\return *this
		PathElement& operator ()( const char* name) throw(exception)			{return doSelect( Attribute, name).defineOutput( ThisAttributeValue);}
		///\brief Find tag with one attribute
		///\remark same as selectAttribute(const char*)
		///\param [in] name name of the attribute
		///\return *this
		PathElement& selectAttribute( const char* name) throw(exception)		{return doSelect( Attribute, name).defineOutput( ThisAttributeValue);}

		//\brief Find tag with one attribute,value condition
		///\remark same as ifAttribute(const char*,const char*)
		///\param [in] name name of the attribute
		///\param [in] value value of the attribute
		///\return *this
		PathElement& operator ()( const char* name, const char* value) throw(exception)	{return doSelect( Attribute, name).doSelect( ThisAttributeValue, value);}
		//\brief Find tag with one attribute,value condition
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
		PathElement& RANGE(int idx1, int idx2) throw(exception)		{return doRange(idx1,(idx2>=0)?(idx2+1):-1); return *this;}
		///\brief Define element type to push
		///\remark same as assignType(int)
		///\param [in] type element type
		///\return *this
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

///\brief XML path select template
///\tparam InputIterator input iterator with ++ and read only * returning 0 als last character of the input
///\tparam InputCharSet_ character set encoding of the input, read as stream of bytes
///\tparam OutputCharSet_ character set encoding of the output, printed as string of the item type of the character set,
///\tparam OutputBuffer_ buffer for output with STL back insertion sequence interface (e.g. std::string,std::vector<char>,textwolf::StaticBuffer)
template <
		class InputIterator,
		class InputCharSet_,
		class OutputCharSet_,
		class OutputBuffer_
>
class XMLPathSelect :public throws_exception
{
public:
	typedef OutputBuffer_ OutputBuffer;
	typedef XMLPathSelectAutomaton<OutputCharSet_> ThisXMLPathSelectAutomaton;
	typedef XMLScanner<InputIterator,InputCharSet_,OutputCharSet_,OutputBuffer> ThisXMLScanner;
	typedef XMLPathSelect<InputIterator,InputCharSet_,OutputCharSet_,OutputBuffer> ThisXMLPathSelect;
	typedef std::map<const char*,UChar> EntityMap;

private:
	ThisXMLScanner scan;				//< XML Scanner for fetching elements for the automaton input
	const ThisXMLPathSelectAutomaton* atm;		//< XML select automaton used
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

	Array<Scope> scopestk;		//< stack of scopes opened
	Array<unsigned int> follows;	//< indices of tokens active in all descendant scopes
	Array<int> triggers;		//< triggered elements
	Array<Token> tokens;		//< list of waiting tokens

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
	///\param[in] p_src source input iterator to process
	///\param[in] obuf reference to buffer to use for the output elements (STL back insertion sequence interface)
	///\param[in] entityMap read only map of named entities to expand
	XMLPathSelect( const ThisXMLPathSelectAutomaton* p_atm, InputIterator& p_src, const EntityMap& entityMap)
		:scan(p_src,entityMap),atm(p_atm),scopestk(p_atm->maxScopeStackSize),follows(p_atm->maxFollows),triggers(p_atm->maxTriggers),tokens(p_atm->maxTokens)
	{
		if (atm->states.size() > 0) expand(0);
	}
	///\brief Constructor
	///\param[in] p_atm read only ML path select automaton reference
	///\param[in] obuf reference to buffer to use for the output elements (STL back insertion sequence interface)
	///\param[in] entityMap read only map of named entities to expand
	XMLPathSelect( const ThisXMLPathSelectAutomaton* p_atm, const EntityMap& entityMap)
		:scan(entityMap),atm(p_atm),scopestk(p_atm->maxScopeStackSize),follows(p_atm->maxFollows),triggers(p_atm->maxTriggers),tokens(p_atm->maxTokens)
	{
		if (atm->states.size() > 0) expand(0);
	}
	///\brief Constructor
	///\param[in] p_atm read only ML path select automaton reference
	///\param[in] p_src source input iterator to process
	XMLPathSelect( const ThisXMLPathSelectAutomaton* p_atm, InputIterator& p_src)
		:scan(p_src),atm(p_atm),scopestk(p_atm->maxScopeStackSize),follows(p_atm->maxFollows),triggers(p_atm->maxTriggers),tokens(p_atm->maxTokens)
	{
		if (atm->states.size() > 0) expand(0);
	}
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
		:scan(o.scan),atm(o.atm),scopestk(o.maxScopeStackSize),follows(o.maxFollows),follows(o.maxTriggers),tokens(o.maxTokens){}

	///\brief Assign something to the source iterator while keeping the state
	///\param [in] a source iterator assignment
	template <class IteratorAssignment>
	void setSource( const IteratorAssignment& a)
	{
		scan.setSource( a);
	}

	///\brief Get the current source iterator position
	///\return source iterator position in character words (usually bytes)
	std::size_t getPosition() const
	{
		return scan.getPosition();
	}

	///\brief Set the tokenization behaviour
	///\param [out] v the tokenization behaviour flag
	void doTokenize( bool v)
	{
		scan.doTokenize(v);
	}

	///\class End
	///\brief end of input iterator for the output of this XMLScanner
	struct End {};

	///\class iterator
	///\brief input iterator for the output of this XMLScanner
	class iterator
	{
	public:
		///\class Element
		///\brief visited current element data of the iterator
		class Element
		{
		public:
			///\class State
			///\brief state of the iterator
			enum State
			{
				Ok,			//< normal
				EndOfInput,		//< end of input triggered
				ErrorState		//< error occurred (identifier as string is in the output token buffer)
			};

			///\brief Constructor
			Element()				:m_state(Ok),m_type(0),m_content(0),m_size(0) {}
			///\brief Constructor for content end iterator
			Element( const End&)			:m_state(EndOfInput),m_type(0),m_content(0),m_size(0) {}
			///\brief Copy constructor
			///\param [in] orig element to copy
			Element( const Element& orig)		:m_state(orig.m_state),m_type(orig.m_type),m_content(orig.m_content),m_size(orig.m_size) {}
			///\brief Get the iterator state
			State state() const			{return m_state;}
			///\brief Get the currently visited element type
			int type() const			{return m_type;}
			///\brief Get the currently visited element content
			const char* content() const		{return m_content;}
			///\brief Get the size of the content of the currently visited element in bytes
			unsigned int size() const		{return m_size;}
		private:
			friend class iterator;			//< friend to intialize the elements
			State m_state;				//< current state
			int m_type;				//< currently visited element type
			const char* m_content;			//< currently visited element content
			unsigned int m_size;			//< size of the content of the currently visited element in bytes
		};
		typedef Element value_type;
		typedef std::size_t difference_type;
		typedef Element* pointer;
		typedef Element& reference;
		typedef std::input_iterator_tag iterator_category;

	private:
		Element element;				//< currently visited element
		ThisXMLPathSelect* input;			//< producing XML path selection stream

		///\brief Skip to next element
		///\return *this
		iterator& skip() throw(exception)
		{
			if (input != 0)
			{
				do
				{
					if (!input->context.key)
					{
						XMLScannerBase::ElementType et = input->scan.nextItem( input->context.scope.mask.pos);
						if (et == XMLScannerBase::Exit)
						{
							if (input->scopestk.size() == 0)
							{
								element.m_state = Element::EndOfInput;
							}
							else
							{
								element.m_state = Element::ErrorState;
								element.m_content = XMLScannerBase::getErrorString( XMLScannerBase::ErrUnexpectedEndOfInput);
							}
							return *this;
						}
						if (et == XMLScannerBase::ErrorOccurred)
						{
							input->scan.getError( &element.m_content);
							element.m_state = Element::ErrorState;
							return *this;
						}
						input->initProcessElement( et, input->scan.getItem(), input->scan.getItemSize());
					}
					element.m_type = input->fetch();

				} while (element.m_type == 0);

				element.m_content = input->context.key;
				element.m_size = input->context.keysize;
			}
			return *this;
		}
		///\brief Iterator compare
		///\param [in] iter iterator to compare with
		///\return true, if the elements are equal
		bool compare( const iterator& iter) const
		{
			return (element.state() != Element::Ok && iter.element.state() != Element::Ok);
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
		///\param [in] skipToFirst true, if the iterator should skip to the first character of the input (default behaviour of STL conform iterators but maybe not exception save)
		iterator( ThisXMLPathSelect& p_input, bool skipToFirst=true)
				:input( &p_input)
		{
			if (skipToFirst) skip();
		}

		///\brief Constructor
		///\param [in] et end of input tag
		iterator( const End& et)	:element(et),input(0) {}

		///\brief Constructor
		iterator()			:input(0) {}

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
		const Element& operator*()
		{
			return element;
		}

		///\brief Element acceess
		///\return read only element reference
		const Element* operator->()
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

	///\brief Get the start iterator
	///\return iterator pointing to the first of the selected XML path elements
	iterator begin( bool skipToFirst=true)
	{
		return iterator( *this, skipToFirst);
	}

	///\brief Get the end of content marker
	///\return iterator as end of content marker
	iterator end()
	{
		return iterator( End());
	}
};

}//namespace
#endif
