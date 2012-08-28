
struct SQLite3ConfigStruct
{
	std::string identifier;
	std::string filename;
	bool flag;

	static const serialize::StructDescriptionBase* getStructDescription();
};

static const serialize::StructDescriptionBase* SQLite3ConfigStruct::getStructDescription()
{
	typedef SQLite3ConfigStruct ThisStruct;
	struct ThisDescription :public serialize::StructDescription<ThisStruct>
	{
		ThisDescription()
		{
			(*this)
			( "identifier",		&ThisStruct::identifier)
			( "filename",		&ThisStruct::filename)
			( "flag",		&ThisStruct::flag)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}
