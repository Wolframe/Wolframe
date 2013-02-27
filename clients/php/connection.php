<?php
namespace Wolframe
{
class Connection
{
	private $context = NULL;
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

	public function __construct( $address, $port, $cafile, $local_cert, $keyfile)
	{
		$this->context = stream_context_create();
		$timeout = 30;

		stream_context_set_option( $this->context, 'ssl', 'verify_host', true);
		stream_context_set_option( $this->context, 'ssl', 'cafile', $cafile);
		stream_context_set_option( $this->context, 'ssl', 'local_cert', $local_cert);
		stream_context_set_option( $this->context, 'ssl', 'verify_peer', true);

		$this->socket = stream_socket_client("tcp://{$address}:{$port}", $errno, $errstr, $timeout, STREAM_CLIENT_CONNECT, $this->context);
		if ($this->socket === FALSE) throw $this->conn_exception( "socket creation failed");

		stream_set_blocking( $this->socket, true);
		stream_socket_enable_crypto( $this->socket, true, STREAM_CRYPTO_METHOD_SSLv3_CLIENT);
		stream_set_blocking( $this->socket, false);

		$this->alive = TRUE;
	}

	public function __destruct()
	{
		fclose( $this->socket);
	}

	public function writechunk( $data)
	{
		if (fwrite( $this->socket, $data, strlen( $data)) === FALSE) throw $this->conn_exception( "write failed");
	}

	public function readchunk()
	{
		if (($rt = fread( $socket, 4096)) === FALSE) throw $this->conn_exception( "read failed");
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
