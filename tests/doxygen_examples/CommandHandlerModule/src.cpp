#include "appdevel/commandHandlerModuleMacros.hpp"
#include "appdevel/moduleFrameMacros.hpp"
#include "cmdbind/commandHandlerUnit.hpp"
#include "serialize/descriptiveConfiguration.hpp"
#include <vector>

class MyCommandHandlerConfig
	:public _Wolframe::serialize::DescriptiveConfiguration
{
public:
	// ... define your configuration data members here

	static const _Wolframe::serialize::StructDescriptionBase* getStructDescription()
	{
		// ... return your introspection description reference of the configuration here
		return 0;
	}
	MyCommandHandlerConfig( const char* /*classname*/, const char* title, const char* logprefix, const char* /*subsection*/)
		:_Wolframe::serialize::DescriptiveConfiguration( title, "authentication", logprefix, getStructDescription())
	{
		setBasePtr( (void*)this); // ... mandatory to set pointer to start of configuration
	}
};

class MyCommandHandlerUnit
	:public _Wolframe::cmdbind::CommandHandlerUnit
{
public:
	// ... your command handler unit definition is here
	MyCommandHandlerUnit( const MyCommandHandlerConfig* /*cfg*/)
	{
		// ... required constructor from configuration
	}
	virtual bool loadPrograms( const _Wolframe::proc::ProcessorProviderInterface* /*provider*/)
	{
		// ... load your command handler programs here
		return true;
	}

	virtual std::vector<std::string> commands() const
	{
		// ... return the commands of the command handler here
		return std::vector<std::string>();
	}

	virtual _Wolframe::cmdbind::CommandHandler* createCommandHandler( const std::string& /*cmdname*/, const std::string& /*docformat*/)
	{
		// ... create and return an instance of a command handler for executing the command cmdname here
		return 0;
	}
};

WF_MODULE_BEGIN( "MyCommandHandler", "my command handler short description")
 WF_COMMAND_HANDLER( "MyCommandHandler", "cmdhandler", "mycmd", MyCommandHandlerUnit, MyCommandHandlerConfig)
WF_MODULE_END
