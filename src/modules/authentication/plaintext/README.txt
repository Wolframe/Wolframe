Plain Text Authentication

NOTE: this module is here as an example only !!!

Although is should be fully functional, it is BAD practice to have
the passwords in plain text.

The format of the file is:

<username>:<password>:<user info (name)>:.....(ignored)

The protocol is very simple. It is one message in the format
	USER <username> PASS(WORD) <password>
and the answer is
	Hello <user name>
or
	Login failed
Obviously, depending on the result of the authentication combined
with the login authorization.
