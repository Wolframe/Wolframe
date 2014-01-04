#!/usr/bin/perl

# GLOBALS
@subroutines = ();
@subroutineArgs = ();
@subroutineResults = ();
@actions = ();
@actionArgs = ();
@actionResults = ();
@tags = ();
@tagElems = ();
@intos = ();

$testSize = $ARGV[0];
$testSeed = $ARGV[1];

# INIT
srand($testSeed);


sub createName {
	my ($prefix,$cnt) = @_;
	my $suffix = "$cnt";
	if ($cnt < 10)
	{
		$suffix = "0$suffix";
	}
	return "$prefix$suffix";
}

sub createSubroutineName {
	my $rt = createName( "proc_", $#subroutines + 1);
	push( @subroutines, $rt);
	push( @subroutineArgs, int( rand( 4)));
	push( @subroutineResults, 0);
	return $rt;
}

sub getSubroutineIdx {
	return int( rand( $#subroutines + 1));
}

sub getSubroutineName {
	my ($idx) = (@_);
	return $subroutines[ $idx];
}



sub createActionName {
	my $rt = createName( "dbcall_", $#actions + 1);
	push( @actions, $rt);
	push( @actionArgs, int( rand( 4)));
	push( @actionResults, int( rand( 4)));
	return $rt;
}

for ($ii=0; $ii<100; ++$ii) {
	createActionName();
}

sub getActionIdx {
	return int( rand( 100));
}

sub getActionName {
	my ($idx) = (@_);
	return $actions[ $idx];
}

sub getActionResults {
	my ($idx) = (@_);
	return $actionResults[ $idx];
}


sub createTagName {
	my $rt = createName( "tag_", $#tags + 1);
	push( @tags, $rt);
	push( @tagElems, rand(4));
	return $rt;
}

for ($ii=0; $ii<100; ++$ii)
{
	createTagName();
}

sub getTagIdx {
	$idx = int( rand( 100));
	return $idx;
}

sub getTagName {
	my ($idx) = (@_);
	return $tags[ $idx];
}

sub getTagNofElems {
	my ($idx) = (@_);
	return $tagElems[ $idx];
}

sub getTagElem {
	my ($idx,$no) = (@_);
	return "X$idx" . "X$no" . "X";
}


sub createIntoName {
	my $rt = createName( "out_", $#intos + 1);
	push( @intos, $rt);
	return $rt;
}

for ($ii=0; $ii<100; ++$ii)
{
	createIntoName();
}

sub getIntoIdx {
	$idx = int( rand( 100));
	return $idx;
}

sub getIntoName {
	my ($idx) = (@_);
	return $intos[ $idx];
}


@instructionResults = ();
@instructionResultColumns = ();
@instructionResultNofRows = ();

sub createResultName {
	my ($nofRows) = (@_);
	my $rt = createName( "res_", $#instructionResults + 1);
	push( @instructionResults, $rt);
	push( @instructionResultColumns, int( rand( 4)));
	push( @instructionResultNofRows, $nofRows);
	return $rt;
}

sub getResultIdx {
	my $idx = int( rand($#instructionResults+1));
	return $idx;
}

sub getLastResultIdx {
	return $#instructionResults;
}

sub getResultName {
	my ($idx) = (@_);
	if ($idx == $#instructionResults)
	{
		return "RESULT";
	} else {
		return $instructionResults[ $idx];
	}
}

sub getResultColumn {
	my ($idx) = (@_);
	return int( rand( $instructionResultColumns[ $idx])) + 1;
}

sub getResultNofRows {
	my ($idx) = (@_);
	return $instructionResultNofRows[ $idx];
}

sub getArg {
	my ($fidx,$tidx) = (@_);
	my $rt = "";
	my $rnd = int( rand( 8));
	if ($#instructionResults < 0 || $rnd < 1) {
		$rt = '$(.)';
	} elsif ($rnd < 3 && $subroutineArgs[ $#subroutineArgs] > 0) {
		$argno = int( rand( $subroutineArgs[ $#subroutineArgs])) + 1;
		$rt = '$' . "PARAM." . $argno;
	} else {
		if (getLastResultIdx() < 0 || $rnd < 6) {
			my $idx = getResultIdx();
			if ($tidx >= 0 && ($tagElems[$tidx] > 1 || getResultNofRows($idx) > 1)) {
				$rt = '$(.)';
			} else {
				$rt = '$' . getResultName($idx) . "." . getResultColumn($idx);
			}
		} elsif ($fidx >= 0) {
			if ($tidx >= 0 && ($tagElems[$tidx] > 1 || getResultNofRows($fidx) > 1)) {
				$rt = '$(.)';
			} else {
				$rt = '$' . getResultColumn($fidx);
			}
		} else {
			my $idx = getLastResultIdx();
			if ($tidx >= 0 && ($tagElems[$tidx] > 1 || getResultNofRows($idx) > 1)) {
				$rt = '$(.)';
			} else {
				$rt = '$' . getResultColumn($idx);
			}
		}
	}
}

sub getArgs {
	my ($fidx,$tidx) = (@_);
	my $nn = int( rand(4) );
	my $ii;
	my $rt = "";
	for ($ii=0; $ii<$nn; $ii++) {
		if ($ii > 0) {
			$rt = $rt . ",";
		}
		$rt = $rt . getArg($fidx,$tidx);
	}
	return $rt;
}

sub getSubroutineResults {
	my ($idx) = (@_);
	return $subroutineResults[ $idx];
}

sub createInstruction {
	my $rt = "";
	my $foreach_result_idx = getResultIdx();
	my $foreach_tag_idx = -1;
	if ($instructionResultColumns[ $foreach_result_idx] <= 0 || int(rand(10)) > 5)
	{
		$foreach_result_idx = -1;
	}
	if ($foreach_result_idx <= 0)
	{
		$foreach_tag_idx = getTagIdx();
	}
	if (int(rand(10)) > 1) {
		if ($foreach_tag_idx >= 0) {
			$rt = $rt . "FOREACH " . getTagName( $foreach_tag_idx) . " ";
		} else {
			$rt = $rt . "FOREACH " . getResultName( $foreach_result_idx) . " ";
		}
	}
	if (int(rand(5)) > 1) {
		my $idx = getIntoIdx();
		$rt = $rt . "INTO " . getIntoName($idx) . " ";
	}
	my $nofRows = 0;
	if ($#subroutines >= 0 && int(rand(5)) > 3) {
		my $idx = getSubroutineIdx();
		$rt = $rt . "DO " . getSubroutineName( $idx) . "(" . getArgs($foreach_result_idx,$foreach_tag_idx) . ")";
		$subroutineResults[ $#subroutineResults] = getSubroutineResults( $idx);
	}
	else
	{
		my $idx = getActionIdx();
		$rt = $rt . "DO " . getActionName($idx) . "(" . getArgs($foreach_result_idx,$foreach_tag_idx) . ")";
		$nofRows = getActionResults( $idx);
		if ($#subroutineResults >= 0) {
			$subroutineResults[ $#subroutineResults] = $nofRows;
		}
	}
	$rt = $rt . ";\n";
	if (int(rand(5)) > 2)
	{
		$rt = $rt . "KEEP AS " . createResultName( $nofRows) . ";\n";
	}
	return $rt;
}

sub createSubroutine {
	my $rt = "SUBROUTINE " . createSubroutineName();
	if ($subroutineArgs[$#subroutineArgs] > 0)
	{
		$rt = $rt . "(";
		my $kk;
		for ($kk=1; $kk<=$subroutineArgs[$#subroutineArgs]; $kk++)
		{
			if ($kk > 1)
			{
				$rt = $rt . ",";
			}
			$rt = $rt . "ARG" . $kk;
		}
		$rt = $rt . ")";
	}
	$rt = $rt . "\n";
	$rt = $rt . "BEGIN\n";
	@instructionResults = ();
	@instructionResultColumns = ();
	my $nn = int(rand(12))+1;
	my $ii;
	for ($ii=0; $ii<$nn; ++$ii) {
		$rt = $rt . createInstruction();
	}
	$rt = $rt . "END\n";
	return $rt;
}

sub createTransaction
{
	my $rt = "TRANSACTION test_transaction\n"; 
	$rt = $rt . "BEGIN\n";
	@instructionResults = ();
	@instructionResultColumns = ();
	my $nn = int(rand(12))+1;
	my $ii;
	for ($ii=0; $ii<$nn; ++$ii) {
		$rt = $rt . createInstruction();
	}
	$rt = $rt . "END\n";
	return $rt;
}

my $ii;
print "**\n";
print "**file:DBRES\n";
print "**file:DBIN.tdl\n";
for ($ii=0; $ii<$testSize; ++$ii) {
	print (createSubroutine() . "\n\n");
}
print (createTransaction() . "\n\n");

print "**file: test.dmap\n";
print "COMMAND(run) CALL(test_transaction);\n";

print "**config\n";
my $dir_testmod = "./../wolfilter/modules/";
print "--input-filter token --output-filter token ";
print "--module $dir_testmod/filter/testtoken/mod_filter_testtoken ";
print "--module ./../../src/modules/cmdbind/directmap/mod_command_directmap ";
print "--module $dir_testmod/database/testtrace/mod_db_testtrace ";
print "--database 'identifier=testdb,outfile=DBOUT,file=DBRES' ";
print "--program=DBIN.tdl --cmdprogram=test.dmap run";

print "**input\n";
print ">doc\n";
for ($ii=0; $ii<=$#tags; ++$ii) {
	my $nn = getTagNofElems( $ii);
	my $kk;
	for ($kk=0; $kk<=$nn; ++$kk) {
		print ">" . getTagName( $ii) . "\n";
		print "=" . getTagElem( $ii,$kk) . "\n";
		print "<" . getTagName( $ii) . "\n";
	}
}
print "<doc\n";
print "**output\n";
print "**end\n";
