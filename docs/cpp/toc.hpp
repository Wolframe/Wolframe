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
 * \subsection ModuleObjTypeList Module component types
    - \b Command \b handler: 
         A command handler (_Wolframe::cmdbind::CommandHandler) implements a sub part of the client server protocol. They are declared in the section Processor of the main configuration. Currently there exist only two types of command handlers loadable from modules:
            - the standard command handler also called direct map. The standard command handler delegates the requests to functions to execute. It uses the filter modules to get an iterator on the input to pass to functions to execute. It uses forms declared to validate input and output.
            - the lua command handler
            .
         See \ref CommandHandlerModule
    - \b Document \b type \b detection:
         We need for each document format processed a document type detection (_Wolframe::cmdbind::DoctypeDetector) that extracts the document type information (_Wolframe::types::DoctypeInfo). This info structure is needed by command handlers to associate a document with a function to execute. Currently there are two document type detection modules implemented:
            - XML
            - JSON
            .
         See \ref DoctypeDetectionModule.
    - \b Filter \b type:
         Filters (_Wolframe::langbind::Filter) are used to parse a document 
         and to create a unified representation for processing it and 
         contrarywise map the unified representation to a document. 
         Hence a filter consist of two parts, an input filter 
         (_Wolframe::langbind::InputFilter) and an output filter
         (_Wolframe::langbind::OutputFilter). The unified representation 
         for filters is a structure with the document meta data as key/value
         pairs and a sequence of content elements of the following types:
            - OpenTag: Open a substructure. The value is the name of the structure opened or if empty, defining a new array element
            - CloseTag: Close a substructure or an array element or it is marking the end of content (final close)
            - Value: Defines an atomic element
            - Attribute: Defines an attribute name, the following element is the attribute value (as type value).
            .
         Filters define flags (_Wolframe::langbind::FilterBase::Flags)
         to level out language differences between producer and consumer of
         the filter element sequence.
         The initialization of the flags define a contract between producer and 
         consumer. The idea behind is that not the weakest peer involved defines
         globally what information is transmitted with an filter. The 
         consumer and the producer set some flags of the filter that describe the
         requirements of the consumer and the capabilities of the producer.
         There are two flags set by the producer:
            - PropagateNoCase tells the consumer that the tag names used are case insensitive
            - PropagateNoAttr tells the consumer that producer does not know about attributes (only OpenTag,CloseTag,Value are used)
            .
         There is one flag set by the consumer:
            - SerializeWithIndices tells the producer that the consumer has no 
              structure description that helps to distinguish between an array
              with one element or a single element. Therefore the consumer has
              to produce a sequence that contains the information if an element
              belongs to an array. This is done by having one named tag for
              the array structure and one tag without name for every array element.
              Lets take an array with name "Colors" two atomic elements "Red" and 
              "Blue" as example. Instead of producing a sequence like this, 
              if SerializeWithIndices is not set
              \code
               OpenTag "Colors",Value "Red",CloseTag,OpenTag "Colors",Value "Blue",CloseTag
              \endcode
              the following sequence has to be produced
              \code
               OpenTag "Colors",OpenTag,Value "Red",CloseTag,OpenTag,Value "Blue",CloseTag,CloseTag
              \endcode
            .
         Unfortunately we cannot define a format with array indices as
         unified format, because there are languages that do not have the
         capability to produce this information (like for example XML).
         So we take the weakest form as base and provide an upgrade if required 
         and if the model behind allows it. 
         The method _Wolframe::langbind::InputFilter::setFlags( Flags f)
         can return false if it cannot provide the required information.
         See \ref FilterModule
    - \b Form \b Function:
         Form functions (_Wolframe::langbind::FormFunction) are functions with a structure 
         as input and a structure as output. The input structure is represented by an 
         iterator implementing the input filter interface
         (iterator on OpenTag,CloseTag,Attribute,Value elements) with typed 
         values instead of strings (_Wolframe::langbind::TypedInputFilter). 
         In Wolframe any function in any language used for processing is 
         implemented as form function. With this object type it is also 
         possible to implement form functions in C++
         (_Wolframe::serialize::CppFormFunction).
         See \ref FormFunctionModule
    - \b Program \b type:
         Program types define the loading of objects into the program library (_Wolframe::prgbind::ProgramLibrary). Each program type declares a file type to be of its own and loads every file of this type configured with 'program' in the 'Processor' section of the configuration.
         See \ref ProgramTypeModule
    - \b DDL \b compiler:
         DDL (data definition language) compilers are compilers for forms used to validate input and output. Currently only 'simpleform' is implemented.
         See \ref DDLCompilerModule
    - \b Custom \b data \b type:
         Custom data types (_Wolframe::types::CustomDataType) define arithmetic types with some methods. The idea is to define arithmetic data types for things like date/time or currency only once and not for every language binding. Custom data types can be used in normalization programs and so in data forms to validate and normalize atomic elements.
         See \ref CustomDataTypeModule
    - \b Normalization \b function:
         Normalization functions (_Wolframe::types::NormalizeFunction) are besides custom data types the basic bricks to define atomic data types in forms. This component type lets you define your own normalization functions.
         See \ref NormalizerModule
    - \b Runtime \b environment:
         A runtime environment (_Wolframe::langbind::RuntimeEnvironment) is a configurable environment for functions that need a context for execution. The only case where a runtime environment is currently used in Wolframe is for .NET (Windows only).
         See \ref RuntimeEnvironmentModule
    - \b Authenticator \b unit:
         An authenticator unit (_Wolframe::AAAA::AuthenticatorUnit) implements one
         or more authentication mechanisms. An authentication unit is chosen
         for authentication of the client if it is the first configured authentication
         unit in the configuration section AAAA that implements the mechanism chosen.
         The class processing the authentication is called authentication slice (_Wolframe::AAAA::AuthenticatorSlice).
         See \ref AuthenticatorModule
    - \b Database \b interface:
         Wolframe has interfaces to execute queries on Sqlite3 and PostgreSQL databases. 
         To define a new database interface, we have to implement the following interfaces:
              - configuration (implements _Wolframe::config::NamedConfiguration)
              - database language description (implements _Wolframe::db::LanguageDescription)
              - database unit (implements _Wolframe::db::DatabaseUnit)
              - database (implements _Wolframe::db::Database)
              - transaction execution statemachine (implements _Wolframe::db::TransactionExecStatemachine)
              .
         The database unit and the configuration are the objects you have to declare when implementing
         a database module. See \ref DatabaseModule.

 * \page CommandHandlerModule Command handler module
 *
 * \code
#include "appdevel/commandHandlerModuleMacros.hpp"
#include "appdevel/moduleFrameMacros.hpp"
#include "cmdbind/commandHandlerUnit.hpp"

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
	virtual bool loadPrograms( const _Wolframe::proc::ProcessorProviderInterface* provider)
	{
		// ... load your command handler programs here
	}

	virtual std::vector<std::string> commands() const
	{
		// ... return the commands of the command handler here
	}

	virtual _Wolframe::cmdbind::CommandHandler* createCommandHandler( const std::string& cmdname, const std::string& docformat)
	{
		// ... create and return an instance of a command handler for executing the command cmdname here
	}
}

WF_MODULE_BEGIN( "MyCommandHandler", "my command handler short description")
 WF_COMMAND_HANDLER( "MyCommandHandler", "cmdhandler", "mycmd", MyCommandHandlerUnit, MyCommandHandlerConfig)
WF_MODULE_END
 * \endcode
 *
 * \page DoctypeDetectionModule Document type detection module
 * \code
#include "appdevel/doctypeModuleMacros.hpp"
#include "appdevel/moduleFrameMacros.hpp"

class DoctypeDetectorMyFormat
	:public _Wolframe::cmdbind::DoctypeDetector
{
public:
	DoctypeDetectorMyFormat(){}

	virtual void putInput( const char* chunk, std::size_t chunksize)
	{
		// ... feed your detector with a new input chunk here
	}

	virtual bool run()
	{
		// ... try to detect your document format in the input
		// ... return false and set the lastError in case of error
		// ... return false without setting lastError if you need more input chunks
		// ... return true, if you can decide wheter the document format is yours or not
	}

	virtual const char* lastError() const
	{
		// ... return the last error occurred here
	}

	virtual const types::DoctypeInfoR& info() const
	{
		// ... return the document type infor structure here, if the document format is yours
	}
};

WF_MODULE_BEGIN( "myDocformat", "document type/format detection for MyFormat")
 WF_DOCUMENT_FORMAT( "MYFM", DoctypeDetectorMyFormat)
WF_MODULE_END

 * \endcode
 * \page FilterModule Filter module
 *
 * \code

#include "appdevel/moduleFrameMacros.hpp"
#include "appdevel/filterModuleMacros.hpp"
#include "filter/filter.hpp"

class MyInputFilter
	:public _Wolframe::langbind::InputFilter
{
	virtual InputFilter* copy() const
	{
		// ... create a copy of this input filter here
	}

	virtual InputFilter* initcopy() const
	{
		// ... create a copy of this in first initialization state here
	}

	virtual void putInput( const void* ptr, std::size_t size, bool end)
	{
		// ... feed the filter with the next input chunk here
	}

	virtual void getRest( const void*& ptr, std::size_t& size, bool& end)
	{
		// ... get the unprocessed input here
	}

	virtual bool getNext( typename _Wolframe::langbind::InputFilter::ElementType& type, const void*& element, std::size_t& elementsize)
	{
		// ... get the next element produced by this input filter here
		// ... return false on error or end of buffer (state)
		// ... return true, if we returned a valid element in type,element,elementsize
	}

	virtual bool setFlags( _Wolframe::langbind::FilterBase::Flags f)
	{
		// ... check if you can fulfill the requirements imposed by the flags and set them if yes
		// ... return false, if the requirements imposed by the flags cannot be met
	}

	virtual bool checkSetFlags( _Wolframe::langbind::FilterBase::Flags f)const
	{
		// ... check, if the requirements imposed by the flags can be met
	}

	virtual const _Wolframe::types::DocMetaData* getMetaData()
	{
		// ... get the document meta data if possible
		// ... return false on error, or if another input chunk is needed
	}
};

class MyOutputFilter
	:public _Wolframe::langbind::OutputFilter
{
	virtual _Wolframe::langbind::OutputFilter* copy() const
	{
		// ... create a copy of this output filter here
	}

	virtual bool print( typename _Wolframe::langbind::OutputFilter::ElementType type, const void* element, std::size_t elementsize)
	{
		// ... do the output of one element here
		// ... use the method write( const void*, std::size_t) for writing to sink
	}

	virtual bool close()
	{
		// ... do check if the final close has been called (it does not have to) and do the final actions of the output filter
		// ... return false, if this method has to be called again because not all output could be flushed (failed method write( const void*, std::size_t))
	}
};

class MyFilter
	:public _Wolframe::langbind::Filter
{
	MyFilter()
		:_Wolframe::langbind::Filter( new _Wolframe::langbind::InputFilter(), new _Wolframe::langbind::OutputFilter()){}
};

class MyFilterType
	:public _Wolframe::langbind::FilterType
{
public:
	MyFilterType()
		:_Wolframe::langbind::FilterType("myfilter"){}

	virtual _Wolframe::langbind::Filter* create( const std::vector<_Wolframe::langbind::FilterArgument>&) const
	{
		return new MyFilter();
	}
};

WF_MODULE_BEGIN( "MyFilter", "my content filter")
 WF_FILTER_TYPE( "myfilter", MyFilterType)
WF_MODULE_END

 * \endcode
 *
 * \page FormFunctionModule Form function module
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
* \page ProgramTypeModule Program type module
*
* \code
#include "appdevel/programTypeModuleMacros.hpp"
#include "appdevel/moduleFrameMacros.hpp"
#include "prgbind/program.hpp"
#include "prgbind/programLibrary.hpp"
#include <string>

class MyProgram
	:public _Wolframe::prgbind::Program
{
public:
	MyProgram()
		:_Wolframe::prgbind::Program( SuperFunction){}

	virtual bool is_mine( const std::string& filename) const
	{
		// ... return true, if the file filename is of my type here
	}

	virtual void loadProgram( _Wolframe::prgbind::ProgramLibrary& library, _Wolframe::db::Database* transactionDB, const std::string& filename)
	{
		// ... load the program in the file filename and store its declared items in the program library or in the database
	}
};

WF_MODULE_BEGIN( "MyProgramTypeModule", "my program type module")
 WF_PROGRAM_TYPE( "MyProgram", MyProgram)
WF_MODULE_END

* \endcode
*
* \page DDLCompilerModule Data definition language (DDL) compiler module
*
* \code
#include "appdevel/ddlCompilerModuleMacros.hpp"
#include "appdevel/moduleFrameMacros.hpp"
#include "langbind/ddlCompilerInterface.hpp"

class MyDDLCompiler
	:public _Wolframe::langbind::DDLCompiler
{
public:
	MyDDLCompiler()
		:_Wolframe::langbind::DDLCompiler( "myddl", "mdl") {}

	virtual std::vector<_Wolframe::types::FormDescriptionR> compile( const std::string& filename, const _Wolframe::types::NormalizeFunctionMap* typemap) const
	{
		// ... compile the source file filename and return the resulting form definitions here
	}
};

WF_MODULE_BEGIN( "MyDDLCompiler", "compiler for my language describing data forms")
 WF_DDLCOMPILER( "MyDDL", MyDDLCompiler)
WF_MODULE_END
* \endcode
*
* \page CustomDataTypeModule Custom data type module
*
* \code
#include "appdevel/moduleFrameMacros.hpp"
#include "appdevel/customDataTypeModuleMacros.hpp"
#include "types/customDataType.hpp"
#include "types/variant.hpp"

class MyInitializer
	:public _Wolframe::types::CustomDataInitializer
{
public:
	MyInitializer( const std::vector<types::Variant>& arg)
	{
		// ... construct your custom data type initializer object here
	}

	static CustomDataInitializer* create( const std::vector<types::Variant>& arg)
	{
		return new MyInitializer( arg);
	}
};

class MyValue
	:public _Wolframe::types::CustomDataValue
{
public:
	explicit MyValue( const std::string& dt, const MyInitializer* ini=0)
	{
		// ... put your value constructor from string here
	}

	virtual int compare( const _Wolframe::types::CustomDataValue& o) const
	{
		// ... put your compare function returning -1,0 or +1 here
	}

	virtual std::string tostring() const
	{
		// ... put your conversion of the value to a string here
	}

	virtual void assign( const _Wolframe::types::Variant& o)
	{
		// ... put the assignment from a variant data type here
	}

	virtual bool getBaseTypeValue( _Wolframe::types::Variant& dest) const
	{
		// ... try to convert this value to a variant data type returned in dest
		// ... return false, if not possible
	}

	virtual _Wolframe::types::CustomDataValue* copy() const
	{
		// ... return an exact copy of this here
	}

	static _Wolframe::types::CustomDataValue* create( const _Wolframe::types::CustomDataInitializer*)
	{
		// ... create a default value from initializer (can be NULL) here
	}
};

class MyType
	:public _Wolframe::types::CustomDataType
{
public:
	MyType( const std::string& name_)
		:_Wolframe::types::CustomDataType(name_,&MyValue::create,&MyInitializer::create)
	{
		define( Increment, &increment);
		define( Add, &add);
		define( "mymethod", &myMethod);
	}

	static _Wolframe::types::Variant myMethod( const _Wolframe::types::CustomDataValue& operand, const std::vector<_Wolframe::types::Variant>& arg)
	{
		// ... the implementation of the method "mymethod" operating on operand with arg as list of arguments comes here
	}
	static _Wolframe::types::Variant increment( const _Wolframe::types::CustomDataValue& operand)
	{
		// ... the increment operator implementation follows here
	}
	static _Wolframe::types::Variant add( const CustomDataValue& operand, const Variant& arg);
	{
		// ... the implementation of the addition operator operating on operand with arg as argument comes here
	}

	static _Wolframe::types::CustomDataType* create( const std::string& name)
	{
		return new MyType( name);
	}
};

WF_MODULE_BEGIN( "mydatatype", "my custom data type module")
 WF_CUSTOM_DATATYPE( "mydatatype", MyType::create)
WF_MODULE_END
* \endcode
*
* \page NormalizerModule Normalize function module
*
* \code
#include "appdevel/normalizeModuleMacros.hpp"
#include "appdevel/moduleFrameMacros.hpp"
#include "types/normalizeFunction.hpp"

class MyNormalizeFunction
	:_Wolframe::types::NormalizeFunction
{
public:
	MyNormalizeFunction( const std::vector<_Wolframe::types::Variant>& arg)
	{
		// ... the WF_NORMALIZER* template needs a constructor with this signature
	}

	virtual const char* name() const
	{
		// ... return the identifier of the function here
	}
	virtual _Wolframe::types::Variant execute( const _Wolframe::types::Variant& i) const
	{
		// ... execure this function on the input variant here
	}
	virtual _Wolframe::types::NormalizeFunction* copy() const
	{
		// ... return a copy of this here
	}
};

WF_MODULE_BEGIN( "My normalizer", "my normalizer module")
 WF_NORMALIZER( "mynormalize",  MyNormalizeFunction)
WF_MODULE_END
* \endcode
*
* If your normalizer functions use common resources, you can declare them as follows:
*
* \code
class MyResources
	:public _Wolframe::types::NormalizeResourceHandle
{
	// ... put your common data structure (normalizer resources) decrarations here
};
* \endcode
* The MyNormalizeFunction has a slighly different constructor signature required.
The first parameter is the base resource handle reference. The consructor can use a 
dynamic_cast to get his resource object:
:
* \code
MyNormalizeFunction::MyNormalizeFunction( _Wolframe::types::NormalizeResourceHandle* reshnd, const std::vector<_Wolframe::types::Variant>& arg)
{
	MyResources* myreshnd = dynamic_cast<MyResources*>(reshnd);
	// ... do other initializations
}
* \endcode
* We then declare the resources and the function with the following macro after WF_MODULE_BEGIN:
* \code
WF_NORMALIZER_RESOURCE( MyResources)
WF_NORMALIZER_WITH_RESOURCE( "mynormalize",  MyNormalizeFunction, MyResources)
* \endcode
* \page RuntimeEnvironmentModule Runtime environment host structure module
*
* \code
#include "appdevel/runtimeEnvironmentModuleMacros.hpp"
#include "appdevel/moduleFrameMacros.hpp"

static initMyRuntimeEnvironment()
{
    // ... put your global initializations here
}

class MyRuntimeEnvironmentConfig
	:public config::NamedConfiguration
{
	// ... put your runtime environment configuration here
}

class MyRuntimeEnvironment
	:public langbind::RuntimeEnvironment
{
public:
	// ... put your runtime environment host structures here

	MyRuntimeEnvironment( const MyRuntimeEnvironmentConfig* cfg)
	{
		// ... create your runtime environment from its configuration here
	}
}

WF_MODULE_BEGIN( "MyRuntimeEnvironment", "runtime environment for my programs")
 WF_RUNTIME_ENVIRONMENT( "my runtime environment", "runtimeenv", "myrunenv", MyRuntimeEnvironment, MyRuntimeEnvironmentConfig, initMyRuntimeEnvironment)
WF_MODULE_END
* \endcode
*
* \page AuthenticatorModule Authenticator module
* \code
class MyAuthenticationConfig
	:public config::NamedConfiguration
{
	// ... define your authentication configuration here (derived from config::NamedConfiguration)
};

class MyAuthenticatorSlice
	:public _Wolframe::AAAA::AuthenticatorSlice
{
public:
	virtual void dispose()
	{
		// ... destroy the object accordingly to the method is was created by the unit method _Wolframe::AAAA::AuthenticatorUnit::slice( const std::string&, const _Wolframe::net::RemoteEndpoint&)
	}

	virtual const char* className() const
	{
		return "MyAuthenticatorSlice";
	}

	virtual const std::string& identifier() const
	{
		// ... return the identifier of the authenticator
	}

	virtual void messageIn( const std::string& msg)
	{
		// ... process the message requested by 'status()const' here
	}

	virtual std::string messageOut()
	{
		// ... return the message to be sent announced by 'status()const' here
	}

	virtual _Wolframe::AAAA::Authenticator::Status status() const
	{
		// ... return the current status of the authenticator slice
	}

	virtual bool inputReusable() const
	{
		// ... return true, if the last message processed can be forwarded to another slice of the same mech
	}

	virtual User* user()
	{
		// ... 	return the authenticated user or NULL if not authenticated here
	}
};

class MyAuthenticationUnit
	:public _Wolframe::AAAA::AuthenticationUnit
{
public:
	MyAuthenticationUnit( const MyAuthenticationConfig& cfg);

	virtual const char* className() const
	{
		return "MyAuthentication";
	}

	const char** mechs() const
	{
		// ... return the mechs implemented by this authenticator unit here
	}

	MyAuthenticatorSlice* slice( const std::string& mech, const _Wolframe::net::RemoteEndpoint& client)
	{
		// ... create and return a new instance of an authenticator slice here
	}
};

WF_MODULE_BEGIN( "MyAuthenticator", "my authenticator module")
 WF_AUTHENTICATOR( "my authenticator", MyAuthenticatorUnit, MyAuthenticatorConfig)
WF_MODULE_END
* \endcode

* \page DatabaseModule Database interface module
* \code

class MyDatabaseUnit;

class MyDatabaseConfig
	:public _Wolframe::config::NamedConfiguration
{
	// ... define your database configuration structure here
};

class MyTransactionExecStatemachine
	:public _Wolframe::db::TransactionExecStatemachine
{
public:
	MyTransactionExecStatemachine( MyDatabaseUnit* unit)
	{
		// ... create a statemachine for one transaction on a database of 'unit' that will start with the next call of begin()
	}

	virtual const std::string& databaseID() const
	{
		// ... return the configuration identifier of your database here ...
	}

	virtual bool begin()
	{
		// ... begin of the current transaction
	}

	virtual bool commit()
	{
		// ... commit of the current transaction
	}

	virtual bool rollback()
	{
		// ... rollback of the current transaction
	}

	virtual bool start( const std::string& statement)=0;
	{
		// ... create a new statement instance from string
	}

	virtual bool bind( std::size_t idx, const types::VariantConst& value)
	{
		// ... bind a parameter (idx >= 1) of the current statement instance
	}

	virtual bool execute()
	{
		// ... execute the built instance of the current statement
	}

	virtual std::size_t nofColumns()
	{
		// ... get the number of columns of the last result, 0 if there was no result
	}

	virtual const char* columnName( std::size_t idx)
	{
		// ... get the name of a column by index (idx >= 1) of the last result
	}

	virtual types::VariantConst get( std::size_t idx)
	{
		// ... get the value of a column by index (idx >= 1) of the last result
	}

	virtual bool next()
	{
		// ... skip to the next result, return false, if there is no result left
	}

	virtual bool hasResult()
	{
		// ... return true, if the last database statement returned at least one result row
	}

	virtual const db::DatabaseError* getLastError()
	{
		// ... return the last database error as structure here
	}

	virtual bool isCaseSensitive()
	{
		// ... return true, if the database language is case sensitive (SQL is case insensitive)
	}
};

class MyDatabase
	:public _Wolframe::db::Database
{
public:
	virtual const std::string& ID() const
	{
		// ... return the identifier of the database here
	}

	virtual const char* className() const
	{
		return "MyDatabase";
	}

	virtual _Wolframe::db::Transaction* transaction( const std::string& name_)
	{
		return new _Wolframe::db::Transaction( name_, new MyTransactionExecStatemachine( this));
	}

	virtual const LanguageDescription* getLanguageDescription() const
	{
		static _Wolframe::db::LanguageDescription langdescr;
		return &langdescr;
	}
};

class MyDatabaseUnit
{
public:
	MyDatabaseUnit( const MyDatabaseConfig& config)
	{
		// ... create the database unit from configuration here
	}

	virtual const char* className() const
	{
		return "MyDatabaseUnit";
	}

	virtual const std::string& ID() const
	{
	}

	virtual Database* database()
	{
	}
};

WF_MODULE_BEGIN( "MyDatabase", "my database module")
 WF_DATABASE("MyDB",MyDatabaseUnit,MyDatabaseConfig)
WF_MODULE_END
* \endcode
*
*/

