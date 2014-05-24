/************************************************************************
Copyright (C) 2011 - 2014 Project Wolframe.
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
///\file directmapCommandHandler.hpp
///\brief Interface to the directmap command handler
#ifndef _Wolframe_cmdbind_DIRECTMAP_COMMAND_HANDLER_HPP_INCLUDED
#define _Wolframe_cmdbind_DIRECTMAP_COMMAND_HANDLER_HPP_INCLUDED
#include "directmapProgram.hpp"
#include "langbind/formFunction.hpp"
#include "filter/redirectFilterClosure.hpp"
#include "cmdbind/ioFilterCommandHandlerEscDLF.hpp"
#include "processor/execContext.hpp"
#include <boost/shared_ptr.hpp>
#include "types/keymap.hpp"
#include "types/form.hpp"
#include "serialize/ddl/ddlStructParser.hpp"
#include "serialize/ddl/ddlStructSerializer.hpp"

namespace _Wolframe {
namespace cmdbind {

///\class DirectmapCommandHandler
///\brief Command handler instance for processing a call as Directmap (mapping with forms and a transaction or form function)
class DirectmapCommandHandler :public IOFilterCommandHandlerEscDLF
{
public:
	typedef IOFilterCommandHandlerEscDLF Parent;
	///\brief Constructor
	explicit DirectmapCommandHandler( const langbind::DirectmapCommandDescription* cmddescr, const std::string& default_filter_)
		:m_cmd(cmddescr)
		,m_state(0)
		,m_function(0)
		,m_checkIfInputLeft(false)
		,m_default_filter(default_filter_)
		{}

	///\brief Destructor
	virtual ~DirectmapCommandHandler(){}

	///\brief Execute the Directmap
	///\param[out] err error code in case of error
	///\return CallResult status (See IOFilterCommandHandler::CallResult)
	virtual CallResult call( const char*& err);

private:
	void initcall();

private:
	const langbind::DirectmapCommandDescription* m_cmd;			//< description of command to execute
	int m_state;								//< internal state
	const langbind::FormFunction* m_function;				//< function to execute
	types::FormR m_inputform;						//< (optional) form for validating and formating input
	types::FormR m_outputform;						//< (optional) form for validating and formating output
	std::string m_output_rootelement;					//< output root
	langbind::TypedInputFilterR m_input;					//< structure for input
	langbind::TypedOutputFilterR m_output;					//< structure for output
	langbind::FormFunctionClosureR m_functionclosure;			//< processor for the transaction
	langbind::TypedInputFilterR m_outputform_serialize;			//< serializer of the data in the output form
	boost::shared_ptr<serialize::DDLStructParser> m_inputform_parser;	//< parser to map the input to the input form
	langbind::RedirectFilterClosure m_outputprinter;			//< processor for redirection of the transaction call result or if defined the output form content to output
	std::string m_errormsg;							//< buffer for error message returned
	bool m_checkIfInputLeft;						//< true, if we need to check, if there is input left (SKIP)
	const std::string m_default_filter;					//< default filter name defined by the document format
};

}}//namespace
#endif

