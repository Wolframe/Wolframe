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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file cmdbind/directmapCommandHandler.hpp
///\brief interface to the directmap command handler
#ifndef _Wolframe_cmdbind_DIRECTMAP_COMMAND_HANDLER_HPP_INCLUDED
#define _Wolframe_cmdbind_DIRECTMAP_COMMAND_HANDLER_HPP_INCLUDED
#include "langbind/appObjects.hpp"
#include "langbind/directmapConfig_struct.hpp"
#include "cmdbind/ioFilterCommandHandlerEscDLF.hpp"
#include "types/countedReference.hpp"

namespace _Wolframe {
namespace cmdbind {

///\class DirectmapContext
class DirectmapContext
{
public:
	DirectmapContext(){}
	~DirectmapContext(){}

	void load( const langbind::DirectmapConfigStruct& cfg_, const module::ModulesDirectory*);

	///\brief Get the list of commands
	std::list<std::string> commands() const;

	const langbind::DirectmapCommandConfigStruct* command( const std::string& name) const
	{
		std::map<std::string, std::size_t>::const_iterator itr = m_map.find( name);
		if (itr == m_map.end()) return 0;
		return &m_cfg.command[ itr->second];
	}

private:
	std::map<std::string, std::size_t> m_map;
	langbind::DirectmapConfigStruct m_cfg;
};

///\class DirectmapCommandHandler
///\brief Command handler instance for processing a call as Directmap (mapping with forms and a transaction or form function)
class DirectmapCommandHandler :public IOFilterCommandHandlerEscDLF
{
public:
	///\brief Declaration for cmdbind::ScriptCommandHandlerConstructor
	typedef DirectmapContext ContextStruct;
	///\brief Type definition for instantiation of cmdbind::ScriptCommandHandlerBuilder
	typedef langbind::DirectmapConfigStruct ConfigStruct;

	///\brief Constructor
	explicit DirectmapCommandHandler( const DirectmapContext* ctx_)
		:m_ctx(ctx_)
		,m_cmd(0)
		,m_state(0)
		,m_function(0)
		,m_inputform_defined(false)
		,m_outputform_defined(false){}

	///\brief Destructor
	virtual ~DirectmapCommandHandler(){}

	///\brief Execute the Directmap
	///\param[out] err error code in case of error
	///\return CallResult status (See IOFilterCommandHandler::CallResult)
	virtual CallResult call( const char*& err);

	///\brief Get the identifier of this command handler type
	static const char* identifier()
	{
		return "DirectmapCommandHandler";
	}

private:
	void initcall();
private:
	const DirectmapContext* m_ctx;					//< execution context of the command handler
	const langbind::DirectmapCommandConfigStruct* m_cmd;		//< description of command to execute
	int m_state;							//< internal state
	const db::TransactionFunction* m_function;			//< function object with input/output transaction serialization description
	ddl::Form m_inputform;						//< (optional) form for validating and formating input
	ddl::Form m_outputform;						//< (optional) form for validating and formating output
	bool m_inputform_defined;					//< true if m_inputform defined
	bool m_outputform_defined;					//< true if m_outputform defined
	langbind::TypedInputFilterR m_input;				//< structure for input
	langbind::TypedOutputFilterR m_output;				//< structure for output
	types::CountedReference<langbind::TransactionFunctionClosure>
		 m_functionclosure;					//< processor for the transaction
	langbind::TypedInputFilterR m_outputform_serialize;		//< serializer of the data in the output form
	types::CountedReference<serialize::DDLStructParser>
		m_inputform_parser;					//< parser to map the input to the input form
	langbind::RedirectFilterClosure m_outputprinter;		//< processor for redirection of the transaction call result or if defined the output form content to output
	std::string m_errormsg;						//< buffer for error message returned
};

}}//namespace
#endif

