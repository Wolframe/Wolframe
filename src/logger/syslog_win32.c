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

/*
 * syslog-client.c - syslog client implementation for windows
 *
 * Created by Alexander Yaworsky
 *
 * THIS SOFTWARE IS NOT COPYRIGHTED
 *
 * This source code is offered for use in the public domain. You may
 * use, modify or distribute it freely.
 *
 * This code is distributed in the hope that it will be useful but
 * WITHOUT ANY WARRANTY. ALL WARRANTIES, EXPRESS OR IMPLIED ARE HEREBY
 * DISCLAIMED. This includes but is not limited to warranties of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/**
 * \file syslog_win32.c
 * \brief implementation of a syslog client on Windows
 *
 * It is roughly based on the one of http://syslog-win32.sourceforge.net,
 * changed in the way it's configured (we can't have a /etc/syslog.conf
 * on Windows, host and port must be injectable through a function from
 * the Wolframe configuration), in the way the messages are composed using
 * newer string functions, using WSock2 funtions like getaddrinfo and
 * some small other things like added documentation.
*/

#include "logger/syslog_win32.h"

#include <Winsock2.h>
#include <string.h>
#include <stdio.h>

static BOOL initialized = FALSE;
static BOOL wsa_initialized = FALSE;

static int log_mask = 0xFF;
static const char *syslog_ident = NULL;
static int syslog_facility = 0;
static char str_pid[40];
static char local_hostname[MAX_COMPUTERNAME_LENGTH+4];

static char syslog_hostname[MAX_COMPUTERNAME_LENGTH+4] = "localhost";
static unsigned short syslog_port = 0;

/* UDP socket and data to use for sending messages */
static SOCKADDR_IN sa_logger;
static SOCKET sock = INVALID_SOCKET;
#define SYSLOG_DGRAM_SIZE 1024
static char datagram[SYSLOG_DGRAM_SIZE];
static int datagram_size = SYSLOG_DGRAM_SIZE;

/******************************************************************************
 * init_logger_addr
 *
 * Read configuration file syslog.host. This file should contain host address
 * and, optionally, port. Initialize sa_logger. If the configuration file does
 * not exist, use localhost:514.
 * Returns: 0 - ok, -1 - error.
 *
static void init_logger_addr()
{
    char pathname[ FILENAME_MAX ];
    char *p;
    FILE *fd;
    char host[256];
    struct hostent * phe;

    memset( &sa_logger, 0, sizeof(SOCKADDR_IN) );
    sa_logger.sin_family = AF_INET;

    phe = gethostbyname( host );
    if( !phe )
        goto use_default;

    memcpy( &sa_logger.sin_addr.s_addr, phe->h_addr, phe->h_length );

    if( p )
        sa_logger.sin_port = htons( (unsigned short) strtoul( p, NULL, 0 ) );
    else
        sa_logger.sin_port = htons( SYSLOG_PORT );
    return;

use_default:
}

void openlog( char* ident, int option, int facility )
{

    init_logger_addr();
*/

/* reset logmask?
 * handle more options
 */
void openlog( const char* ident, int option, int facility )
{
	BOOL failed = TRUE;
	WSADATA wsd;
	SOCKADDR_IN sa_local;
	int size;
    DWORD n;
	
	/* allow idempotent calls, first options are taken, use closelog
	 * before openlog if you want to reconfigure the syslog interface
	 */
	if( initialized ) return;

	/* Initialize Windows Socket DLL */
	if( WSAStartup( MAKEWORD( 2, 2 ), &wsd ) != 0 ) goto DONE;
	wsa_initialized = TRUE;

	/* Get the socket address and port of the logging destination */
    memset( &sa_logger, 0, sizeof( SOCKADDR_IN ) );
    sa_logger.sin_family = AF_INET;
	sa_logger.sin_addr.S_un.S_addr = htonl( 0x7F000001 );
	sa_logger.sin_port = htons( 514 );

	/* Create a UDP socket */
	/* for 1..100 loop? needed? */
	sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP /* was 0? */ );
	if( sock == INVALID_SOCKET ) goto DONE;

	/* from RFC 3164: The client should connect from a constant port, if possible
	 * also port 514. but if we are on the same machine with loopback the port
	 * is taken?
	 */
	
	/* bind to the socket */
	memset( &sa_local, 0, sizeof( SOCKADDR_IN ) );
	sa_local.sin_family = AF_INET;
	if( bind( sock, (SOCKADDR *)&sa_local, sizeof( SOCKADDR_IN ) ) != 0 ) goto DONE;

	/* determine the maximal size of a datagram packet we can send */
	size = sizeof( datagram_size );
	if( getsockopt( sock, SOL_SOCKET, SO_MAX_MSG_SIZE, (char *)&datagram_size, &size ) != 0 ) goto DONE;
	if( datagram_size > sizeof( datagram ) ) datagram_size = sizeof( datagram );
	
	/* set global facility and ident */
	syslog_facility = facility ? facility : LOG_USER;
	syslog_ident = ident;

	/* by RFC 3164 we should put here the name of the local machine */
    n = sizeof( local_hostname );
    if( !GetComputerName( local_hostname, &n ) ) goto DONE;
	
	/* interpret options, currently we use only LOG_PID */
	if( option & LOG_PID )
		_snprintf_s( str_pid, sizeof( str_pid ), _TRUNCATE, "[%lu]", GetCurrentProcessId( ) );
    else
		str_pid[0] = '\0';
	
	/* install C cleanup function */
	if( atexit( closelog ) ) goto DONE;
	
	/* if we get here, everything's peachy */
	failed = FALSE;
	
DONE:
    if( failed ) {
        if( sock != INVALID_SOCKET ) {
			closesocket( sock );
			sock = INVALID_SOCKET;
		}
        if( wsa_initialized ) {
			wsa_initialized = FALSE;
			WSACleanup( );
		}
    }
    initialized = !failed;
}

extern int setlogmask( int mask )
{
	/* set log mask, return old one */
	int old_mask = log_mask;
	
	if( mask ) log_mask = mask;	
	
	return old_mask;
}

void syslog( int pri, char* fmt, ... )
{
	va_list ap;
	va_start( ap, fmt );
	vsyslog( pri, fmt, ap );
	va_end( ap );
}
	
void vsyslog( int pri, char* fmt, va_list ap )
{
	/* from RFC 3164: Mmm is the English language abbreviation for the month of the
	 * year with the first character in uppercase and the other two
	 * characters in lowercase.  The following are the only acceptable
	 * values:
	 *
	 * Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec
	 */
    static char *month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	                        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

	int len;
	SYSTEMTIME t;

	/* do not log if we are not inside the logging mask */
	if( !( LOG_MASK( LOG_PRI( pri ) ) & log_mask ) ) return;

	/* open default logger, will return immediatelly if already
	 * registered before */
	openlog( NULL, 0, pri & LOG_FACMASK );

	/* do not log if not initialized after opening a default log */
	if( !initialized ) return;

	if( !( pri & LOG_FACMASK ) ) pri |= syslog_facility;

	/* from RFC 3164: .. hh:mm:ss is the local time. .. */
	GetLocalTime( &t );
	
	/* the PRI and the HEADER with TIMESTAMP and HOSTNAME as
	 * well as the TAG field of the MSG part
	 */
	len = _snprintf_s( datagram, datagram_size, _TRUNCATE, "<%d>%s %2d %02d:%02d:%02d %s %s%s: ",
		pri, month[t.wMonth-1], t.wDay, t.wHour, t.wMinute, t.wSecond,
		local_hostname, syslog_ident ? syslog_ident : "", str_pid );

	/* append now the formatted user message */
	(void)_vsnprintf_s( datagram + len, datagram_size - len, _TRUNCATE, fmt, ap );

	/* send as datagram, we are not really interested in errors here */
    (void)sendto( sock, datagram, strlen( datagram ), 0, (SOCKADDR *)&sa_logger, sizeof( SOCKADDR_IN ) );
}

void closelog( )
{
	if( !initialized ) return;
	
	if( sock != INVALID_SOCKET ) (void)closesocket( sock );
	if( wsa_initialized ) WSACleanup( );

    sock = INVALID_SOCKET;
	wsa_initialized	= FALSE;
	initialized = FALSE;	
}

void set_syslogd_data( const char *hostname, unsigned short port )
{
	strncpy( syslog_hostname, hostname,
		( strlen( hostname ) > MAX_COMPUTERNAME_LENGTH ) ? strlen( hostname ) : MAX_COMPUTERNAME_LENGTH );
	syslog_port = port;
}
