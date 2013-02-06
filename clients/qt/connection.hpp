/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/

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
