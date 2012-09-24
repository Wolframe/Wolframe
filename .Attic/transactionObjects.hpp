
struct Input
{
	struct Element
	{
		enum Type
		{
			String,
			ResultColumn
		};
		Type type;
		std::string value;
		std::size_t resultColumnIdx;

		Element(){}
		Element( const Element& o)
			:type(o.type)
			,value(o.value)
			,resultColumnIdx(o.resultColumnIdx){}
	};
	struct Command
	{
		std::string name;
		std::vector<Element> arg;

		Command(){}
		Command( const CommandResult& o)
			:name(o.name)
			,arg(o.arg){}
	};
	std::vector<Command> cmd;
};

struct Output
{
	struct CommandResult
	{
		std::vector<string> columnName;
		std::vector< std::vector< std::string> > row;

		CommandResult(){}
		CommandResult( const CommandResult& o)
			:columnName(o.columnName)
			,row(o.row){}
	};
	std::vector<CommandResult> result;
};


