Text File Authentication

The format of the file is:

<username>:<SHA-224 of the password>:<user info (name)>:.....(ignored)

The PLAIN TEXT protocol is very simple. It is one message in the format
	USER <username> PASS(WORD) <password>
and the answer is
	Hello <user name>
or
	Login failed
Obviously, depending on the result of the authentication combined
with the login authorization.

NOTE: The PLAIN TEXT protocol is veeeery BAD.
