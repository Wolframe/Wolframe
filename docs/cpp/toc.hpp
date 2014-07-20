/// \file docs/cpp/toc.hpp
/// \brief File used by doxygen to generate the source documentation main page

/*! \mainpage Wolframe, a modular, 3-tier application server written in C++
 *
 * \section ModuleTypes Writing modules for Wolframe
 * \subsection ModuleTypeIntro Introduction

 * In Wolframe we can write modules for extending our application. Modules are units that export objects that can be used as components. The main source file of the module contains the following code block that declares the objects to be exported by the module. For using the macros shown in the following examples, we have to include the file \ref include/appDevel.hpp or the sub-include files needed.
 * \code
WF_MODULE_BEGIN( "MyModule", "this text is a short, one line description of the module")
 // ... here we declare the components to export
WF_MODULE_END
 * \endcode
 * \subsection ModuleObjTypeList Module component types in Wolframe
    - \b Command \b handler: 
          A command handler (_Wolframe::cmdbind::CommandHandler) implements a sub part of the client server protocol. They are declared in the section Processor of the main configuration. Currently there exist only two types of command handlers:
            - the standard command handler also called direct map. The standard command handler delegates the requests to functions to execute. It uses the filter modules to get an iterator on the input to pass to functions to execute. It uses forms declared to validate input and output.
            - the lua command handler
    - \b Document \b type \b detection:
          We need for each document format processed a document type detection (_Wolframe::cmdbind::DoctypeDetector) that extracts the document type information (_Wolframe::types::DoctypeInfo). This info structure is needed by command handlers to associate a document with a function to execute. Currently there are two document type detection modules implemented:
            - XML
            - JSON
    - \b Filter \b type:
          Filters (_Wolframe::langbind::Filter) are used to parse a document and to create a unified representation for processing it and contrarywise map the unified representation to a document. Hence a filter consist of two parts, an input filter (_Wolframe::langbind::InputFilter) and an output filter (_Wolframe::langbind::OutputFilter). The unified representation for filters is a structure with the document meta data as key/value pairs and a sequence of content elements of the following types:
            - OpenTag: Open a substructure. The value is the name of the structure opened
            - CloseTag: Close a substructure or marking the end of content (final close)
            - Value: Defines an atomic element
            - Attribute: Defines an attribute name, the following value is the attribute value.
    - \b Form \b Function:
          Form functions (_Wolframe::langbind::FormFunction) are functions with a structure as input and a structure as output. The input structure is represented by an iterator implementing the filter interface (iterator on OpenTag,CloseTag,Attribute,Value elements). In Wolframe any function in any language used for processing is implemented as form function. With this object type it is also possible to implement form functions in C++ (_Wolframe::serialize::CppFormFunction).
    - \b Program \b type:
          Program types define the loading of objects into the program library (_Wolframe::prgbind::ProgramLibrary). Each program type declares a file type to be of its own and loads every file of this type configured with 'program' in the 'Processor' section of the configuration.
    - \b DDL \b compiler:
          DDL (data definition language) compilers are compilers for forms used to validate input and output. Currently only 'simpleform' is implemented.
    - \b Custom \b data \b type:
          Custom data types (_Wolframe::types::CustomDataType) define arithmetic types with some methods. The idea is to define arithmetic data types for things like date/time or currency only once and not for every language binding. Custom data types can be used in normalization programs and so in data forms to validate and normalize atomic elements.
    - \b Normalization \b function:
          Normalization functions (_Wolframe::types::NormalizeFunction) are besides custom data types the basic bricks to define atomic data types in forms. This component type lets you define your own normalization functions.

 * \subsection ModuleTypeList Special module types in Wolframe
 *  The following module types do not have yet macros defined to declare them as the components introduced in the previous section. We have to refer to examples for the time being.
    - \b Authentication:
         An authentication module implements the object authentication unit and authenticator slice. An authentication unit declares one or more authentication mechs that can be chosen by the client for authentication when the module is loaded and configured in the section AAAA of the configuration. When more than one authentication unit implements an authentication mech then the first one in the configuration is chosen. The authentication slice is an instance created to do the authentication procedure. The result of authentication is a User (_Wolframe::AAAA::User) object in the execution context (_Wolframe::proc::ExecContext) that is the base for authorization to do anything (user privileges). The authorization module type does not yet have the constructors declared in the section before. We have to declare four classes involved:
              - configuration (implements _Wolframe::config::NamedConfiguration)
              - authentication unit (implements _Wolframe::AAAA::AuthenticationUnit)
              - authenticator slice (implements _Wolframe::AAAA::AuthenticatorSlice)
              - password changer (implements _Wolframe::AAAA::PasswordChanger)
    - \b Database:
         A database module is a quite complex component. The following interfaces have to be implemented:
              - configuration (implements _Wolframe::config::NamedConfiguration)
              - database language description (implements _Wolframe::db::LanguageDescription)
              - database unit (implements _Wolframe::db::DatabaseUnit)
              - database (implements _Wolframe::db::Database)
              - transaction (implements _Wolframe::db::Transaction)
              - transaction execution statemachine (implements _Wolframe::db::TransactionExecStatemachine)
    - \b Runtime \b environment:
         A runtime environment (_Wolframe::langbind::RuntimeEnvironment) is a configurable environment for functions that need a context for execution. The only case where a runtime environment is currently used in Wolframe is for .NET (Windows only).

 * \section ModuleExamples Example modules for Wolframe
 * \subsection FilterModule Writing a filter module for another format than XML or JSON
 *
 * \code

#include "appdevel/moduleFrameMacros.hpp"
#include "appdevel/filterModuleMacros.hpp"
#include "filter/filter.hpp"

static _Wolframe::langbind::FilterType* createMyFilterType()
{
	// ... here we create our own filter type object
}

WF_MODULE_BEGIN( "MyFilter", "my content filter")
 WF_FILTER_TYPE( "myfilter", MyFilterType::create)
WF_MODULE_END

 * \endcode
 *
 * \subsection FormFunctionModule Writing a module for a form function (struct in, struct out) as function, that is callable by Wolframe and any language binding in your application
 *
 * \code

#include "appdevel/moduleFrameMacros.hpp"
#include "appdevel/cppFormFunctionModuleMacros.hpp"

struct MyInput
{
	std::string data;
	int id;
	// ... your data structures are here

	static const _Wolframe::serialize::StructDescriptionBase *getStructDescription()
};

class MyInputDescription :public serialize::StructDescription<MyInput>
{
	MyInputDescription()
	{
		// here you define introspection for your data:
		(*this)
			("data", &MyInput::data)
			("id", &MyInput::id)
		;
	}
}

static const _Wolframe::serialize::StructDescriptionBase* MyInput::getStructDescription()
{
	static MyInputDescription rt;
	return &rt;
}

struct MyOutput
{
	// ... your data structures are here as in MyInput

	static const serialize::StructDescriptionBase *getStructDescription()
	{
		// ... your data structure description is here as for MyInput
	}
};


static int myFunction( proc::ExecContext* ctx, MyOutput& res, const MyInput& param)
{
	// ... your function implementation is here
}

WF_MODULE_BEGIN( "MyFunctions", "my functions short description")
 WF_FORM_FUNCTION( "myfunc", myFunction, MyOutput, MyInput)
WF_MODULE_END

* \endcode
*
* \subsection CommandHandlerModule Writing a command handler
*
* \code
#include "appdevel/commandHandlerModuleMacros.hpp"

class MyCommandHandlerConfig
	:public _Wolframe::config::NamedConfiguration
{
	// ... your command handler configuration is here
}

class MyCommandHandlerUnit
	:public _Wolframe::cmdbind::CommandHandlerUnit
{
	// ... your command handler unit definition is here
	MyCommandHandlerUnit( const MyCommandHandlerConfig* cfg)
	{
		// ... required constructor from configuration
	}
}

WF_MODULE_BEGIN( "MyCommandHandler", "my command handler short description")
 WF_COMMAND_HANDLER( "MyCommandHandler", "cmdhandler", "mycmd", _Wolframe::cmdbind::CommandHandlerUnit, config::NamedConfiguration)
WF_MODULE_END
* \endcode
*
*/

