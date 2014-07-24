#include "config/configurationBase.hpp"
#include "serialize/descriptiveConfiguration.hpp"
#include "serialize/struct/structDescription.hpp"
#include "config/configurationTree.hpp"

class MyConfigSubstruct
{
public:
	static const _Wolframe::serialize::StructDescriptionBase* getStructDescription();

public:
	int identifier;
	std::string filename;
};

class MyConfig
	:public _Wolframe::serialize::DescriptiveConfiguration
{
public:
	static const _Wolframe::serialize::StructDescriptionBase* getStructDescription();

public:
	// Our configuration elements:
	std::vector<MyConfigSubstruct> files;
	int classifier;
	std::string name;

	// Default constructor that declares the name of the structure
	// to be "MyConfig" and the logging prefix to be "MyClass/MyApp":
	MyConfig()
		:_Wolframe::serialize::DescriptiveConfiguration( "MyConfig", "MyClass", "MyApp", getStructDescription())
	{
		setBasePtr( (void*)this); // ... mandatory to set pointer to start of configuration
	}

	virtual bool parse( const _Wolframe::config::ConfigurationNode& cfgTree, const std::string& node,
				const _Wolframe::module::ModulesDirectory* modules )
	{
		// ... if your structure has some more initializations than the pure structure mapping you can overload this method
		if (_Wolframe::serialize::DescriptiveConfiguration::parse(cfgTree,node,modules))
		{
			// ... you can do some further initializations and checks (e.g. mapping strings to enums) here
			return true;
		}
		else
		{
			return false;
		}
	}
	
	virtual bool check() const
	{
		// ... you can do some additional semantic checks and test if resources are available here
		return true;
	}

	virtual void setCanonicalPathes( const std::string& /*refPath*/)
	{
		// ... you can expand all local path references with _Wolframe::utils::getCanonicalPath( const std::string&, const std::string&) here
	}

	virtual void print( std::ostream& /*os*/, size_t /*indent*/ = 0 )
	{
		// ... if you do not like the standard way of priting a structure, you can define your way here
	}
};

// Introspection description implementation for MyConfigSubstruct:
const _Wolframe::serialize::StructDescriptionBase* MyConfigSubstruct::getStructDescription()
{
	struct ThisDescription :public _Wolframe::serialize::StructDescription<MyConfigSubstruct>
	{
	ThisDescription()
	{
		(*this)
		( "identifier", &MyConfigSubstruct::identifier)	.mandatory()
		( "file", &MyConfigSubstruct::filename)		.mandatory()
		;
	}
	};
	static const ThisDescription rt;
	return &rt;
}

// Introspection description implementation for MyConfig:
const _Wolframe::serialize::StructDescriptionBase* MyConfig::getStructDescription()
{
	struct ThisDescription :public _Wolframe::serialize::StructDescription<MyConfig>
	{
	ThisDescription()
	{
		(*this)
		( "files", &MyConfig::files)
		( "classifier", &MyConfig::classifier)	.optional()
		( "name", &MyConfig::name)		.mandatory()
		;
	}
	};
	static const ThisDescription rt;
	return &rt;
}


#include <iostream>
int main()
{
	MyConfig config;
	config.print( std::cout);
}


