//
// database.hpp - SMERP base database class
//

#include "appConfig.hpp"


#ifndef _DATABASE_HPP_INCLUDED
#define _DATABASE_HPP_INCLUDED

namespace _Wolframe	{
	namespace Database	{

		/// database type
		enum DatabaseType	{
			DBTYPE_POSTGRESQL,
			DBTYPE_SQLITE
		};

		/// database configuration
		struct DatabaseConfig : public _Wolframe::ConfigurationBase
		{
		public:
			DatabaseType		type;
			std::string		host;
			unsigned short		port;
			std::string		name;
			std::string		user;
			std::string		password;
			unsigned short		connections;
			unsigned short		timeout;

		/// ConfigurationBase virtual functions
			DatabaseConfig() : _Wolframe::ConfigurationBase( "database" )	{}
			bool parse();
			bool check();
			bool test();
			void print( std::ostream& os ) const;
		};

	} // namespace Database
} // namespace _Wolframe

#endif // _DATABASE_HPP_INCLUDED
