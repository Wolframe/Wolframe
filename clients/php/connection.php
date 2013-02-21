<?php
namespace Wolframe
{
class Connection
{
	private $socket = NULL;
	private $readbuffer = "";
	private $alive = FALSE;

	public function isalive()
	{
		return $this->alive;
	}

	public function protocol_exception( $msg)
	{
		$this->alive = FALSE;
		return new \Exception( $msg);
	}

	private function conn_exception( $msg)
	{
		$err = socket_last_error( $this->socket);
		$str = $msg . "(" . $err . ": " . socket_strerror($err) .")";
		$this->alive = FALSE;
		return new \Exception( $str);
	}

	public function __construct( $address, $port)
	{
		$this->socket = socket_create( AF_INET, SOCK_STREAM, SOL_TCP);
		if ($this->socket === FALSE) throw $this->conn_exception( "socket creation failed");

		$result = socket_connect( $this->socket, $address, $port);
		if ($result === FALSE) throw $this->conn_exception( "connection failed");

		$this->alive = TRUE;
	}

	public function __destruct()
	{
		socket_shutdown( $this->socket);
		socket_close( $this->socket);
	}

	public function writechunk( $data)
	{
		if (socket_write( $this->socket, $data, strlen( $data)) === FALSE) throw $this->conn_exception( "write failed");
	}

	public function readchunk()
	{
		if (($rt = socket_read( $this->socket, 4096)) === FALSE) throw $this->conn_exception( "write failed");
		return $rt;
	}

	public function escapecontent( $content)
	{
		return str_replace( "\n." , "\n..", $content);
	}
 
	public function unescapecontent( $content)
	{
		return str_replace( "\n.." , "\n.", $content);
	}

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

	public function writeline( $line)
	{
		$this->writechunk( $line);
		$this->writechunk( "\r\n");
	}
	
	public function writedata( $data)
	{
		$this->writechunk( $this->escapecontent( $data));
		$this->writechunk( "\r\n.\r\n");
	}
} // class Connection
} // namespace Wolframe
?>
