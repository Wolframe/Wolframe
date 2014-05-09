<?php
require 'session.php';
use Wolframe\Session as Session;

/*
 Forwards HTTP GET request as XML to a wolframe server via TCP/IP plain and rendes the result
 as XML with a reference to a CSS associated to the document type identifier of the result
 to render it properly by the HTML client.
 The HTTP REQUEST parameters without 'CMD' are transformed into 
 an XML request set sent to the server. The command prefix is specified with the parameter
 'CMD' and 'DOCTYPE'. If the parameter 'CMD' is not specified then the document type only
 determines what is executed on the server. The document type of the content is defined 
 with the parameter 'DOCTYPE'.
*/

try
{
	$cmd = NULL;			/* _REQUEST['CMD'] -> command to execute */
	$content = array();		/* request parameters without 'CMD' */
	$doctype = NULL;
	$root = NULL;

	foreach ($_REQUEST as $key => $value)
	{
		if ($key == "DOCTYPE")
		{
			list($doctype, $root) = preg_split( "/[\s,;:]+/", $value, 2);
			if ($root == NULL)
			{
				$root = $doctype;
			}
		}
	}

	$body = '<?xml version="1.0" encoding="UTF-8" standalone="no"?>' . "\n";
	$body .= '<!DOCTYPE ' . $root . " SYSTEM '$doctype.sfrm'>\n";
	$body .= "<$root>";

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
			$body .= "<$key>" . $value . "</$key>";
		}
	}
	$body .= "</$root>\n";

	$conn = new Session( "127.0.0.1", 7661, NULL, "NONE");
	if (($result = $conn->request( $cmd, $body)) === FALSE)
	{
		echo "<html><head><title>FAILED</title></head><body>" . $conn->lasterror() . "</body></html>";
	}
	else
	{
		$random = uniqid( md5( rand( ) ) );
		$resnsp = str_replace( "\n", '', $result);
		preg_match('.*(\S+)', $resnsp, $matches);
		$rr = explode( '?>', $result, 2);
		$output = '<?xml version="1.0" encoding="UTF-8" standalone="no"?>'
			. "\n"
			. '<?xml-stylesheet type="text/css" href="css/'
			. var_dump( $matches)
			. ".css?random=$random\"?>"
			. "\n"
			. $rr[1];
		echo $output;
	}
	unset( $conn);
}
catch ( \Exception $e)
{
	echo "<html><head><title>ERROR</title></head><body>" . $e->getMessage() . "</body></html>";
}
?>


