<?php
require 'session.php';
use Wolframe\Session as Session;

/*
 Same as webclient.php but using SSL to communicate with the Wolframe server.
 Forwards HTTP request to a wolframe server via TCP/IP with SSL and returns the result
 as the HTTP request answer.

 Called with 2 request parameters (GET or POST)

 - CMD : (optional) Command prefix that determines together with the document type of the content sent,
	 what command to execute. If not specified then the document type only determines what is executed
	 on the server. The document type of the content is defined in JSON as an attribute named 'doctype'
	 and in XML as stem of the DOCYPE SYSTEM attribute or as stem of the xmlns:xsi root element
	 attribute.

 - CONTENT : Content of the document to process (in case of a POST request the content can be
	alternatevely specified as HTTP request body).
*/

try
{
	$sslpath = "./SSL";
	$sslopt = array(
		"local_cert" => "$sslpath/combinedcert.pem",
		"verify_peer" => false
	);

	$cmd = NULL;
	if( array_key_exists( "CMD", $_REQUEST ) )
	{
		$cmd = $_REQUEST['CMD'];
	}
	$body = $_REQUEST['CONTENT'];

	if ($body == NULL/*or empty*/ && $_SERVER['REQUEST_METHOD'] == 'POST')
	{
		$body = http_get_request_body();
	}
	$authopt = array(
		"mech" => "NONE",
		"username" => "gunibert",
		"password" => "bork123"
	);

	$conn = new Session( "127.0.0.1", 7962, $sslopt, $authopt);
	if (($result = $conn->request( $cmd, $body)) === FALSE)
	{
		echo "<html><head><title>FAILED</title></head><body>" . $conn->lasterror() . "</body></html>";
	}
	else
	{
		echo "<html><head><title>RESULT</title></head><body><p>" . $result . "</p></body></html>";
	}
	unset( $conn);
}
catch ( \Exception $e)
{
	echo "<html><head><title>ERROR</title></head><body>" . $e->getMessage() . "</body></html>";
}
?>

