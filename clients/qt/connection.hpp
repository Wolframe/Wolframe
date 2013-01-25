#ifndef _CONNECTION_HPP_INCLUDED
#define _CONNECTION_HPP_INCLUDED

#include <QString>

struct ConnectionParameters	{
	QString		name;
	QString		host;
	unsigned short	port;
	bool		SSL;
	bool		clientCertificate;
	QString		SSLcertificate;
	QString		SSLkey;
	bool		SSLverify;
	QString		SSLCAbundle;
	unsigned short	timeout;

public:
	ConnectionParameters();

	// true if the connection is undefined
	bool isEmpty() const;

	// Return a description of the connection
	QString toString() const;

	// Check if the connection parameters are ok
	// Fills in the error string in case of an error
	bool check( QString& error ) const;

	// Check if the connection can be made
	// Fills in the error string in case of an error
	bool test( QString& error ) const;
};

#endif // _CONNECTION_HPP_INCLUDED
