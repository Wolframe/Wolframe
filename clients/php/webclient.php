<?php
require 'session.php';
use Wolframe\Session as Session;

try
{
	$sslpath = "./SSL";
	$sslopt = array(
		"local_cert" => "$sslpath/combinedcert.pem",
		"verify_peer" => false
	);

	$command = "";
	if( array_key_exists( "COMMAND", $_REQUEST ) )
	{
		$command = $_REQUEST['COMMAND'];
	}

	if( array_key_exists( "CONTENT", $_REQUEST ) )
	{
		$conn = new Session( "127.0.0.1", 7962, $sslopt, "NONE");
		if (($result = $conn->request( $command, $_REQUEST['CONTENT'])) === FALSE)
		{
			throw new Exception( $conn->lasterror());
		}
		unset( $conn);
	}
	else
	{
		var_dump( $_REQUEST);
	}
}
catch ( \Exception $e)
{
	echo "<html><head><title>ERROR</title></head><body>" . $e->getMessage() . "</body></html>";
}
?>

