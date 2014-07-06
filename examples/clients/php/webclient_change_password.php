<?php
header('Content-Type: text/xml');
require 'session.php';
use Wolframe\Session as Session;

try
{
	$oldpwd = NULL;			/* _REQUEST['OLDPWD'] -> old password */
	$newpwd = NULL;			/* _REQUEST['NEWPWD'] -> new password */
	$ssl_enabled = false;		/* SSL yes (true) / no (false) */
	$username = NULL;		/* Username for authorization with WOLFRAME-CRAM */

	foreach ($_REQUEST as $key => $value)
	{
		if ($key == "OLDPWD")
		{
			$oldpwd = $value;
		}
		else if ($key == "NEWPWD")
		{
			$newpwd = $value;
		}
		else if ($key == "SSL" && 0==strcmp($value,"yes"))
		{
			$ssl_enabled = true;
		}
		else if ($key == "USERNAME")
		{
			$username = $value;
		}
	}
	if ($ssl_enabled == true)
	{
		$sslpath = "./SSL";
		$sslopt = array(
			"local_cert" => "$sslpath/combinedcert.pem",
			"verify_peer" => false
		);
	}
	if ($oldpwd != NULL && $username != NULL)
	{
		$authopt = array(
			"mech" => "WOLFRAME-CRAM",
			"username" => $username,
			"password" => $oldpwd
		);
	}
	$port = 7661;
	if ($ssl_enabled)
	{
		$port = 7961;
	}
	$conn = new Session( "127.0.0.1", $port, $sslopt, $authopt);
	$conn->changePassword( $oldpwd, $newpwd);
	unset( $conn);
}
catch ( \Exception $e)
{
	echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	echo "<error><class>EXCEPTION</class><message>" . $e->getMessage() . "</message></error>";
}
?>


