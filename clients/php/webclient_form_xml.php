<?php
header('Content-Type: text/xml');
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

	$conn = new Session( "127.0.0.1", 7661, NULL, NULL);
	if (($result = $conn->request( $cmd, $body)) === FALSE)
	{
		echo "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>";
		echo "<error><class>ANSWER</class><message>" . $conn->lasterror() . "</message></error>";
	}
	else
	{
		echo $result;
	}
	unset( $conn);
}
catch ( \Exception $e)
{
	echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	echo "<error><class>EXCEPTION</class><message>" . $e->getMessage() . "</message></error>";
}
?>


