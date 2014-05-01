<?php
namespace Wolframe
{
/*
* Connection
* Object representing a connection to a Wolframe server with methods for protocol support
* (send and receive messages).
*/
class Connection
{
	private $context = NULL;	/* connection context */
	private $socket = NULL;		/* connection socket */
	private $readbuffer = "";	/* buffer for chunk sent by the server and received by the client */
	private $alive = FALSE;		/* flag for connection status {alive,closed}*/

	/* Check if connection is alive */
	public function isalive()
	{
		return $this->alive;
	}

	/* Get an exception for the session layer to throw */
	public function protocol_exception( $msg)
	{
		$this->alive = FALSE;
		return new \Exception( $msg);
	}

	/* Get a connection exception to throw */
	private function conn_exception( $msg)
	{
		if ($this->socket === FALSE) return new \Exception( "connection failed");
		$err = socket_last_error( $this->socket);
		$str = $msg . "(" . $err . ": " . socket_strerror($err) .")";
		$this->alive = FALSE;
		return new \Exception( $str);
	}

	/* Constructor */
	public function __construct( $address, $port, $ssloptions)
	{
		$this->context = stream_context_create();
		$timeout = 30;
		$protocol = "tcp";

		foreach ($ssloptions as $key => $value)
		{
			stream_context_set_option( $this->context, 'ssl', $key, $value);
			$protocol = "ssl";
		}

		$this->socket = stream_socket_client("$protocol://{$address}:{$port}", $errno, $errstr, $timeout, STREAM_CLIENT_CONNECT, $this->context);
		if ($this->socket === FALSE) throw $this->conn_exception( "socket creation failed");

		$this->alive = TRUE;
	}

	/* Destructor */
	public function __destruct()
	{
		fclose( $this->socket);
	}

	/* Write a data chunk on the socket and throw if it fails */
	public function writechunk( $data)
	{
		if (fwrite( $this->socket, $data, strlen( $data)) === FALSE) throw $this->conn_exception( "write failed");
	}

	/* Read a data chunk on the socket and throw if it fails */
	public function readchunk()
	{
		if (($rt = fread( $this->socket, 4096)) === FALSE) throw $this->conn_exception( "read failed");
		return $rt;
	}

	/* Escape CRLFdot in a string to CRLFdotdot */
	public function escapecontent( $content)
	{
		return str_replace( "\n." , "\n..", $content);
	}
 
	/* Substitute CRLFdotdot in a string with CRLFdot (inverse of escapecontent) */
	public function unescapecontent( $content)
	{
		return str_replace( "\n.." , "\n.", $content);
	}

	/* Read a line from socket */
	public function readline()
	{
		for (;;)
		{
			$ar = explode( "\n", $this->readbuffer, 2);
			if (count($ar) == 2)
			{
				$this->readbuffer = $ar[1];
				return str_replace( "\r", "", $ar[0]);
			}
			$this->readbuffer .= $this->readchunk();
		}
	}

	/* Get the end of the data chunk and the start of the follow data after the end of data marker as pair of integer offsets (EoD,followData)*/
	public function end_of_data( $data)
	{
		$pos = 0;
		$next = 0;
		while (($pos = strpos($data , "\n.", $pos)) !== FALSE)
		{
			$epos = strlen( $data);
			if ($pos+1 < $epos && ord($data[$pos+2]) == 13 && ord($data[$pos+3]) == 10)
			{
				if ($pos > 0 && ord($data[$pos-1]) == 13)
				{
					return array( 0 => $pos -1, 1 => ($pos + 4));
				}
				else
				{
					return array( 0 => $pos, 1 => ($pos + 4));
				}
			}
			if ($pos < $epos && ord($data[$pos+2]) == 10)
			{
				if ($pos > 0 && ord($data[$pos-1]) == 13)
				{
					return array( 0 => $pos -1, 1 => ($pos + 3));
				}
				else
				{
					return array( 0 => $pos, 1 => ($pos + 3));
				}
			}
			$pos += 2;
		}
		return FALSE;
	}

	/* Read CRLFdot escaped block of data ending with CRLFdotCRLF from socket */
	public function readdata()
	{
		for (;;)
		{
			if (($eod = $this->end_of_data( $this->readbuffer)) !== FALSE)
			{
				$rt = substr( $this->readbuffer, 0, $eod[0]);
				$rest = substr( $this->readbuffer, $eod[1], strlen($this->readbuffer) - $eod[1]);
				$this->readbuffer = $rest;
				return $this->unescapecontent( $rt);
			}
			$this->readbuffer .= $this->readchunk();
		}
	}

	/* Write a line to socket */
	public function writeline( $line)
	{
		$this->writechunk( $line);
		$this->writechunk( "\r\n");
	}
	
	/* Write a block of data as CRLFdot escaped chunk terminated with CRLFdotCRLF to socket */
	public function writedata( $data)
	{
		$this->writechunk( $this->escapecontent( $data));
		$this->writechunk( "\r\n.\r\n");
	}
} // class Connection
} // namespace Wolframe
