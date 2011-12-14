#include <postgresql/libpq-fe.h>
#include <string>
#include <iostream>

int main( void )
{
	PGconn* conn = PQconnectdb( "host = 'localhost' port = 5432 dbname = 'wolframe' user = 'wolframe' password = 'blabla' connect_timeout = 10" );
	if ( conn == NULL )	{
		std::cout << "PostgreSQL PQconnectdb returned NULL";
		return 1;
	}
	else	{
		ConnStatusType stat = PQstatus( conn );
		switch( stat )	{
			case CONNECTION_OK:
				std::cout << "Connected OK. Message: " << PQerrorMessage( conn );
				break;
			case CONNECTION_BAD:
				std::cout << "Connection BAD. Message: " << PQerrorMessage( conn );
				break;
			case CONNECTION_STARTED:
				std::cout << "Connection STARTED. Message: " << PQerrorMessage( conn );
				break;
			case CONNECTION_MADE:
				std::cout << "Connection MADE. Message: " << PQerrorMessage( conn );
				break;
			case CONNECTION_AWAITING_RESPONSE:
				std::cout << "Connection AWAITING RESPONSE. Message: " << PQerrorMessage( conn );
				break;
			case CONNECTION_AUTH_OK:
				std::cout << " Connection AUTH OK. Message: " << PQerrorMessage( conn );
				break;
			case CONNECTION_SSL_STARTUP:
				std::cout << "Connection SSL start. Message: " << PQerrorMessage( conn );
				break;
			case CONNECTION_SETENV:
				std::cout << "Connection SETENV. Message: " << PQerrorMessage( conn );
				break;
			case CONNECTION_NEEDED:
				std::cout << "Connection NEEDED. Message: " << PQerrorMessage( conn );
				break;
		}
	}

	PGTransactionStatusType tstat = PQtransactionStatus( conn );
	switch( tstat )	{
		case PQTRANS_IDLE:
			PQfinish( conn );
			std::cout << "Connection idle";
			break;
		case PQTRANS_ACTIVE:
			PQfinish( conn );
			std::cout << "Connection active";
			break;
		case PQTRANS_INTRANS:
			PQfinish( conn );
			std::cout << "Connection in transaction";
			break;
		case PQTRANS_INERROR:
			PQfinish( conn );
			std::cout << "Connection in transaction error";
			break;
		case PQTRANS_UNKNOWN:
			PQfinish( conn );
			std::cout << "Connection status unknown";
			break;
	}

	return 0;
}
