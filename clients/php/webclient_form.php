<?php
require 'session.php';
use Wolframe\Session as Session;

/*
 Forwards HTTP GET request to a wolframe server via TCP/IP plain and returns the result
 as the HTTP request answer. The HTTP GET parameters without 'CMD' are transformed into 
 a JSON request set sent to the server. The command prefix is specified with the parameter
 'CMD' and 'DOCTYPE'. If the parameter 'CMD' is not specified then the document type only
 determines what is executed on the server. The document type of the content is defined 
 with the parameter 'DOCTYPE'.
*/

try
{
	$cmd = NULL;			/* _REQUEST['CMD'] -> command to execute */
	$body = array();		/* request parameters without 'CMD' */

	foreach ($_REQUEST as $key => $value)
	{
		if ($key == "DOCTYPE")
		{
			$body[ 'doctype'] = $value;
		}
	}

	foreach ($_REQUEST as $key => $value)
	{
		if ($key == "CMD")
		{
			$cmd = $value;
		}
		else if ($key == "DOCTYPE")
		{
		}
		else
		{
			$body[ $key] = $value;
		}
	}

	$body = json_encode( $body);

	$conn = new Session( "127.0.0.1", 7661, NULL, "NONE");
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


