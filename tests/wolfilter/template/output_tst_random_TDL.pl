#!/usr/bin/perl
use strict;

# GLOBALS
my @subroutines = ();		# MAP subroutine index to subroutine name
my @subroutineArgs = ();		# MAP subroutine index to number of subroutine arguments
my @subroutineResults = ();	# MAP subroutine index to number of subroutine results
my @subroutineResultActions = ();	# MAP subroutine index to number of subroutine results
my %subroutineStart = ();		# MAP subroutine name to address (index) in @instructions
my @actions = ();			# MAP action index to (dbcall) name
my @actionArgs = ();		# MAP action index to number of action arguments
my @actionResults = ();		# MAP action index to number of action results
my @tags = ();			# MAP input tag index to name
my @tagElems = ();			# MAP input tag index to number of input elements
my @intos = ();			# MAP result index (INTO output) to name
my @vmInstructions = ();		# database instructions to execute:
#vmInstructions Element Format:
#	into index#(T|R)#tag/result index#(S|A)#subroutine/action index

my $testSize = $ARGV[0];
my $testSeed = $ARGV[1];

# INIT
srand($testSeed);

# COMMON
sub createName {
	my ($prefix,$cnt) = @_;
	my $suffix = "$cnt";
	if ($cnt < 10)
	{
		$suffix = "0$suffix";
	}
	return "$prefix$suffix";
}

# SUBROUTINE
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


# ACTION
sub createActionName {
	my $rt = createName( "dbcall_", $#actions + 1);
	push( @actions, $rt);
	push( @actionArgs, int( rand( 4)));
	push( @actionResults, int( rand( 4)));
	return $rt;
}

for (my $ii=0; $ii<100; ++$ii) {
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

# INPUT TAG
sub createTagName {
	my $rt = createName( "tag_", $#tags + 1);
	push( @tags, $rt);
	push( @tagElems, rand(4));
	return $rt;
}

for (my $ii=0; $ii<100; ++$ii)
{
	createTagName();
}

sub getTagIdx {
	my $idx = int( rand( 100));
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


# INTO
sub createIntoName {
	my $rt = createName( "out_", $#intos + 1);
	push( @intos, $rt);
	return $rt;
}

for (my $ii=0; $ii<100; ++$ii)
{
	createIntoName();
}

sub getIntoIdx {
	my $idx = int( rand( 100));
	return $idx;
}

sub getIntoName {
	my ($idx) = (@_);
	return $intos[ $idx];
}


# VIRTUAL MACHINE INSTRUCTIONS
sub startVirtualMachineSubroutine
{
	my $addr = $#vmInstructions +1;
	$subroutineStart{ $subroutines[ $#subroutines] } = $addr;
}

sub createVirtualMachineInstruction
{
	my ($intoIndex,$ForeachClass,$ForeachIndex,$InstructionClass,$InstructionIndex) = @_;
	push( @vmInstructions, "$intoIndex:$ForeachClass:$ForeachIndex:$InstructionClass:$InstructionIndex");
}



# SUBROUTINE DATA
my @instructionResults = ();
my @instructionResultColumns = ();
my @instructionResultNofRows = ();
my @instructionResultAction = ();

# INSTRUCTION RESULT
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

# INSTRUCTION/SUBROUTINE ARGUMENT
sub getArg {
	my ($fidx,$tidx) = (@_);
	my $rt = "";
	my $rnd = int( rand( 8));
	if ($#instructionResults < 0 || $rnd < 1) {
		$rt = '$(.)';
	} elsif ($rnd < 3 && $subroutineArgs[ $#subroutineArgs] > 0) {
		my $argno = int( rand( $subroutineArgs[ $#subroutineArgs])) + 1;
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
	my $vm_into_index = -1;
	my $vm_foreach_class = "";
	my $vm_foreach_index = 0;
	my $vm_instruction_class = "";
	my $vm_instruction_index = 0;
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
			$vm_foreach_class = "T";
			$vm_foreach_index = $foreach_tag_idx;
		} else {
			$rt = $rt . "FOREACH " . getResultName( $foreach_result_idx) . " ";
			$vm_foreach_class = "R";
			$vm_foreach_index = $foreach_result_idx;
		}
	}
	if (int(rand(5)) > 1) {
		my $idx = getIntoIdx();
		$rt = $rt . "INTO " . getIntoName($idx) . " ";
		$vm_into_index = $idx;
	}
	my $nofRows = 0;
	if ($#subroutines >= 0 && int(rand(5)) > 3) {
		my $idx = getSubroutineIdx();
		$rt = $rt . "DO " . getSubroutineName( $idx) . "(" . getArgs($foreach_result_idx,$foreach_tag_idx) . ")";
		$subroutineResults[ $#subroutineResults] = getSubroutineResults( $idx);
		$vm_instruction_class = "S";
		$vm_instruction_index = $idx;
	}
	else
	{
		my $idx = getActionIdx();
		$rt = $rt . "DO " . getActionName($idx) . "(" . getArgs($foreach_result_idx,$foreach_tag_idx) . ")";
		$nofRows = getActionResults( $idx);
		if ($#subroutineResults >= 0) {
			$subroutineResults[ $#subroutineResults] = $nofRows;
		}
		$vm_instruction_class = "A";
		$vm_instruction_index = $idx;
	}
	$rt = $rt . ";\n";

	createVirtualMachineInstruction( $vm_into_index, $vm_foreach_class, $vm_foreach_index, $vm_instruction_class, $vm_instruction_index);

	if (int(rand(5)) > 2)
	{
		$rt = $rt . "KEEP AS " . createResultName( $nofRows) . ";\n";
	}
	return $rt;
}

sub createSubroutine {
	my $rt = "SUBROUTINE " . createSubroutineName();
	startVirtualMachineSubroutine();

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
print "COMMAND(run) SKIP CALL(test_transaction);\n";

print "**config\n";
my $dir_testmod = "./../wolfilter/modules/";
print "--input-filter token --output-filter token ";
print "--module $dir_testmod/filter/testtoken/mod_filter_testtoken ";
print "--module ./../../src/modules/cmdbind/directmap/mod_command_directmap ";
print "--module $dir_testmod/database/testtrace/mod_db_testtrace ";
print "--database 'identifier=testdb,outfile=DBOUT,file=DBRES' ";
print "--program=DBIN.tdl --cmdprogram=test.dmap run\n";

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
print "**outputfile:DBOUT\n";
print "**output\n";
print "**end\n";
