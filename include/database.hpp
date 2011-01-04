//
// database.hpp - SMERP base database class
//

#include "appConfig.hpp"


#ifndef _DATABASE_HPP_INCLUDED
#define _DATABASE_HPP_INCLUDED

namespace _SMERP	{
	namespace Database	{
		/// database type
		enum DatabaseType	{
			DBTYPE_POSTGRESQL,
			DBTYPE_SQLITE
		};
		/// database configuration
		struct DatabaseConfig : public _SMERP::ConfigurationBase
		{
		public:
			DatabaseType		type;
			std::string		dbHost;
			unsigned short		dbPort;
			std::string		dbName;
			std::string		dbUser;
			std::string		dbPassword;
			unsigned short		connections;
			unsigned short		timeout;

		/// ConfigurationBase virtual functions
			DatabaseConfig() : _SMERP::ConfigurationBase( "database" )	{}
			bool parse();
			bool check();
			bool test();
			void print( std::ostream& os ) const;
		};

	} // namespace Database
} // namespace _SMERP

#endif // _DATABASE_HPP_INCLUDED
