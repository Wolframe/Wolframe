<?php 

require 'config.php';

function detect_browser( )
{
	$result = '';

	// TODO: don't do this all the time. maybe store it in the session
	include( 'browser_detection.inc' );
	if( $browser = browser_detection( 'full' ) ) {
		$result = $browser[0] . (int)$browser[1];
	}
	
	return $result;
}

$clientSideXSLT = false;
$urlRewrite = false;

try
{
// find out whether to use client side XSLT or not
	if( array_key_exists( "clientSideXSLT", $_GET ) ) {
		$clientSideXSLT = $_GET["clientSideXSLT"];
	} else {
		$browser = detect_browser( );
		
		$clientSideXSLT = false;
		if( in_array( $browser, $BROWSERS_USING_CLIENT_XSLT ) ) {
			$clientSideXSLT = true;
		}
	}

// find out whether Apache/mod_rewrite is around and we should beautify
// URLs
	if( $_SERVER['HTTP_MOD_REWRITE'] == 'On' ) {
		$urlRewrite = true;
	}

// set correct header, whether we return XML and the browser loads XSLT
// and XML to create XHTML or whether we return XHTML directy after processing
// it on the server
	if( $clientSideXSLT ) {
		header( "Content-Type: text/xml" );
	} else {
		header( "Content-Type: text/html" );
	}

// parse URL parameters, TODO: map with XSLT back to requests
	$uri = $_SERVER["PATH_INFO"];
	$file = "docs/" . $uri;
	
	$doc = file_get_contents( $file );
	
	if( $doc == FALSE )
	{
		throw new Exception( "File '" . $uri . "' not found" );
	}
	else
	{
		$dirs = explode( "/", $file );
		$parts = explode( ".", end($dirs) );
		$xsltFile = $parts[0] . '.xslt';
		
		$xmlDoc = transformData( $doc );
		
		echo render( $xmlDoc, $xsltFile );
	}
}
catch ( \Exception $e)
{
	$xmlOrig = "<error>" . $e->getMessage( ) . "</error>";
	$xmlOrig .= "<trace>" . $e->getTraceAsString( ) . "</trace>";
	
	$xmlDoc = transformData( $xmlOrig );

	render( $xmlDoc, "error.xslt" );
}

// transform the XML from the wolframe daemon and add Php layer stuff
// here:
// - variables
// - state
// - the page frame
function transformData( $xmlOrig )
{
	global $urlRewrite;
		
	$dom = new DOMDocument( );
	$root = $dom->createElement( 'page' );

	// TODO: find a dynamic way to do this (PHP_SELF? careful with security!!)
	$base = '/develop/builds/';
	$self = $base;
	if( !$urlRewrite ) {
		$self .= 'index.php/';
	}
	$root->setAttribute( 'base', $base );
	$root->setAttribute( 'self', $self );
	$dom->appendChild( $root );                
	$f = $dom->createDocumentFragment( );
	$f->appendXML( $xmlOrig );
	$root->appendChild( $f );
		
	$xml = $dom->saveXML( $dom->documentElement );
	
	return $xml;
}

// apply the output stylesheet, either by referencing it for client-side XSLT
// or by mapping it directly in the Php layer on the server
function render( $xmlDoc, $xsltFile )
{
	global $clientSideXSLT;
		
	if( $clientSideXSLT ) {
		$random = uniqid( md5( rand( ) ) );
		$output = '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
			. "\n"
			. '<?xml-stylesheet type="text/xsl" href="/develop/builds/xslt/'
			. $xsltFile
			. "?random=$random\"?>\n"
			. $xmlDoc;
		echo $output;
	} else {
		$xslt = new DomDocument;
		$xslt->load( 'xslt/' . $xsltFile );
		$xml = new DomDocument;
		$xml->loadXml( $xmlDoc );
		$proc = new XsltProcessor( );
		$proc->importStylesheet( $xslt );
		$output = $proc->transformToXML( $xml );
		echo $output;
		unset( $proc );
	}
}


