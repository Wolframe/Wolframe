#!/usr/bin/expect
#
# Start a wolframe daemon and communicate with it
#

package require Expect

set config ../../examples/pecho/pecho.conf
set server ../../examples/pecho/pechod

set timeout 5
set serverpid [spawn $server -c $config -f]

puts "pechod started with configuration file: $config (pid: $serverpid)"
# allow the server to start
sleep 5

set timeout 5
if { [catch {
	spawn telnet localhost 7661

	expect 	{
		timeout				{ return "--> oops. timeout occured waiting for server banner\n" }
		eof				{ return "--> EOF waiting for server banner: that's bad\n" }
		"OK expecting command\r"	{ send_user " --> Connected.\n" }
	}

	send "capa\r"
	expect -re "OK capa .*\r"         {send_user " --> Capabilities.\n"}

	send "echo tolower\r"
	send "ABc Def\r"
	expect -re "abc def\r"            {send_user " --> echo line 1.\n"}
	send "aBc def\r"
	expect -re "abc def\r"            {send_user " --> echo line 2.\n"}
	send ".\r"
	expect "OK expecting command\r"   {send_user " --> expecting next command.\n"}
	send "quit\r"
	expect "BYE"                      {send_user " --> Exit, close connection.\n"}

	expect 	{
		timeout	{ send_user " --> oops. timeout occured\n"; return 1 }
		eof	{ send_user " --> Connection closed. OK\n" }
	}
} reason] } {
	send_user "Error: $reason\n"
	exit 1
}
