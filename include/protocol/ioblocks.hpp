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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
#ifndef _Wolframe_PROTOCOL_IO_BLOCKS_HPP_INCLUDED
#define _Wolframe_PROTOCOL_IO_BLOCKS_HPP_INCLUDED
/// \file protocol/ioblocks.hpp
/// \brief network message blocks as seen from the protocol with the input iterator classes and print functions

#include "iterators.hpp"
#include <stdexcept>

namespace _Wolframe {
namespace protocol {

///
/// \section protocol IO Blocks
/// \brief defines the processed blocks for output from and input to the processor
/// The application processor reads the input through iterators of the input blocks and prints via the output blocks.
///

///
/// \class MemBlock
/// \brief memory block for network messages
///
class MemBlock
{
public:
	//element typedefs
	typedef char value_type;		///< basic STL vector typedefs
	typedef std::size_t size_type;		///< basic STL vector typedefs

	MemBlock();
	MemBlock( size_type p_size);
	MemBlock( void* p_ptr, size_type p_size);
	MemBlock( const MemBlock& o);
	~MemBlock();

	/// \brief assignement operator
	MemBlock& operator=( const MemBlock& o);

	/// \brief set the current cursor byte position
	void setPos( size_type p_pos=0)				{m_pos = p_pos;}

	/// \brief set the buffer to be used by this class (as non allocated by this class)
	void set( void* p_ptr, size_type p_size);

	/// \brief constant void pointer to start of buffer
	void* ptr()						{return m_ptr;}
	/// \brief constant void pointer to start of buffer
	const void* ptr() const					{return m_ptr;}

	/// \brief constant char pointer to start of buffer
	char* charptr()						{return (char*)m_ptr;}
	/// \brief constant char pointer to start of buffer
	const char* charptr() const				{return (const char*)m_ptr;}

	/// \brief allocation size of the buffer in bytes
	size_type size() const					{return m_size;}

	/// \brief current byte position of the cursor (input or output)
	size_type pos() const					{return m_pos;}

	/// \brief access violation exceptions
	struct ArrayBoundReadError				:public std::logic_error {ArrayBoundReadError():std::logic_error("ABR"){}};

	/// \brief const random access operator
	char operator[]( size_type idx) const			{if (idx>=m_pos) throw ArrayBoundReadError(); return charptr()[idx];}
	/// \brief random access operator
	char& operator[]( size_type idx)			{if (idx>=m_pos) throw ArrayBoundReadError(); return charptr()[idx];}

private:
	void* m_ptr;		///< pointer to buffer
	std::size_t m_size;	///< allocation size in bytes of this buffer
	std::size_t m_pos;	///< current byte position of the cursor
	bool m_allocated;	///< true, if the memory of the buffer \c m_ptr is owned by this class and has to be freed again in the destructor
};



///
/// \class InputBlock
/// \brief input memory block for network input with iterators for the processing of the data
///
class InputBlock  :public MemBlock
{
public:
	///
	/// \class EoD
	/// \brief states of the automaton recognizing end of data and unescaping escape sequences in data
	///
	/// See method getEoD(iterator)
	///
	/// The end of data is marked with a sequence "\n.\r" or "\n.\n".
	/// If such a sequence appears by accident in the content it has to be escaped with a dot as "\n..\r" or "\n..\r".
	/// The additional dot after the end of line escapes any other character sequence too.
	/// As consequence every line starting with a dot has to be escaped.
	///
	struct EoD
	{
		enum State
		{
			SRC,			///< parsing content
			LF,			///< detected an LineFeed in state SRC, belonging either to a CRLF or a LF sequence
			LF_DOT,			///< detected a '.' (dot) in state LF
			LF_DOT_CR,		///< detected a CarriageReturn CR in state LF_DOT. Here we assume that end of data is reached
			LF_DOT_CR_LF		///< detected a LineFeed LF in state LF_DOT or LF_DOT_CR. Here we assume at last that end of data is reached
		};
	};

	InputBlock()						:m_eodState(EoD::SRC){}
	InputBlock( size_type p_size)				:MemBlock(p_size),m_eodState(EoD::SRC){}
	InputBlock( void* p_ptr, size_type p_size)		:MemBlock(p_ptr,p_size),m_eodState(EoD::SRC){}
	InputBlock( const InputBlock& o)			:MemBlock(o),m_eodState(o.m_eodState){}

	/// \brief random access const iterator
	typedef array::iterator_t<const InputBlock,size_type,char,char,const char*> const_iterator;
	/// \brief random access iterator
	typedef array::iterator_t<InputBlock,size_type,char,char&,char*> iterator;

	/// \brief get the starting const iterator
	const_iterator begin() const				{const_iterator rt(this); return rt;}
	/// \brief get the starting iterator
	iterator begin()					{iterator rt(this); return rt;}
	/// \brief random access to a const iterator
	const_iterator at( size_type pos_) const		{const_iterator rt(this); return rt+pos_;}
	/// \brief random access to an iterator
	iterator at( size_type pos_)				{iterator rt(this); return rt+pos_;}
	/// \brief get the end of block const iterator
	const_iterator end() const				{return const_iterator(this)+pos();}
	/// \brief get the end of block iterator
	iterator end()						{return iterator(this)+pos();}

	/// \brief end of data calculation and unescaping of LF_DOT sequences
	iterator getEoD( iterator start)
	{
		int eodpos = getEoDpos( start-begin());
		return (eodpos>=0)?(start+eodpos):end();
	}

	/// \brief reset end of data state machine
	void resetEoD()						{m_eodState=EoD::SRC;}

	/// \brief return true if end of data was recognized with the last call of getEoD(iterator)
	bool gotEoD() const					{return m_eodState>=EoD::LF_DOT_CR;}
	/// \brief return true if we have consumed also the terminating LineFeed after the end of data
	bool gotEoD_LF() const					{return m_eodState>=EoD::LF_DOT_CR_LF;}

	/// \brief consume the terminating LineFeed after the end of data
	iterator getStart( const iterator itr)
	{
		if (m_eodState == EoD::LF_DOT_CR && itr < end() && *itr == '\n')
		{
			m_eodState = EoD::LF_DOT_CR_LF;
			return itr+(size_type)1;
		}
		else
		{
			return itr;
		}
	}

private:
	/// \brief implementation of the end of data recognition and linefeed,dot escaping state machine
	int getEoDpos( size_type offset);
	/// \brief state of end of data recognition
	EoD::State m_eodState;
};




/// \class OutputBlock
/// \brief output interface based on a memory block.
///
/// Print as buffer is available and then order to "ship" what you printed.
///
class OutputBlock :public MemBlock
{
public:
	OutputBlock( size_type p_size)				:MemBlock(p_size) {}
	OutputBlock( void* p_ptr, size_type p_size)		:MemBlock(p_ptr,p_size) {}
	OutputBlock( const OutputBlock& o)			:MemBlock(o) {}

	//return true if the buffer is empty
	bool empty() const
	{
		return (pos()==0);
	}

	//print one character to the output
	bool print( char ch)
	{
		if (pos() == size()) return false;
		charptr()[ pos()] = ch;
		setPos( pos() + 1);
		return true;
	}

	//forward the output buffer cursor
	bool incPos( size_type nn)
	{
		if (pos()+nn >= size()) return false;
		setPos( pos() + nn);
		return true;
	}

	//pointer to the rest of the output buffer block
	const char* rest() const				{return charptr() + pos();}
	char* rest()						{return charptr() + pos();}

	//size of the rest of the output buffer (how many characters can be written)
	size_type restsize() const				{return size()-pos();}

	//release a written memory block
	void release()						{setPos(0);}
};

} // namespace protocol
} // namespace _Wolframe

#endif // _Wolframe_PROTOCOL_IO_BLOCKS_HPP_INCLUDED

