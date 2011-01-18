//
// protocol helper
//
#ifndef _SMERP_PROTOCOL_HPP_INCLUDED
#define _SMERP_PROTOCOL_HPP_INCLUDED

//TODO name const iterators const_iterator as STL does instead of iterator

//parser for the protocol commands and their arguments
#include "protocol/parser.hpp"
//buffers used by parser to collect arguments of protocol commands
#include "protocol/buffers.hpp"
//iterator for CRLFdotCRLF delimited text content
#include "protocol/textIterator.hpp"
//basic input iterators and message blocks
#include "protocol/ioblocks.hpp"
//building blocks for generators in processing languages knowing a 'yield' operation
#include "protocol/generator.hpp" 

#endif // _SMERP_PROTOCOL_HPP_INCLUDED

