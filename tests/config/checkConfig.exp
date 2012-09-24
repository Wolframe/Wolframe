#!/usr/bin/expect
#
# Start a wolframe daemon and check the configuration
#
# Arguments:
#     configuration file
#     wolframed return code
#     error / warning / ok message
#

if { [llength $argv] != 3 } {
    puts "usage: checkConfig <config file> <return code> <error / warning / ok message>"
    exit 1
}

set config [lindex $argv 0]		;# 1st script argument
set expected [lindex $argv 1]		;# 2nd script argument
set outmsg [lindex $argv 2]		;# 3rd script argument

puts "Configuration file: $config, expecting exit code $expected and message \"$outmsg\""

set timeout 3
spawn ../../src/wolframed -c $config -t
# maybe allow the server to start
# sleep 2

expect 	{
	timeout		{ send_user "Process timed out.\n" }
	"$outmsg"	{ puts "Got expected message: $expect_out(0,string)\n" }
	eof		{ puts "Got EOF before the expected message.\n"
			  exit 1
			}
}

catch wait reason
set procid [lindex $reason 0]
if { [lindex $reason 2] == 0 } {
	set retval [lindex $reason 3]
	puts "Pid $procid return code: $retval\n"
} else {
	puts "Pid $procid, some execution error occured. wait returned $reason\n"
}

if { $retval != $expected }	{
	exit 2
}