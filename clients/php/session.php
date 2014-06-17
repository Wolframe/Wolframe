<?php
namespace Wolframe
{
require 'connection.php';
require 'authentication/textfile.php';

use Wolframe\Connection as Connection;

/*
* Session
* Object based on Connection with higher level commands hiding protocol details from the caller.
*/
class Session extends Connection
{
	private $banner;
	private $requesterror = FALSE;

	/* Get a line from socket and check if it starts as expected (success case) */
	function getline( $expect)
	{
		$ln = explode( " ", $this->readline(), 2);
		if ($ln[0] == $expect)
		{
			if (count( $ln) == 2) return $ln[1];
			return "";
		}
		if ($ln[0] == "ERR") throw $this->protocol_exception( "server error: " . $ln[1]);
		if ($ln[0] == "BAD") throw $this->protocol_exception( "protocol error: " . $ln[1]);
		if ($ln[0] == "BYE") throw $this->protocol_exception( "server terminated session: " . $ln[1]);
		throw new Exception( "illegal state in protocol. expected " . $expect . " but got " . $ln[0]);
	}

	/* Authentication mechanism WOLFRAME_CRAM */
	function auth_WOLFRAME_CRAM( $username, $password)
	{
		error_log( "START AUTHENTICATION\n");

		// 1. The client sends a 256 bit seed followed by a HMAC-SHA1 of the (seed, username)
		$this->writedata( userHash( $username)); 
		error_log( "SEND USER NAME " + $username + "\n", 3, "/var/tmp/mylog.txt");

		// 2. If the server finds the user it will reply with a seed and a challenge. 
		//	If the user is not found it will reply with a random challenge for not
		//	giving any information to the client. The procedure will continue.
		$challenge = $this->readdata();
		error_log( "GOT CHALLENGE [" + $challenge + "]\n", 3, "/var/tmp/mylog.txt");

		// 3. The client returns a response. The response is computed from the PBKDF2 
		//	of the seed and the challenge.
		$this->writedata( CRAMresponse( $password, $challenge));
		error_log( "SEND PASSWORD\n", 3, "/var/tmp/mylog.txt");

		// 4. the server tries to authenticate the user and returns "OK" in case
		//	of success, "ERR" else.
		$this->getline("OK");
		error_log( "GOT AUTHENTICATED\n", 3, "/var/tmp/mylog.txt");
	}

	/** Constructor
	* @param[in] address wolframe service ip
	* @param[in] port wolframe service port
	* @param[in] sslopt structure with named options for SSL
	* @param[in] authopt structure with named options for authentication (depending on authentication mech)
	*/
	function __construct( $address, $port, $sslopt, $authopt)
	{
		parent::__construct( $address, $port, $sslopt);
		$this->banner = $this->readline();
		$this->getline( "OK");
		$this->writeline( "AUTH");
		$mechs = $this->getline( "MECHS");
		if ($authopt == NULL)
		{
			$authmethod = "NONE";
			$authopt = array();
		}
		else
		{
			$authmethod = $authopt[ 'mech'];
			if ($authmethod == NULL)
			{
				$authmethod = "NONE";
			}
		}
		if (array_search( $authmethod, explode( " ", $mechs)) === FALSE)
		{
			throw $this->protocol_exception( "authentication method '" . $authmethod . "' not supported by server");
		}
		$this->writeline( "MECH " . $authmethod);
		$this->getline( "OK");

		if (0==strcasecmp( $authmethod, "NONE"))
		{
			// ... accepted without authentication 
		}
		else if (0==strcasecmp( $authmethod, "WOLFRAME-CRAM"))
		{
			$this->auth_WOLFRAME_CRAM( $authopt['username'], $authopt['password']);
		}
		else
		{
			throw $this->protocol_exception( "authentication method '" . $authmethod . "' not supported by client");
		}
	}

	/* Destructor */
	public function __destruct()
	{
		if ($this->isalive()) $this->writeline( "QUIT");
		parent::__destruct();
	}

	/* Get the error of the last request (non exceptional processing error) */
	public function lasterror()
	{
		return $this->requesterror;
	}

	/* Send a request to the server */
	public function request( $command, $query)
	{
		$requesterror = FALSE;
		if ($command == "")
		{
			$this->writeline( "REQUEST");
		}
		else
		{
			$this->writeline( "REQUEST $command");
		}
		$this->writedata( $query); 
		$ln = explode( " ", $this->readline(), 2);
		if ($ln[0] == "OK") return "";
		if ($ln[0] == "ANSWER")
		{
			$rt = $this->readdata();
			$ln = explode( " ", $this->readline(), 2);
			if ($ln[0] == "OK") return $rt;
			if ($ln[0] == "ERR")
			{
				$this->requesterror = $ln[1];
				return FALSE;
			}
			throw $this->protocol_exception( "protocol error: 'OK' or 'ERR' expected instead of '" . $ln[0] . "'");
		}
		if ($ln[0] == "ERR") throw $this->protocol_exception( "server error: " . $ln[1]);
		throw $this->protocol_exception( "protocol error: 'OK' or 'ERR' or 'ANSWER' expected instead of '" . $ln[0] . "'");
	}

} // class Session
} // namespace Wolframe
