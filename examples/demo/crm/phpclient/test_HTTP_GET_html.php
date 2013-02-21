<?php 
require 'session.php';
use Wolframe\Session as Session;

try
{
	$conn = new Session( "127.0.0.1", 7661, "NONE");
	$doctype = $_GET["doctype"];
	if ($doctype == "") $doctype = "ListEmployee";
	if ($doctype == "ListEmployee") $query = '<?xml version="1.0" encoding="UTF-8" standalone="no"?><!DOCTYPE "employee" SYSTEM "ListEmployee.simpleform"><employee></employee>';
	else if ($doctype == "ListCustomer") $query = '<?xml version="1.0" encoding="UTF-8" standalone="no"?><!DOCTYPE "customer" SYSTEM "ListCustomer.simpleform"><customer></customer>';
	else if ($doctype == "ListProject") $query = '<?xml version="1.0" encoding="UTF-8" standalone="no"?><!DOCTYPE "project" SYSTEM "ListProject.simpleform"><project></project>';
	else throw new Exception( "unknown document type '" . $doctype . "'");

	if (($result = $conn->request( $query)) === FALSE)
	{
		throw new Exception( $conn->lasterror());
	}
	else
	{
		echo "<html><head><title>" . $doctype . "</title></head><body>" . htmlspecialchars($result) . "</body></html>";
	}
	unset( $conn);
}
catch ( \Exception $e)
{
	echo "<html><head><title>ERROR</title></head><body>" . $e->getMessage() . "</body></html>";
}
?>

