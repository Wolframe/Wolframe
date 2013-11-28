/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
///\brief Internal interface of a description for transaction function constructor
///\file transactionFunctionDescription.hpp
#ifndef _DATABASE_TRANSACTION_FUNCTION_DESCRIPTION_HPP_INCLUDED
#define _DATABASE_TRANSACTION_FUNCTION_DESCRIPTION_HPP_INCLUDED
#include "database/transactionFunction.hpp"
#include "langbind/authorization.hpp"
#include "types/keymap.hpp"
#include <string>
#include <vector>
#include <map>

namespace _Wolframe {
namespace db {

///\class TransactionFunctionDescription
///\brief Description of a transaction function
class TransactionFunctionDescription
{
public:
	class PreProcessingStep
	{
	public:
		class Argument
		{
		public:
			enum Type {Constant,Selector};

			///\brief Default constructor
			Argument(){}
			///\brief Copy constructor
			Argument( const Argument& o)
				:type(o.type),name(o.name),value(o.value){}
			///\brief Constructor
			Argument( Type t, const std::string& n, const std::string& v)
				:type(t),name(n),value(v){}

			Type type;				//< type of the argument
			std::string name;			//< name of the argument (where it is mapped to)
			std::string value;			//< value of the argument
		};

		///\brief Default constructor
		PreProcessingStep(){}
		///\brief Copy constructor
		PreProcessingStep( const PreProcessingStep& o)
			:selector_FOREACH(o.selector_FOREACH)
			,path_INTO(o.path_INTO)
			,functionname(o.functionname)
			,args(o.args){}

		///\brief Clear content (default constructor)
		void clear()
		{
			selector_FOREACH.clear();
			path_INTO.clear();
			functionname.clear();
			args.clear();
		}

		///\class Error
		struct Error
		{
			///\brief Constructor
			Error( std::size_t elemidx_, const std::string& msg_)
				:elemidx(elemidx_)
				,msg(msg_){}

			std::size_t elemidx;
			std::string msg;
		};

		std::string selector_FOREACH;			//< selector of the nodes to process (FOREACH argument)
		std::vector<std::string> path_INTO;		//< parsed argument of INTO (splitted by '/')
		std::string functionname;			//< name of function to call
		std::vector<Argument> args;			//< function call arguments
	};

	///\class MainProcessingStep
	///\brief Description of one step (database operation) of a transaction function
	class MainProcessingStep
	{
	public:
		///\brief Default constructor
		MainProcessingStep()
			:resultref_FOREACH(-1)
			,nonempty(false)
			,unique(false){}

		///\brief Copy constructor
		MainProcessingStep( const MainProcessingStep& o)
			:selector_FOREACH(o.selector_FOREACH)
			,resultref_FOREACH(o.resultref_FOREACH)
			,call(o.call)
			,path_INTO(o.path_INTO)
			,nonempty(o.nonempty)
			,unique(o.unique)
			,hints(o.hints){}

		///\brief Clear content (default constructor)
		void clear()
		{
			selector_FOREACH.clear();
			resultref_FOREACH = -1;
			path_INTO.clear();
			call.clear();
			nonempty = false;
			unique = false;
			hints.clear();
		}

		std::string tostring() const;

		///\class Error
		struct Error
		{
			///\brief Constructor
			Error( std::size_t elemidx_, const std::string& msg_)
				:elemidx(elemidx_)
				,msg(msg_){}

			std::size_t elemidx;
			std::string msg;
		};

		///\class Call
		///\brief Database command call
		struct Call
		{
			///\class Param
			///\brief Database command call parameter
			struct Param
			{
				///\class Type
				///\brief Parameter type
				enum Type
				{
					NumericResultReference,		//< Parameter is a result reference by index
					SymbolicResultReference,	//< Parameter is a result reference by column name
					Constant,			//< Parameter is a contant value
					InputSelectorPath,		//< Parameter refers to a set of input values
					InternalVariable		//< Parameter refers to an internal variable (e.g. #COUNT)
				};
				///\brief Parameter type name
				static const char* typeName( Type i)
				{
					static const char* ar[] = {"NumericResultReference","SymbolicResultReference","Constant","InputSelectorPath"};
					return ar[(std::size_t)i];
				}
				const char* typeName() const
				{
					return typeName(type);
				}

				Type type;		//< type of the parameter
				int namspace;		//< result context namespace
				std::string value;	//< parsed value of the parameter

				///\brief Default constructor
				Param()
					:type(Constant),namspace(-1){}
				///\brief Copy constructor
				Param( const Param& o)
					:type(o.type),namspace(o.namspace),value(o.value){}
				///\brief Constructor
				Param( Type type_, const std::string& value_, int namspace_=-1)
					:type(type_),namspace(namspace_),value(value_){}
			};

			bool embedded;
			std::string statement;			//< statement or command name
			std::vector<std::string> templatearg;	//< list of template arguments in case of templated subroutine call
			std::vector<Param> paramlist;		//< list of arguments

			///\brief Default constructor
			Call(){}
			///\brief Copy constructor
			Call( const Call& o)
				:embedded(o.embedded),statement(o.statement),templatearg(o.templatearg),paramlist(o.paramlist){}
			///\brief Constructor
			Call( const std::string& statement_, const std::vector<std::string>& templatearg_, const std::vector<Param>& paramlist_, bool embedded_)
				:embedded(embedded_),statement(statement_),templatearg(templatearg_),paramlist(paramlist_)
			{}

			///\brief Reset call
			void clear()			{statement.clear(); paramlist.clear();}
			std::string tostring() const;
		};

		std::string selector_FOREACH;		//< parsed argument of FOREACH in case of a selector path
		int resultref_FOREACH;			//< parsed argument of FOREACH in case of a result reference
		Call call;				//< called database function
		std::vector<std::string> path_INTO;	//< parsed argument of INTO (splitted by '/')
		bool nonempty;				//< true, if NONEMPTY is set
		bool unique;				//< true, if UNIQUE is set
		types::keymap<std::string> hints;	//< error messages to add to database errors depending on the error class
	};

	///\class ResultBlock
	///\brief Result block declaration
	struct ResultBlock
	{
		std::vector<std::string> path_INTO;	//< parsed argument of INTO (splitted by '/')
		std::size_t startidx;			//< start of result block declaration
		std::size_t size;			//< number of command calls in result block declaration

		///\brief Constructor
		ResultBlock( const std::vector<std::string>& p, std::size_t i, std::size_t n)
			:path_INTO(p),startidx(i),size(n){}
		///\brief Copy constructor
		ResultBlock( const ResultBlock& o)
			:path_INTO(o.path_INTO),startidx(o.startidx),size(o.size){}
		///\brief Default constructor
		ResultBlock()
			:startidx(0),size(0){}
	};

	///\class PrintStep
	///\brief Print instruction declaration
	struct PrintStep
	{
		typedef MainProcessingStep::Call::Param Param;

		std::vector<std::string> path_INTO;	//< parsed argument of INTO (splitted by '/')
		Param argument;				//< printed value

		PrintStep(){}
		PrintStep( const std::vector<std::string>& p, const Param& a)
			:path_INTO(p),argument(a){}
		PrintStep( const PrintStep& o)
			:path_INTO(o.path_INTO),argument(o.argument){}
	};

	std::string name;
	std::map<std::size_t,PrintStep> printsteps;	//< Print instruction
	std::vector<PreProcessingStep> preprocs;	//< preprocessing steps on input
	std::string resultfilter;			//< result filter function for post processing
	std::vector<MainProcessingStep> steps;		//< list of database commands or operations
	std::vector<ResultBlock> resultblocks;		//< substructures of the output
	langbind::Authorization auth;			//< authorization definition structure for this function
	bool casesensitive;				//< true, is the database is case sensitive

	///\brief Copy constructor
	TransactionFunctionDescription( const TransactionFunctionDescription& o)
		:name(o.name),printsteps(o.printsteps),steps(o.steps),resultblocks(o.resultblocks),auth(o.auth),casesensitive(o.casesensitive){}
	///\brief Default constructor
	TransactionFunctionDescription()
		:casesensitive(false){}
};

class SubroutineDeclaration
{
public:
	SubroutineDeclaration(){}
	SubroutineDeclaration( const std::vector<std::string>& templateArguments_, const TransactionFunctionR& function_)
		:templateArguments(templateArguments_)
		,function(function_){}
	SubroutineDeclaration( const SubroutineDeclaration& o)
		:templateArguments(o.templateArguments)
		,function(o.function){}
public:
	std::vector<std::string> templateArguments;
	TransactionFunctionR function;
	
};

class SubroutineDeclarationMap :public types::keymap<SubroutineDeclaration>
{
public:
	SubroutineDeclarationMap(){}
	SubroutineDeclarationMap( const SubroutineDeclarationMap& o)
		:types::keymap<SubroutineDeclaration>(o){}
};

}}//namespace
#endif

