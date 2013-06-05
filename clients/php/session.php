<?php
namespace Wolframe
{
require 'connection.php';
use Wolframe\Connection as Connection;

class Session extends Connection
{
	private $banner;
	private $requesterror = FALSE;

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

	function auth_NONE()
	{
		$this->getline( "OK");
	}

	function __construct( $address, $port, $sslopt, $authmethod)
	{
		parent::__construct( $address, $port, $sslopt);
		$this->banner = $this->readline();
		$this->getline( "OK");
		$this->writeline( "AUTH");
		$mechs = $this->getline( "MECHS");
		if (array_search( $authmethod, explode( " ", $mechs)) === FALSE)
		{
			throw $this->protocol_exception( "authentication method NONE not supported by server");
		}
		$this->writeline( "MECH " . $authmethod);
		if ($authmethod == "NONE")
		{
			$this->auth_NONE();
		}
		else
		{
			throw $this->protocol_exception( "authentication method '" . $authmethod . "' not supported by client");
		}
	}

	public function __destruct()
	{
		if ($this->isalive()) $this->writeline( "QUIT");
		parent::__destruct();
	}

	public function lasterror()
	{
		return $this->requesterror;
	}

	public function request( $query)
	{
		$requesterror = FALSE;
		$this->writeline( "REQUEST");
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
