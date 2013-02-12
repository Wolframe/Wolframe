<?php

class WolframeClient
{
	protected $connParams;
	protected $lastError;
	protected $fp;
	
	public function WolframeClient( $connParams )
	{
		$this->connParams = $connParams;
	}
	
	public function connect( )
	{
		$connStr = "tcp://" . $this->connParams["host"] . ":" . $this->connParams["port"];
		$this->fp = stream_socket_client( $connStr, $errno, $error, 10 );

		if( !$this->fp ) {
			$this->lastError = $error;
			return FALSE;
		}
		
		if( !stream_set_blocking( $this->fp, 0 ) ) {
			$this->lastError = "Can't enable non-blocking socket";
			return FALSE;
		}
		
		return TRUE;
	}
	
	public function disconnect( )
	{
		fclose( $this->fp );
	}

	public function getLastError( ) {
		return $this->lastError;
	}

}
