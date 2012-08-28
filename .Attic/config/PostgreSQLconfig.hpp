struct PostgresSQLConfigStruct
{
	std::string identifier;
	std::string host;
	unsigned short port;
	std::string database;
	std::string user;
	std::string password;
	unsigned short connectTimeout;
	unsigned short connections;
	unsigned short acquireTimeout;

	static const serialize::StructDescriptionBase* getStructDescription();
};

static const serialize::StructDescriptionBase* PostgresSQLConfigStruct::getStructDescription()
{
	typedef PostgresSQLConfigStruct ThisStruct;
	struct ThisDescription :public serialize::StructDescription<ThisStruct>
	{
		ThisDescription()
		{
			(*this)
			( "identifier",		&ThisStruct::identifier)
			( "host",		&ThisStruct::host)
			( "port",		&ThisStruct::port)
			( "database",		&ThisStruct::database)
			( "user",		&ThisStruct::user)
			( "password",		&ThisStruct::password)
			( "connectTimeout",	&ThisStruct::connectTimeout)
			( "connections",	&ThisStruct::connections)
			( "acquireTimeout",	&ThisStruct::acquireTimeout)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}
