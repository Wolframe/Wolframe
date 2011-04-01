/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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

///
/// \file syslog_win32.c
/// \brief implementation of a syslog client on Windows
///

#include "logger/syslog_win32.h"

#include <Winsock2.h>

static BOOL initialized = FALSE;
static int log_mask = 0xFF;

void openlog( const char* ident, int option, int facility )
{
	WSADATA wsd;
	
	if( initialized ) return ;
	
	if( WSAStartup( MAKEWORD( 2, 2 ), &wsd ) != 0 ) return;

	initialized = TRUE;
}

void syslog( int pri, char* fmt, ... )
{
	va_list ap;
	va_start( ap, fmt );
	vsyslog( pri, fmt, ap );
	va_end( ap );
}

extern int setlogmask( int mask )
{
	int old_mask = log_mask;
	
	if( mask ) log_mask = mask;	
	
	return old_mask;
}
	
void vsyslog( int pri, char* fmt, va_list ap )
{
	if( !initialized ) return;
}

void closelog( )
{
	if( !initialized ) return;
	
    WSACleanup( );
	initialized = FALSE;	
}

void set_syslogd_data( const char *hostname, unsigned short port )
{
}
