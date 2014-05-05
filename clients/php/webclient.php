<?php
require 'session.php';
use Wolframe\Session as Session;

try
{
	$command = "";
	if( array_key_exists( "COMMAND", $_REQUEST ) )
	{
		$command = $_REQUEST['COMMAND'];
	}
	$body = string http_get_request_body();
	if ($body == NULL/*or empty*/)
	{
		$body = $_REQUEST['CONTENT'];
	}
	$conn = new Session( "127.0.0.1", 7661, NULL, "NONE");
	if (($result = $conn->request( $command, $body)) === FALSE)
	{
		throw new Exception( $conn->lasterror());
	}
	echo "<html><head><title>RESULT</title></head><body><p>" . $result . "</p></body></html>";
	unset( $conn);
}
catch ( \Exception $e)
{
	echo "<html><head><title>ERROR</title></head><body>" . $e->getMessage() . "</body></html>";
}
?>

