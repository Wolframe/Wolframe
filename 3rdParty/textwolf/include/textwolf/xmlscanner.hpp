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

    Copyright (C) 2010,2011,2012 Patrick Frey

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
#ifndef __TEXTWOLF_XML_SCANNER_HPP__
#define __TEXTWOLF_XML_SCANNER_HPP__
#include "textwolf/char.hpp"
#include "textwolf/charset_interface.hpp"
#include "textwolf/exception.hpp"
#include "textwolf/textscanner.hpp"
#include <map>
#include <cstddef>

namespace textwolf {

///\class ScannerStatemachine
///\brief Class to build up the XML element scanner state machine in a descriptive way
class ScannerStatemachine :public throws_exception
{
public:
	enum
	{
		MaxNofStates=64				//< maximum number of states (fixed allocated array for state machine)
	};
	///\class Element
	///\brief One state in the state machine
	struct Element
	{
		int fallbackState;			//< state transition if the event does not match (it belongs to the next state = fallbackState)
		int missError;				//< error code in case of an event that does not match and there is no fallback

		///\class Action
		///\brief Definition of action fired by the state machine
		struct Action
		{
			int op;				//< action operand
			int arg;			//< action argument
		};
		Action action;				//< action executed after entering this state
		unsigned char nofnext;			//< number of follow states defined
		signed char next[ NofControlCharacter];	//< follow state fired by an event (control character type parsed)

		///\brief Constructor
		Element() :fallbackState(-1),missError(-1),nofnext(0)
		{
			action.op = -1;
			action.arg = 0;
			for (unsigned int ii=0; ii<NofControlCharacter; ii++) next[ii] = -1;
		}
	};
	///\brief Get state addressed by its index
	///\param [in] stateIdx index of the state
	///\return state defintion reference
	Element* get( int stateIdx) throw(exception)
	{
		if ((unsigned int)stateIdx>size) throw exception(InvalidState);
		return tab + stateIdx;
	}

private:
	Element tab[ MaxNofStates];	//< states of the STM
	unsigned int size;		//< number of states defined in the STM

	///\brief Create a new state
	///\param [in] stateIdx index of the state (must be the size of the STM array, so that state identifiers can be named by enumeration constants for better readability)
	void newState( int stateIdx) throw(exception)
	{
		if (size != (unsigned int)stateIdx) throw exception( StateNumbersNotAscending);
		if (size >= MaxNofStates) throw exception( DimOutOfRange);
		size++;
	}

	///\brief Define a transition for all control character types not firing yet in the last state defined
	///\param [in] nextState the follow state index defined for these transitions
	void addOtherTransition( int nextState) throw(exception)
	{
		if (size == 0) throw exception( InvalidState);
		if (nextState < 0 || nextState > MaxNofStates) throw exception( InvalidParamState);
		for (unsigned int inputchr=0; inputchr<NofControlCharacter; inputchr++)
		{
			if (tab[ size-1].next[ inputchr] == -1) tab[ size-1].next[ inputchr] = (unsigned char)nextState;
		}
		tab[ size-1].nofnext = NofControlCharacter;
	}

	///\brief Define a transition for inputchr in the last state defined
	///\param [in] inputchr the firing input control character type
	///\param [in] nextState the follow state index defined for this transition
	void addTransition( ControlCharacter inputchr, int nextState) throw(exception)
	{
		if (size == 0) throw exception( InvalidState);
		if ((int)inputchr >= (int)NofControlCharacter) throw exception( InvalidParamChar);
		if (nextState < 0 || nextState > MaxNofStates) throw exception( InvalidParamState);
		if (tab[ size-1].next[ inputchr] != -1) throw exception( DuplicateStateTransition);
		tab[ size-1].next[ inputchr] = (unsigned char)nextState;
		tab[ size-1].nofnext += 1;
	}

	///\brief Define a self directing transition for inputchr in the last state defined (the state remains the same for this input)
	///\param [in] inputchr the firing input control character type
	void addTransition( ControlCharacter inputchr) throw(exception)
	{
		addTransition( inputchr, size-1);
	}

	///\brief Define an action in the last state defined (to be executed when entering the state)
	///\param [in] action_op action operand
	///\param [in] action_arg action argument
	void addAction( int action_op, int action_arg=0) throw(exception)
	{
		if (size == 0) throw exception( InvalidState);
		if (tab[ size-1].action.op != -1) throw exception( InvalidState);
		tab[ size-1].action.op = action_op;
		tab[ size-1].action.arg = action_arg;
	}

	///\brief Define an error in the last state defined to be reported when no fallback is defined and no firing input character parsed
	///\param [in] error code to be reported
	void addMiss( int error) throw(exception)
	{
		if (size == 0) throw exception( InvalidState);
		if (tab[ size-1].missError != -1) throw exception( InvalidState);
		tab[ size-1].missError = error;
	}

	///\brief Define in the last state defined a fallback state transition that is fired when no firing input character parsed
	///\param [in] stateIdx follow state index
	void addFallback( int stateIdx) throw(exception)
	{
		if (size == 0) throw exception( InvalidState);
		if (tab[ size-1].fallbackState != -1) throw exception( InvalidState);
		if (stateIdx < 0 || stateIdx > MaxNofStates) throw exception( InvalidParamState);
		tab[ size-1].fallbackState = stateIdx;
	}
public:
	///\brief Constructor
	ScannerStatemachine() :size(0){}

	///\brief See ScannerStatemachine::newState(int)
	ScannerStatemachine& operator[]( int stateIdx)									{newState(stateIdx); return *this;}
	///\brief See ScannerStatemachine::addTransition(ControlCharacter,int)
	ScannerStatemachine& operator()( ControlCharacter inputchr, int ns)						{addTransition(inputchr,ns); return *this;}
	///\brief See ScannerStatemachine::addTransition(ControlCharacter,int)
	ScannerStatemachine& operator()( ControlCharacter i1, ControlCharacter i2, int ns)				{addTransition(i1,ns); addTransition(i2,ns); return *this;}
	///\brief See ScannerStatemachine::addTransition(ControlCharacter,int)
	ScannerStatemachine& operator()( ControlCharacter i1, ControlCharacter i2, ControlCharacter i3, int ns)		{addTransition(i1,ns); addTransition(i2,ns); addTransition(i3,ns); return *this;}
	///\brief See ScannerStatemachine::addTransition(ControlCharacter)
	ScannerStatemachine& operator()( ControlCharacter inputchr)							{addTransition(inputchr); return *this;}
	///\brief See ScannerStatemachine::addAction(int,int)
	ScannerStatemachine& action( int aa, int arg=0)									{addAction(aa,arg); return *this;}
	///\brief See ScannerStatemachine::addMiss(int)
	ScannerStatemachine& miss( int ee)										{addMiss(ee); return *this;}
	///\brief See ScannerStatemachine::addFallback(int)
	ScannerStatemachine& fallback( int stateIdx)									{addFallback(stateIdx); return *this;}
	///\brief See ScannerStatemachine::addOtherTransition(int)
	ScannerStatemachine& other( int stateIdx)									{addOtherTransition(stateIdx); return *this;}
};

///\class XMLScannerBase
///\brief XML scanner base class for things common for all XML scanners
class XMLScannerBase
{
public:
	///\enum ElementType
	///\brief Enumeration of XML element types returned by an XML scanner
	enum ElementType
	{
		None,					//< empty (NULL)
		ErrorOccurred,				//< XML scanning error error reported
		HeaderStart,				//< open XML header tag
		HeaderAttribName,			//< tag attribute name in the XML header
		HeaderAttribValue,			//< tag attribute value in the XML header
		HeaderEnd,				//< end of XML header event (after parsing '?&gt;')
		DocAttribValue,				//< document attribute value in a DOCTYPE or ENTITY definition
		DocAttribEnd,				//< end of a document attribute definition <! .. !>
		TagAttribName,				//< tag attribute name (e.g. "id" in &lt;person id='5'&gt;
		TagAttribValue,				//< tag attribute value (e.g. "5" in &lt;person id='5'&gt;
		OpenTag,				//< open tag (e.g. "bla" for "&lt;bla...")
		CloseTag,				//< close tag (e.g. "bla" for "&lt;/bla&gt;")
		CloseTagIm,				//< immediate close tag (e.g. "bla" for "&lt;bla /&gt;")
		Content,				//< content element string (separated by spaces or end of line)
		Exit					//< end of document
	};
	enum
	{
		NofElementTypes=Exit+1		//< number of XML element types defined
	};

	///\brief Get the XML element type as string
	///\param [in] ee XML element type
	///\return XML element type as string
	static const char* getElementTypeName( ElementType ee)
	{
		static const char* names[ NofElementTypes] = {"None","ErrorOccurred","HeaderStart","HeaderAttribName","HeaderAttribValue","HeaderEnd", "DocAttribValue", "DocAttribEnd", "TagAttribName","TagAttribValue","OpenTag","CloseTag","CloseTagIm","Content","Exit"};
		return names[ (unsigned int)ee];
	}

	///\enum Error
	///\brief Enumeration of XML scanner error codes
	enum Error
	{
		Ok,					//< no error, everything is OK
		ErrIllegalDocumentAttributeDef,		//< error in document attribute or entity definition
		ErrExpectedOpenTag,			//< expected an open tag in this state
		ErrExpectedXMLTag,			//< expected an <?xml tag in this state
		ErrUnexpectedEndOfText,			//< unexpected end of text in the middle of the XML definition
		ErrSyntaxToken,				//< a specific string expected as token in XML but does not match
		ErrStringNotTerminated,			//< attribute string in XML not terminated on the same line
		ErrUndefinedCharacterEntity,		//< named entity is not defined in the entity map
		ErrExpectedTagEnd,			//< expected end of tag
		ErrExpectedEqual,			//< expected equal in tag attribute definition
		ErrExpectedTagAttribute,		//< expected tag attribute
		ErrExpectedCDATATag,			//< expected CDATA tag definition
		ErrInternal,				//< internal error (textwolf implementation error)
		ErrUnexpectedEndOfInput,		//< unexpected end of input stream
		ErrExpectedEndOfLine,			//< expected mandatory end of line (after XML header)
		ErrExpectedDash2			//< expected second '-' after '<!-' to start an XML comment as '<!-- ... -->'
	};

	///\brief Get the error code as string
	///\param [in] ee error code
	///\return the error code as string
	static const char* getErrorString( Error ee)
	{
		enum {NofErrors=16};
		static const char* sError[NofErrors]
			= {0,"illegal document attribute definition",
				"expected open tag",
				"expected XML tag",
				"unexpected end of text",
				"syntax token",
				"string not terminated",
				"undefined character entity",
				"expected tag end",
				"expected equal",
				"expected tag attribute",
				"expected CDATA tag",
				"internal",
				"unexpected end of input",
				"expected end of line",
				"expected 2nd '-' to complete marker for start of comment '<!--'"
		};
		return sError[(unsigned int)ee];
	}

	///\enum STMState
	///\brief Enumeration of states of the XML scanner state machine
	enum STMState
	{
		START, STARTTAG, XTAG, PITAG, PITAGEND, XTAGEND, XTAGDONE, XTAGAISK, XTAGANAM, XTAGAESK, XTAGAVSK, XTAGAVID, XTAGAVSQ, XTAGAVDQ, XTAGAVQE,
		DOCSTART, CONTENT, TOKEN, SEEKTOK, XMLTAG, OPENTAG, CLOSETAG, TAGCLSK, TAGAISK, TAGANAM, TAGAESK, TAGAVSK, TAGAVID, TAGAVSQ, TAGAVDQ, TAGAVQE,
		TAGCLIM, ENTITYSL, ENTITY, ENTITYE, ENTITYID, ENTITYSQ, ENTITYDQ, ENTITYLC, 
		COMDASH2, COMSEEKE, COMENDD2, COMENDCL, CDATA, CDATA1, CDATA2, CDATA3, EXIT
	};

	///\brief Get the scanner state machine state as string
	///\param [in] s the state
	///\return the state as string
	static const char* getStateString( STMState s)
	{
		enum Constant {NofStates=48};
		static const char* sState[NofStates]
		= {
			"START", "STARTTAG", "XTAG", "PITAG", "PITAGEND",
			"XTAGEND", "XTAGDONE", "XTAGAISK", "XTAGANAM",
			"XTAGAESK", "XTAGAVSK", "XTAGAVID", "XTAGAVSQ", "XTAGAVDQ",
			"XTAGAVQE", "DOCSTART", "CONTENT", "TOKEN", "SEEKTOK", "XMLTAG",
			"OPENTAG", "CLOSETAG", "TAGCLSK", "TAGAISK", "TAGANAM",
			"TAGAESK", "TAGAVSK", "TAGAVID", "TAGAVSQ", "TAGAVDQ",
			"TAGAVQE", "TAGCLIM", "ENTITYSL", "ENTITY", "ENTITYE",
			"ENTITYID", "ENTITYSQ", "ENTITYDQ",  "ENTITYLC",
			"COMDASH2", "COMSEEKE", "COMENDD2", "COMENDCL",
			"CDATA", "CDATA1", "CDATA2", "CDATA3", "EXIT"
		};
		return sState[(unsigned int)s];
	}

	///\enum STMAction
	///\brief Enumeration of actions in the XML scanner state machine
	enum STMAction
	{
		Return, ReturnWord, ReturnContent, ReturnIdentifier, ReturnSQString, ReturnDQString, ExpectIdentifierXML, ExpectIdentifierCDATA, ReturnEOF,
		NofSTMActions = 9
	};

	///\brief Get the scanner state machine action as string
	///\param [in] a the action
	///\return the action as string
	static const char* getActionString( STMAction a)
	{
		static const char* name[ NofSTMActions] = {"Return", "ReturnWord", "ReturnContent", "ReturnIdentifier", "ReturnSQString", "ReturnDQString", "ExpectIdentifierXML", "ExpectIdentifierCDATA", "ReturnEOF"};
		return name[ (unsigned int)a];
	};

	///\class Statemachine
	///\brief XML scanner state machine implementation
	struct Statemachine :public ScannerStatemachine
	{
		///\brief Constructor (defines the state machine completely)
		Statemachine( bool doTokenize)
		{
			(*this)
			[ START    ](EndOfText,EXIT)(EndOfLine)(Cntrl)(Space)(Lt,STARTTAG).miss(ErrExpectedOpenTag)
			[ STARTTAG ](EndOfLine)(Cntrl)(Space)(Questm,XTAG)(Exclam,ENTITYSL).fallback(OPENTAG)
			[ XTAG     ].action(ExpectIdentifierXML)(EndOfLine,Cntrl,Space,XTAGAISK)(Questm,XTAGEND).miss(ErrExpectedXMLTag)
			[ PITAG    ](Questm,PITAGEND).other(PITAG)
			[ PITAGEND ](Gt,CONTENT).miss(ErrExpectedTagEnd)
			[ XTAGEND  ](Gt,XTAGDONE)(EndOfLine)(Cntrl)(Space).miss(ErrExpectedTagEnd)
			[ XTAGDONE ].action(Return,HeaderEnd).fallback(DOCSTART)
			[ XTAGAISK ](EndOfLine)(Cntrl)(Space)(Questm,XTAGEND).fallback(XTAGANAM)
			[ XTAGANAM ].action(ReturnIdentifier,HeaderAttribName)(EndOfLine,Cntrl,Space,XTAGAESK)(Equal,XTAGAVSK).miss(ErrExpectedEqual)
			[ XTAGAESK ](EndOfLine)(Cntrl)(Space)(Equal,XTAGAVSK).miss(ErrExpectedEqual)
			[ XTAGAVSK ](EndOfLine)(Cntrl)(Space)(Sq,XTAGAVSQ)(Dq,XTAGAVDQ).fallback(XTAGAVID)
			[ XTAGAVID ].action(ReturnIdentifier,HeaderAttribValue)(EndOfLine,Cntrl,Space,XTAGAISK)(Questm,XTAGEND).miss(ErrExpectedTagAttribute)
			[ XTAGAVSQ ].action(ReturnSQString,HeaderAttribValue)(Sq,XTAGAVQE).miss(ErrStringNotTerminated)
			[ XTAGAVDQ ].action(ReturnDQString,HeaderAttribValue)(Dq,XTAGAVQE).miss(ErrStringNotTerminated)
			[ XTAGAVQE ](EndOfLine,Cntrl,Space,XTAGAISK)(Questm,XTAGEND).miss(ErrExpectedTagAttribute)
			[ DOCSTART ](EndOfText,EXIT)(EndOfLine)(Cntrl)(Space)(Lt,XMLTAG).fallback(TOKEN);
			if (doTokenize)
			{
				(*this)
				[ CONTENT  ](EndOfText,EXIT)(EndOfLine)(Cntrl)(Space)(Lt,XMLTAG).fallback(TOKEN)
				[ TOKEN    ].action(ReturnWord,Content)(EndOfText,EXIT)(EndOfLine,Cntrl,Space,CONTENT)(Lt,XMLTAG).fallback(CONTENT);
			}
			else
			{
				(*this)
				[ CONTENT  ](EndOfText,EXIT)(Lt,XMLTAG).fallback(TOKEN)
				[ TOKEN    ].action(ReturnContent,Content)(EndOfText,EXIT)(EndOfLine,Cntrl,Space,CONTENT)(Lt,XMLTAG).fallback(CONTENT);
			}
			(*this)
			[ SEEKTOK  ](EndOfText,EXIT)(EndOfLine)(Cntrl)(Space)(Lt,XMLTAG).fallback(TOKEN)
			[ XMLTAG   ](EndOfLine)(Cntrl)(Space)(Questm,PITAG)(Exclam,ENTITYSL)(Slash,CLOSETAG).fallback(OPENTAG)
			[ OPENTAG  ].action(ReturnIdentifier,OpenTag)(EndOfLine,Cntrl,Space,TAGAISK)(Slash,TAGCLIM)(Gt,CONTENT).miss(ErrExpectedTagAttribute)
			[ CLOSETAG ].action(ReturnIdentifier,CloseTag)(EndOfLine,Cntrl,Space,TAGCLSK)(Gt,CONTENT).miss(ErrExpectedTagEnd)
			[ TAGCLSK  ](EndOfLine)(Cntrl)(Space)(Gt,CONTENT).miss(ErrExpectedTagEnd)
			[ TAGAISK  ](EndOfLine)(Cntrl)(Space)(Gt,CONTENT)(Slash,TAGCLIM).fallback(TAGANAM)
			[ TAGANAM  ].action(ReturnIdentifier,TagAttribName)(EndOfLine,Cntrl,Space,TAGAESK)(Equal,TAGAVSK).miss(ErrExpectedEqual)
			[ TAGAESK  ](EndOfLine)(Cntrl)(Space)(Equal,TAGAVSK).miss(ErrExpectedEqual)
			[ TAGAVSK  ](EndOfLine)(Cntrl)(Space)(Sq,TAGAVSQ)(Dq,TAGAVDQ).fallback(TAGAVID)
			[ TAGAVID  ].action(ReturnIdentifier,TagAttribValue)(EndOfLine,Cntrl,Space,TAGAISK)(Slash,TAGCLIM)(Gt,CONTENT).miss(ErrExpectedTagAttribute)
			[ TAGAVSQ  ].action(ReturnSQString,TagAttribValue)(Sq,TAGAVQE).miss(ErrStringNotTerminated)
			[ TAGAVDQ  ].action(ReturnDQString,TagAttribValue)(Dq,TAGAVQE).miss(ErrStringNotTerminated)
			[ TAGAVQE  ](EndOfLine,Cntrl,Space,TAGAISK)(Slash,TAGCLIM)(Gt,CONTENT).miss(ErrExpectedTagAttribute)
			[ TAGCLIM  ].action(Return,CloseTagIm)(EndOfLine)(Cntrl)(Space)(Gt,CONTENT).miss(ErrExpectedTagEnd)
			[ ENTITYSL ](Osb,CDATA)(Dash,COMDASH2).fallback(ENTITY)
			[ ENTITY   ](Gt,ENTITYE)(EndOfLine)(Cntrl)(Space)(Dq,ENTITYDQ)(Sq,ENTITYSQ)(Osb,ENTITYLC).fallback(ENTITYID)
			[ ENTITYE  ].action(Return,DocAttribEnd).fallback(SEEKTOK)
			[ ENTITYID ].action(ReturnIdentifier,DocAttribValue)(EndOfLine,Cntrl,Space,ENTITY)(Gt,ENTITYE).miss(ErrIllegalDocumentAttributeDef)
			[ ENTITYSQ ].action(ReturnSQString,DocAttribValue)(Sq,ENTITY).miss(ErrStringNotTerminated)
			[ ENTITYDQ ].action(ReturnDQString,DocAttribValue)(Dq,ENTITY).miss(ErrStringNotTerminated)
			[ ENTITYLC ](Csb,ENTITY).other( ENTITYLC)
			[ COMDASH2 ](Dash,COMSEEKE).miss(ErrExpectedDash2)
			[ COMSEEKE ](Dash,COMENDD2).other(COMSEEKE)
			[ COMENDD2 ](Dash,COMENDCL).other(COMSEEKE)
			[ COMENDCL ](Gt,SEEKTOK)(Dash,COMENDD2).other(COMSEEKE)
			[ CDATA    ].action(ExpectIdentifierCDATA)(Osb,CDATA1).miss(ErrExpectedCDATATag)
			[ CDATA1   ](Csb,CDATA2).other(CDATA1)
			[ CDATA2   ](Csb,CDATA3).other(CDATA1)
			[ CDATA3   ](Gt,CONTENT).other(CDATA1)
			[ EXIT     ].action(Return,Exit);
		}
	};

	///\typedef IsTokenCharMap
	///\brief Forms a set of characters by assigning (true/false) to the whole domain
	typedef CharMap<bool,false,NofControlCharacter> IsTokenCharMap;

	///\class IsTagCharMap
	///\brief Defines the set of tag characters
	struct IsTagCharMap :public IsTokenCharMap
	{
		IsTagCharMap()
		{
			(*this)(Undef,true)(Any,true)(Dash,true);
		}
	};

	///\class IsWordCharMap
	///\brief Defines the set of content word characters (tokenization switched on)
	struct IsWordCharMap :public IsTokenCharMap
	{
		IsWordCharMap()
		{
			(*this)(Undef,true)(Equal,true)(Gt,true)(Slash,true)(Dash,true)(Exclam,true)(Questm,true)(Sq,true)(Dq,true)(Osb,true)(Csb,true)(Any,true);
		}
	};

	///\class IsContentCharMap
	///\brief Defines the set of content token characters
	struct IsContentCharMap :public IsTokenCharMap
	{
		IsContentCharMap()
		{
			(*this)(Cntrl,true)(Space,true)(EndOfLine,true)(Undef,true)(Equal,true)(Gt,true)(Slash,true)(Dash,true)(Exclam,true)(Questm,true)(Sq,true)(Dq,true)(Osb,true)(Csb,true)(Any,true);
		}
	};

	///\class IsSQStringCharMap
	///\brief Defines the set characters belonging to a single quoted string
	struct IsSQStringCharMap :public IsContentCharMap
	{
		IsSQStringCharMap()
		{
			(*this)(Sq,false)(Space,true);
		}
	};

	///\class IsDQStringCharMap
	///\brief Defines the set characters belonging to a double quoted string
	struct IsDQStringCharMap :public IsContentCharMap
	{
		IsDQStringCharMap()
		{
			(*this)(Dq,false)(Space,true);
		}
	};
};


///\class XMLScanner
///\brief XML scanner template that adds the functionality to the statemachine base definition
///\tparam InputIterator input iterator with ++ and read only * returning 0 als last character of the input
///\tparam InputCharSet_ character set encoding of the input, read as stream of bytes
///\tparam OutputCharSet_ character set encoding of the output, printed as string of the item type of the character set,
///\tparam OutputBuffer_ buffer for output with STL back insertion sequence interface (e.g. std::string,std::vector<char>,textwolf::StaticBuffer)
template
<
		class InputIterator,
		class InputCharSet_,
		class OutputCharSet_,
		class OutputBuffer_
>
class XMLScanner :public XMLScannerBase
{
private:
	///\class TokState
	///\brief Token state variables
	struct TokState
	{
		///\enum Id
		///\brief Enumeration of token parser states.
		///\remark These states define where the scanner has to continue parsing when it was interrupted by an EoD exception and reentered again with more input to process.
		enum Id
		{
			Start,				//< start state (no parsing action performed at the moment)
			ParsingDone,			//< scanner war interrupted after parsing something when accessing the follow character
			ParsingKey,			//< scanner was interrupted when parsing a key
			ParsingEntity,			//< scanner was interrupted when parsing an XML character entity
			ParsingNumericEntity,		//< scanner was interrupted when parsing an XML numeric character entity
			ParsingNumericBaseEntity,	//< scanner was interrupted when parsing an XML basic character entity (apos,amp,etc..)
			ParsingNamedEntity,		//< scanner was interrupted when parsing an XML named character entity
			ParsingToken			//< scanner was interrupted when parsing a token (not in entity cotext)
		};
		Id id;					//< the scanner token parser state

		enum EolnState				//< end of line state to fulfill the W3C requirements for end of line mapping (see http://www.w3.org/TR/xml/: 2.11 End-of-Line Handling)
		{
			SRC,CR
		};
		EolnState eolnState;			//< the scanner end of line state

		unsigned int pos;			//< entity buffer position (buf)
		unsigned int base;			//< numeric entity base (10 for decimal/16 for hexadecimal)
		EChar value;				//< parsed entity value
		char buf[ 16];				//< parsed entity buffer
		UChar curchr_saved;			//< save current character parsed for the case we cannot print it (output buffer too small)

		///\brief Constructor
		TokState()				:id(Start),eolnState(SRC),pos(0),base(0),value(0),curchr_saved(0) {}

		///\brief Reset this state variables (after succesful exit with a new token parsed)
		///\param [in] id_ the new entity parse state
		///\param [in] eolnState_ the end of line mapping state
		void init(Id id_=Start, EolnState eolnState_=SRC)
		{
			id=id_;eolnState=eolnState_;pos=0;base=0;value=0;curchr_saved=0;
		}
	};
	TokState tokstate;								//< the entity parsing state of this XML scanner

public:
	typedef InputCharSet_ InputCharSet;
	typedef OutputCharSet_ OutputCharSet;
	class iterator;

public:
	typedef TextScanner<InputIterator,InputCharSet_> InputReader;
	typedef XMLScanner<InputIterator,InputCharSet_,OutputCharSet_,OutputBuffer_> ThisXMLScanner;
	typedef std::map<const char*,UChar> EntityMap;
	typedef OutputBuffer_ OutputBuffer;

	///\brief Print a character to the output token buffer
	///\param [in] ch unicode character to print
	void push( UChar ch)
	{
		m_output.print( ch, m_outputBuf);
	}

	///\brief Map a hexadecimal digit to its value
	///\param [in] ch hexadecimal digit to map to its decimal value
	static unsigned char HEX( unsigned char ch)
	{
		struct HexCharMap :public CharMap<unsigned char, 0xFF>
		{
			HexCharMap()
			{
				(*this)
					('0',0) ('1', 1)('2', 2)('3', 3)('4', 4)('5', 5)('6', 6)('7', 7)('8', 8)('9', 9)
					('A',10)('B',11)('C',12)('D',13)('E',14)('F',15)('a',10)('b',11)('c',12)('d',13)('e',14)('f',15);
			}
		};
		static HexCharMap hexCharMap;
		return hexCharMap[ch];
	}

	///\brief Parse a numeric entity value for a table definition (map it to the target character set)
	///\param [in] ir input reader
	///\return the value of the entity parsed
	static UChar parseStaticNumericEntityValue( InputReader& ir)
	{
		EChar value = 0;
		unsigned char ch = ir.ascii();
		unsigned int base;
		if (ch != '#') return 0;
		ir.skip();
		ch = ir.ascii();
		if (ch == 'x')
		{
			ir.skip();
			ch = ir.ascii();
			base = 16;
		}
		else
		{
			base = 10;
		}
		while (ch != ';')
		{
			unsigned char chval = HEX(ch);
			if (value >= base) return 0;
			value = value * base + chval;
			if (value >= 0xFFFFFFFF) return 0;
			ir.skip();
			ch = ir.ascii();
		}
		return (UChar)value;
	}

	///\brief Print the characters of a sequence that was thought to form an entity but did not
	///\return true on success
	void fallbackEntity()
	{
		switch (tokstate.id)
		{
			case TokState::Start:
			case TokState::ParsingDone:
			case TokState::ParsingKey:
			case TokState::ParsingToken:
				break;
			case TokState::ParsingEntity:
				push('&');
				break;
			case TokState::ParsingNumericEntity:
				push('&');
				push('#');
				break;
			case TokState::ParsingNumericBaseEntity:
				push('&');
				push('#');
				for (unsigned int ii=0; ii<tokstate.pos; ii++) push( tokstate.buf[ii]);
				break;
			case TokState::ParsingNamedEntity:
				push('&');
				for (unsigned int ii=0; ii<tokstate.pos; ii++) push( tokstate.buf[ii]);
				break;
		}
	}

	///\brief Try to parse an entity (we got '&')
	///\return true on success
	bool parseEntity()
	{
		unsigned char ch;
		tokstate.id = TokState::ParsingEntity;
		ch = m_src.ascii();
		if (ch == '#')
		{
			m_src.skip();
			return parseNumericEntity();
		}
		else
		{
			return parseNamedEntity();
		}
	}

	///\brief Try to parse a numeric entity (we got '&#')
	///\return true on success
	bool parseNumericEntity()
	{
		unsigned char ch;
		tokstate.id = TokState::ParsingNumericEntity;
		ch = m_src.ascii();
		if (ch == 'x')
		{
			tokstate.base = 16;
			m_src.skip();
			return parseNumericBaseEntity();
		}
		else
		{
			tokstate.base = 10;
			return parseNumericBaseEntity();
		}
	}

	///\brief Try to parse a numeric entity with known base (we got '&#' and we know the base 10/16 of it)
	///\return true on success
	bool parseNumericBaseEntity()
	{
		unsigned char ch;
		tokstate.id = TokState::ParsingNumericBaseEntity;

		while (tokstate.pos < sizeof(tokstate.buf))
		{
			ch = m_src.ascii();
			if (ch == ';')
			{
				if (tokstate.value > 0xFFFFFFFF)
				{
					tokstate.buf[ tokstate.pos++] = ch;
					fallbackEntity();
					return true;
				}
				push( (UChar)tokstate.value);
				tokstate.init( TokState::ParsingToken);
				m_src.skip();
				return true;
			}
			else
			{
				unsigned char chval = HEX(ch);
				if (chval >= tokstate.base)
				{
					fallbackEntity();
					return true;
				}
				tokstate.buf[ tokstate.pos++] = ch;
				tokstate.value = tokstate.value * tokstate.base + chval;
				m_src.skip();
			}
		}
		fallbackEntity();
		return true;
	}

	///\brief Try to parse a named entity
	///\return true on success
	bool parseNamedEntity()
	{
		unsigned char ch;
		tokstate.id = TokState::ParsingNamedEntity;
		ch = m_src.ascii();
		while (tokstate.pos < sizeof(tokstate.buf)-1 && ch != ';' && m_src.control() == Any)
		{
			tokstate.buf[ tokstate.pos] = ch;
			m_src.skip();
			tokstate.pos++;
			ch = m_src.ascii();
		}
		if (ch == ';')
		{
			tokstate.buf[ tokstate.pos] = '\0';
			if (!pushEntity( tokstate.buf)) return false;
			tokstate.init( TokState::ParsingToken);
			m_src.skip();
			return true;
		}
		else
		{
			fallbackEntity();
			return true;
		}
	}

	///\brief Try to recover from an interrupted token parsing state (end of input exception)
	///\return true on success
	bool parseTokenRecover()
	{
		bool rt = false;
		if (tokstate.curchr_saved)
		{
			push( tokstate.curchr_saved);
			tokstate.curchr_saved = 0;
		}
		switch (tokstate.id)
		{
			case TokState::Start:
			case TokState::ParsingDone:
			case TokState::ParsingKey:
			case TokState::ParsingToken:
				error = ErrInternal;
				return false;
			case TokState::ParsingEntity: rt = parseEntity(); break;
			case TokState::ParsingNumericEntity: rt = parseNumericEntity(); break;
			case TokState::ParsingNumericBaseEntity: rt = parseNumericBaseEntity(); break;
			case TokState::ParsingNamedEntity: rt = parseNamedEntity(); break;
		}
		tokstate.init( TokState::ParsingToken);
		return rt;
	}

	///\brief Parse a token defined by the set of valid token characters
	///\param [in] isTok set of valid token characters
	///\return true on success
	bool parseToken( const IsTokenCharMap& isTok)
	{
		if (tokstate.id == TokState::Start)
		{
			tokstate.id = TokState::ParsingToken;
			m_outputBuf.clear();
		}
		else if (tokstate.id != TokState::ParsingToken)
		{
			if (!parseTokenRecover())
			{
				tokstate.init();
				return false;
			}
		}
		for (;;)
		{
			ControlCharacter ch;
			while (isTok[ (unsigned char)(ch=m_src.control())])
			{
				UChar chr = m_src.chr();
				if (chr <= 0xD)
				{
					//handling W3C requirements for end of line translation in XML:
					unsigned char aa = m_src.ascii();
					if (aa == '\r')
					{
						push( (unsigned char)'\n');
						tokstate.eolnState = TokState::CR;
					}
					else if (aa == '\n')
					{
						if (tokstate.eolnState != TokState::CR)
						{
							push( (unsigned char)'\n');
						}
						tokstate.eolnState = TokState::SRC;
					}
					else
					{
						push( chr);
						tokstate.eolnState = TokState::SRC;
					}
				}
				else
				{
					push( chr);
					tokstate.eolnState = TokState::SRC;
				}
				m_src.skip();
			}
			if (ch == Amp)
			{
				m_src.skip();
				if (!parseEntity()) break;
				tokstate.init( TokState::ParsingToken);
				continue;
			}
			else
			{
				tokstate.init( TokState::ParsingDone);
				return true;
			}
		}
		tokstate.init();
		return false;
	}

	///\brief Static version of parse a token for parsing table definition elements
	///\tparam OutputBufferType type buffer for output
	///\param [in] isTok set of valid token characters
	///\param [in] ir input reader iterator
	///\param [out] buf buffer where to write the result to
	///\return true on success
	template <class OutputBufferType>
	static bool parseStaticToken( const IsTokenCharMap& isTok, InputReader ir, OutputBufferType& buf)
	{
		static OutputCharSet output;
		buf.clear();
		for (;;)
		{
			ControlCharacter ch;
			for (;;)
			{
				UChar pc;
				if (isTok[ (unsigned char)(ch=ir.control())])
				{
					pc = ir.chr();
				}
				else if (ch == Amp)
				{
					pc = parseStaticNumericEntityValue( ir);
				}
				else
				{
					return true;
				}
				output.print( pc, buf);
				ir.skip();
			}
		}
	}

	///\brief Skip a token defined by the set of valid token characters (same as parseToken but nothing written to the output buffer)
	///\param [in] isTok set of valid token characters
	///\return true on success
	bool skipToken( const IsTokenCharMap& isTok)
	{
		do
		{
			ControlCharacter ch;
			while (isTok[ (unsigned char)(ch=m_src.control())] || ch == Amp)
			{
				m_src.skip();
			}
		}
		while (m_src.control() == Any);
		return true;
	}

	///\brief Parse a token that must be the same as a given string
	///\param [in] str string expected
	///\return true on success
	bool expectStr( const char* str)
	{
		bool rt = true;
		tokstate.id = TokState::ParsingKey;
		for (; str[tokstate.pos] != '\0'; m_src.skip(),tokstate.pos++)
		{
			if (m_src.ascii() == str[ tokstate.pos]) continue;
			ControlCharacter ch = m_src.control();
			if (ch == EndOfText)
			{
				error = ErrUnexpectedEndOfText;
			}
			else
			{
				error = ErrSyntaxToken;
			}
			rt = false;
			break;
		}
		tokstate.init( TokState::ParsingDone);
		return rt;
	}

	///\brief Parse an entity defined by name (predefined)
	///\param [in] str pointer to the buffer with the entity name
	///\return true on success
	bool pushPredefinedEntity( const char* str)
	{
		switch (str[0])
		{
			case 'q':
				if (str[1] == 'u' && str[2] == 'o' && str[3] == 't' && str[4] == '\0')
				{
					push( '\"');
					return true;
				}
				break;

			case 'a':
				if (str[1] == 'm')
				{
					if (str[2] == 'p' && str[3] == '\0')
					{
						push( '&');
						return true;
					}
				}
				else if (str[1] == 'p')
				{
					if (str[2] == 'o' && str[3] == 's' && str[4] == '\0')
					{
						push( '\'');
						return true;
					}
				}
				break;

			case 'l':
				if (str[1] == 't' && str[2] == '\0')
				{
					push( '<');
					return true;
				}
				break;

			case 'g':
				if (str[1] == 't' && str[2] == '\0')
				{
					push( '>');
					return true;
				}
				break;

			case 'n':
				if (str[1] == 'b' && str[2] == 's' && str[3] == 'p' && str[4] == '\0')
				{
					push( ' ');
					return true;
				}
				break;
		}
		return false;
	}

	///\brief Parse an entity defined by name (predefined or in defined in entity table)
	///\param [in] str pointer to the buffer with the entity name
	///\return true on success
	bool pushEntity( const char* str)
	{
		if (pushPredefinedEntity( str))
		{
			return true;
		}
		else if (m_entityMap)
		{
			EntityMap::const_iterator itr = m_entityMap->find( str);
			if (itr == m_entityMap->end())
			{
				error = ErrUndefinedCharacterEntity;
				return false;
			}
			else
			{
				UChar ch = itr->second;
				push( ch);
				return true;
			}
		}
		else
		{
			error = ErrUndefinedCharacterEntity;
			return false;
		}
	}

private:
	STMState state;			//< current state of the XML scanner
	bool m_doTokenize;		//< true, if we do tokenize the input, false if we get the content according the W3C default (see http://www.w3.org/TR/xml: 2.10 White Space Handling)
	Error error;			//< last error code
	InputReader m_src;		//< source input iterator
	const EntityMap* m_entityMap;	//< map with entities defined by the caller
	OutputBuffer m_outputBuf;	//< buffer to use for output
	OutputCharSet m_output;

public:
	///\brief Constructor
	///\param [in] p_src source iterator
	///\param [in] p_outputBuf buffer to use for output
	///\param [in] p_entityMap read only map of named entities defined by the user
	XMLScanner( const InputIterator& p_src, const EntityMap& p_entityMap)
			:state(START),m_doTokenize(false),error(Ok),m_src(InputCharSet(),p_src),m_entityMap(&p_entityMap),m_output(OutputCharSet())
	{}
	XMLScanner( const InputIterator& p_src)
			:state(START),m_doTokenize(false),error(Ok),m_src(InputCharSet(),p_src),m_entityMap(0),m_output(OutputCharSet())
	{}
	XMLScanner( const InputCharSet& charset_, const InputIterator& p_src, const EntityMap& p_entityMap)
			:state(START),m_doTokenize(false),error(Ok),m_src(charset_,p_src),m_entityMap(&p_entityMap),m_output(OutputCharSet())
	{}
	XMLScanner( const InputCharSet& charset_, const InputIterator& p_src)
			:state(START),m_doTokenize(false),error(Ok),m_src(charset_,p_src),m_entityMap(0),m_output(OutputCharSet())
	{}
	XMLScanner( const InputCharSet& charset_)
			:state(START),m_doTokenize(false),error(Ok),m_src(charset_),m_entityMap(0)
	{}
	XMLScanner()
			:state(START),m_doTokenize(false),error(Ok),m_src(InputCharSet()),m_entityMap(0)
	{}

	///\brief Copy constructor
	///\param [in] o scanner to copy
	XMLScanner( const XMLScanner& o)
		:state(o.state)
		,m_doTokenize(o.m_doTokenize)
		,error(o.error)
		,m_src(o.m_src)
		,m_entityMap(o.m_entityMap)
		,m_outputBuf(o.m_outputBuf)
	{}

	///\brief Assign something to the source iterator while keeping the state
	///\param [in] a source iterator assignment
	template <class IteratorAssignment>
	void setSource( const IteratorAssignment& a)
	{
		m_src.setSource( a);
	}

	///\brief Get the current source iterator position
	///\return source iterator position in character words (usually bytes)
	std::size_t getPosition() const
	{
		return m_src.getPosition();
	}

	///\brief Get the current parsed XML element string, if it was not masked out, see nextItem(unsigned short)
	///\return the item string
	const char* getItem() const {return m_outputBuf.size()?&m_outputBuf.at(0):"\0\0\0\0";}

	///\brief Get the size of the current parsed YML element string in bytes
	///\return the item string
	std::size_t getItemSize() const {return m_outputBuf.size();}

	///\brief Get the current XML scanner state machine state
	///\return pointer to the state variables
	ScannerStatemachine::Element* getState()
	{
		static Statemachine STMtok(true);
		static Statemachine STMW3C(false);
		static Statemachine* stm[2] = {&STMW3C,&STMtok};
		return stm[ m_doTokenize]->get( state);
	}

	///\brief Set the tokenization behaviour
	///\param [out] v the tokenization behaviour flag
	void doTokenize( bool v)
	{
		m_doTokenize = v;
	}

	///\brief Get the last error
	///\param [out] str the error as string
	///\return the error code
	Error getError( const char** str=0)
	{
		Error rt = error;
		error = Ok;
		if (str) *str=getErrorString(rt);
		return rt;
	}

	///\brief Scan the next XML element
	///\param [in] mask element types that should be printed to the output buffer (1 -> print, 0 -> mask out, just return the element as event)
	///\return the type of the XML element
	ElementType nextItem( unsigned short mask=0xFFFF)
	{
		static const IsWordCharMap wordC;
		static const IsContentCharMap contentC;
		static const IsTagCharMap tagC;
		static const IsSQStringCharMap sqC;
		static const IsDQStringCharMap dqC;
		static const IsTokenCharMap* tokenDefs[ NofSTMActions] = {0,&wordC,&contentC,&tagC,&sqC,&dqC,0,0,0};
		static const char* stringDefs[ NofSTMActions] = {0,0,0,0,0,0,"xml","CDATA",0};

		ElementType rt = None;
		ControlCharacter ch;
		do
		{
			ScannerStatemachine::Element* sd = getState();
			if (sd->action.op != -1)
			{
				if (tokenDefs[sd->action.op])
				{
					if (tokstate.id != TokState::ParsingDone)
					{
						if ((mask&(1<<sd->action.arg)) != 0)
						{
							if (!parseToken( *tokenDefs[ sd->action.op])) return ErrorOccurred;
						}
						else
						{
							if (!skipToken( *tokenDefs[ sd->action.op])) return ErrorOccurred;
						}
					}
					rt = (ElementType)sd->action.arg;
				}
				else if (stringDefs[sd->action.op])
				{
					if (tokstate.id != TokState::ParsingDone)
					{
						if (!expectStr( stringDefs[sd->action.op])) return ErrorOccurred;
						if (sd->action.op == ExpectIdentifierXML)
						{
							//... special treatement for xml header for not
							//    enforcing the model too much just for this case
							push( '?'); push( 'x'); push( 'm'); push( 'l');
							rt = HeaderStart;
						}
					}
					else if (sd->action.op == ExpectIdentifierXML)
					{
						//... special treatement for xml header for not
						//    enforcing the model too much just for this case
						rt = HeaderStart;
					}
				}
				else
				{
					m_outputBuf.clear();
					rt = (ElementType)sd->action.arg;
				}
				if (sd->nofnext == 0)
				{
					if (sd->fallbackState != -1)
					{
						state = (STMState)sd->fallbackState;
					}
					return rt;
				}
			}
			ch = m_src.control();
			tokstate.id = TokState::Start;

			if (sd->next[ ch] != -1)
			{
				state = (STMState)sd->next[ ch];
				m_src.skip();
			}
			else if (sd->fallbackState != -1)
			{
				state = (STMState)sd->fallbackState;
			}
			else if (sd->missError != -1)
			{
				error = (Error)sd->missError;
				return ErrorOccurred;
			}
			else if (ch == EndOfText)
			{
				error = ErrUnexpectedEndOfText;
				return ErrorOccurred;
			}
			else
			{
				error = ErrInternal;
				return ErrorOccurred;
			}
		}
		while (rt == None);
		return rt;
	}

	///\class End
	///\brief end of input tag
	struct End {};

	///\class iterator
	///\brief input iterator for iterating on the output of an XML scanner
	class iterator
	{
	public:
		///\class Element
		///\brief Iterator element visited
		class Element
		{
		private:
			friend class iterator;
			ElementType m_type;		//< type of the element
			const char* m_content;		//< value string of the element
			std::size_t m_size;		//< size of the value string in bytes
		public:
			///\brief Type of the current element as string
			const char* name() const	{return getElementTypeName( m_type);}
			///\brief Type of the current element
			ElementType type() const	{return m_type;}
			///\brief Value of the current element
			const char* content() const	{return m_content;}
			///\brief Size of the value of the current element in bytes
			std::size_t size() const	{return m_size;}
			///\brief Constructor
			Element()			:m_type(None),m_content(0),m_size(0) {}
			///\brief Constructor
			Element( const End&)		:m_type(Exit),m_content(0),m_size(0) {}
			///\brief Copy constructor
			///\param [in] orig element to copy
			Element( const Element& orig)	:m_type(orig.m_type),m_content(orig.m_content),m_size(orig.m_size) {}
		};
		// input iterator traits
		typedef Element value_type;
		typedef std::size_t difference_type;
		typedef std::size_t size_type;
		typedef Element* pointer;
		typedef Element& reference;
		typedef std::input_iterator_tag iterator_category;

	private:
		Element element;				//< currently visited element
		ThisXMLScanner* input;				//< XML scanner

		///\brief Skip to the next element
		///\param [in] mask element types that should be printed to the output buffer (1 -> print, 0 -> mask out, just return the element as event)
		///\return iterator pointing to the next element
		iterator& skip( unsigned short mask=0xFFFF)
		{
			if (input != 0)
			{
				element.m_type = input->nextItem(mask);
				element.m_content = input->getItem();
				element.m_size = input->getItemSize();
			}
			return *this;
		}

		///\brief Compare iterator with another
		///\param [in] iter iterator to compare with
		///\return true if they are equal
		bool compare( const iterator& iter) const
		{
			if (element.type() == iter.element.type())
			{
				if (element.type() == Exit || element.type() == None) return true;  //equal only at beginning and end
			}
			return false;
		}
	public:
		///\brief Assign an iterator to another
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
		///\brief Constructor
		///\param [in] p_input XML scanner to use for iteration
		///\param [in] doSkipToFirst true, if the iterator should skip to the first character of the input (default behaviour of STL conform iterators but maybe not exception save)
		iterator( ThisXMLScanner& p_input, bool doSkipToFirst=true)
				:input( &p_input)
		{
			if (doSkipToFirst)
			{
				element.m_type = input->nextItem();
				element.m_content = input->getItem();
				element.m_size = input->getItemSize();
			}
		}
		///\brief Constructor
		iterator( const End& et)  :element(et),input(0) {}
		///\brief Constructor
		iterator()  :input(0) {}
		///\brief Assignement operator
		///\param [in] orig iterator to assign to this
		iterator& operator = (const iterator& orig)
		{
			assign( orig);
			return *this;
		}
		///\brief Element dereference operator
		const Element& operator*() const
		{
			return element;
		}
		///\brief Element dereference operator
		const Element* operator->() const
		{
			return &element;
		}
		///\brief Preincrement
		///\return *this
		iterator& operator++()				{return skip();}
		///\brief Postincrement
		///\return *this
		iterator operator++(int)			{iterator tmp(*this); skip(); return tmp;}

		///\brief Compare to check for equality
		///\return true, if equal
		bool operator==( const iterator& iter) const	{return compare( iter);}
		///\brief Compare to check for unequality
		///\return true, if not equal
		bool operator!=( const iterator& iter) const	{return !compare( iter);}
	};

	///\brief Get begin iterator
	///\return iterator
	///\param [in] doSkipToFirst true, if the iterator should skip to the first character of the input (default behaviour of STL conform iterators but maybe not exception save)
	iterator begin( bool doSkipToFirst=true)
	{
		return iterator( *this, doSkipToFirst);
	}
	///\brief Get the pointer to the end of content
	///\return iterator
	iterator end()
	{
		return iterator( End());
	}
};

}//namespace
#endif


