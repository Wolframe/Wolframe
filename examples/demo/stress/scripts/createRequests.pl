#!/usr/bin/perl

sub createDoc
{
	my ($doctype, $name, %map) = @_;
	open (MYFILE, '>request/' . "$name" . '.xml');

	if (($doctype) && ($doctype ne ""))
	{
		print MYFILE '<?xml version="1.0" encoding="UTF-8"?>' . "\n";
		print MYFILE '<!DOCTYPE doc SYSTEM "' . $doctype . '">';
	}
	else
	{
		print MYFILE '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>' . "\n";
	}
	print MYFILE '<doc>';
	foreach $key( keys %map)
	{
		my $k = $key;
		my $v = $map{$key};
		print MYFILE '<' . $k . '>' . $v . '</' . $k . '>';
	}
	print MYFILE '</doc>' . "\n";
	close MYFILE;
}

sub createInsertDocument
{
	my ($number) = @_;
	my $mul = $number * $number;
	my $dup = $number + $number;
	my $trip = $number * 3;
	my %doc = ( "numbr" => "$number", "mul" => "$mul", "dup" => "$dup", "trip" => "$trip" );
	createDoc( "insertNumber", "insert/$number", %doc );
}

sub createUpdateDocument
{
	my ($number) = @_;
	my $mul = $number * $number;
	my $dup = $number + $number;
	my $trip = $number * 3;
	my %doc = ( "numbr" => "$number", "mul" => "$mul", "dup" => "$dup", "trip" => "$trip" );
	createDoc( "updateNumber", "update/$number", %doc );
}

sub createSelectDocument
{
	my ($number) = @_;
	my %doc = ( "numbr" => "$number" );
	createDoc( "selectNumber", "select/$number", %doc );
}

sub createDeleteDocument
{
	my ($number) = @_;
	my %doc = ( "numbr" => "$number" );
	createDoc( "deleteNumber", "delete/$number", %doc );
}

for ($count = 1; $count <= 10000; $count++)
{
	createInsertDocument( $count);
	createUpdateDocument( $count);
	createSelectDocument( $count);
	createDeleteDocument( $count);
}

