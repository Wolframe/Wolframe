<?php
	require_once "WolframeClient.php";

	$connParams = array(
		"host" => "localhost",
		"port" => "7661"
	);
	$conn = new WolframeClient( $connParams );

	if( !$conn->connect( ) ) {
		print "Unable to connect to Wolframe server: " . $conn->getLastError( );
		return;
	}
	
	$conn->disconnect( );
	
	echo $_GET["username"];
