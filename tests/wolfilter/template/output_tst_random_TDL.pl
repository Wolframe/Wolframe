#!/usr/bin/perl
use strict;

# GLOBALS
my @subroutines = ();			# MAP subroutine index to subroutine name
my @subroutineArgs = ();		# MAP subroutine index to number of subroutine arguments
my @subroutineResult = ();		# MAP subroutine index to number of subroutine results
my @subroutineStart = ();		# MAP subroutine index to address (index) in @instructions
my @actions = ();			# MAP action index to (dbcall) name
my @actionArgs = ();			# MAP action index to number of action arguments
my @actionResultColumns = ();		# MAP action index to number of action result columns
my @actionResultRows = ();		# MAP action index to number of action result rows
my @tags = ();				# MAP input tag index to name
my @tagElems = ();			# MAP input tag index to number of input elements
my @intos = ();				# MAP result index (INTO output) to name
my @vmInstructions = ();		# database instructions to execute:
my $vmStartAddress = -1;		# start address of the VM code to execute to print simulation of result
#vmInstructions Element Format:
#	into index#(T|R)#tag/result index#(S|A|R)#subroutine/action index
#		     ^ T=tag,R=Result
#					     ^ S=subroutine,A=Action,R=Return
my $testSize = $ARGV[0];
my $testSeed = $ARGV[1];
my $testInputSize = $testSize;
if ($#ARGV >= 2)
{
	$testInputSize = $ARGV[2]+1;
}
my $intosSize = 100;
my $actionsSize = 100;

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
	push( @subroutineResult, 0);
	push( @subroutineStart, $#vmInstructions +1);
	return $rt;
}

sub getRandSubroutineIdx {
	return int( rand( $#subroutines));
}

sub getSubroutineName {
	my ($idx) = (@_);
	return $subroutines[ $idx];
}

sub getSubroutineNofArgs {
	my ($idx) = (@_);
	return $subroutineArgs[ $idx];
}



# ACTION
sub createActionName {
	my $rt = createName( "dbcall_", $#actions + 1);
	push( @actions, $rt);
	push( @actionArgs, int( rand( 4)));
	push( @actionResultRows, int( rand( 4)));
	push( @actionResultColumns, int( rand( 4)));
	return $rt;
}

for (my $ii=0; $ii<$actionsSize; ++$ii) {
	createActionName();
}

sub getRandActionIdx {
	return int( rand( $actionsSize));
}

sub getActionName {
	my ($idx) = (@_);
	return $actions[ $idx];
}

sub getActionNofArgs {
	my ($idx) = (@_);
	return $actionArgs[ $idx];
}

sub getActionNofResultRows {
	my ($idx) = (@_);
	return $actionResultRows[ $idx];
}

sub getActionNofResultColumns {
	my ($idx) = (@_);
	return $actionResultColumns[ $idx];
}

sub getRandActionResultColumn {
	my ($idx) = (@_);
	return int( rand( $actionResultColumns[ $idx]));
}


# INPUT TAG
sub createTagName {
	my $rt = createName( "tag_", $#tags + 1);
	push( @tags, $rt);
	push( @tagElems, int( rand(5)));
	return $rt;
}

for (my $ii=0; $ii<$testInputSize; ++$ii)
{
	createTagName();
}

sub getRandTagIdx {
	my $idx = int( rand( $testInputSize));
	return $idx;
}

sub getTagName {
	my ($idx) = (@_);
	return $tags[ $idx];
}

sub getTagNofElems {
	my ($idx) = (@_);
	return 0 if ($idx < 0);
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

for (my $ii=0; $ii<$intosSize; ++$ii)
{
	createIntoName();
}

sub getRandIntoIdx {
	my $idx = int( rand( $intosSize));
	return $idx;
}

sub getIntoName {
	my ($idx) = (@_);
	return $intos[ $idx];
}


# VIRTUAL MACHINE INSTRUCTIONS
sub createVirtualMachineInstruction
{
	my ($intoIndex,$ForeachClass,$ForeachIndex,$InstructionClass,$InstructionIndex,$vm_encparam) = @_;
	push( @vmInstructions, "$intoIndex:$ForeachClass:$ForeachIndex:$InstructionClass:$InstructionIndex:$vm_encparam");
}

sub createVirtualMachineOpReturn
{
	push( @vmInstructions, ":::R:");
}

sub setVirtualMachineExecutionStartAddress
{
	$vmStartAddress = $#vmInstructions+1;
}

my @transactionInput = ();	# List of transaction input (part of test output)
sub executeVirtualMachine
{
	my @stack = ();
	my $ip = $vmStartAddress;

	while ($ip <= $#vmInstructions)
	{
		my $instr = $vmInstructions[ $ip];
		my ($intoI,$ForeachC,$ForeachI,$InstC,$InstI,$tinput) = split(/:/, $instr);

#DEBUG: print "STACK " . $#stack . " IP " . $ip . " INST " . $InstC . " " . $InstI .  "\n";
		if ($InstC eq "R")
		{
			# ... RETURN
			$ip = pop @stack;

		} elsif ($InstC eq "D") {
			# ... DBCALL
			if ($ForeachC eq '' || ($ForeachC eq 'T' && getTagNofElems( $ForeachI) > 0))
			{
				if ($tinput ne "")
				{
					my $ti;
					foreach $ti (split(/\n/, $tinput))
					{
						push @transactionInput, getActionName( $InstI) . " " . $ti;
					}
				}
				else
				{
					push @transactionInput, getActionName( $InstI);
				}
			}
			++$ip;

		} elsif ($InstC eq "S") {
			# ... SUBCALL
			++$ip;
			push @stack, $ip;
#DEBUG: print "CALL " . getSubroutineName( $InstI);
			$ip = $subroutineStart[ $InstI];
		} else {
			die "Illegal instruction in VM: $InstC ($instr)";
		}
	}
	my $line;
	foreach $line (@transactionInput)
	{
		print $line . "\n";
	}
}


# SUBROUTINE DATA
my @instructionResultName = ();
my @instructionResultAction = ();

# INSTRUCTION RESULT
sub createResultName {
	my ($nofRows) = (@_);
	my $rt = createName( "res_", $#instructionResultName + 1);
	push( @instructionResultName, $rt);
	push( @instructionResultAction, int( rand( $#actions+1)));
	return $rt;
}

sub getRandResultIdx {
	my $idx = int( rand($#instructionResultName+1));
	return $idx;
}

sub getLastResultIdx {
	return $#instructionResultName;
}

sub getResultName {
	my ($idx) = (@_);
	if ($idx > $#instructionResultName)
	{
		return "RESULT";
	} else {
		return $instructionResultName[ $idx];
	}
}

sub getResultNofColumns {
	my ($idx) = (@_);
	my $actionidx = $instructionResultAction[ $idx];
	return $actionResultColumns[ $idx];
}

sub getRandResultColumn {
	my ($idx) = (@_);
	my $actionidx = $instructionResultAction[ $idx];
	return int( rand( $actionResultColumns[ $idx]))+1;
}

sub getResultNofRows {
	my ($idx) = (@_);
	my $actionidx = $instructionResultAction[ $idx];
	return $actionResultRows[ $idx];
}


# INSTRUCTION/SUBROUTINE ARGUMENT
sub getRandArg {
	my ($fidx,$tidx) = (@_);
	my @rt = ('N',0,0);
	my $rnd = int( rand( 8));
	if ($#instructionResultName < 0 || $rnd < 1) {
		@rt = ('A',0,0);
	} elsif ($rnd < 3 && $subroutineArgs[ $#subroutineArgs] > 0) {
		my $argno = int( rand( $subroutineArgs[ $#subroutineArgs])) + 1;
		@rt = ('P',$argno,0);
	} else {
		if (getLastResultIdx() < 0 || $rnd < 6) {
			my $idx = getRandResultIdx();
			if ($tidx >= 0 && ($tagElems[$tidx] > 1 || getResultNofColumns($idx) >= 1)) {
				@rt = ('A',$tidx,0);
			} else {
				my $col = getRandResultColumn($idx);
				@rt = ('R',$idx,$col);
			}
		} elsif ($fidx >= 0) {
			if ($tidx >= 0 && ($tagElems[$tidx] > 1 || getResultNofColumns($fidx) >= 1)) {
				@rt = ('A',0,0);
			} else {
				my $col = getRandResultColumn($fidx);
				@rt = ('R',$fidx,$col);
			}
		} else {
			my $idx = getLastResultIdx();
			if ($tidx >= 0 && ($tagElems[$tidx] > 1 || getResultNofColumns($idx) >= 1)) {
				@rt = ('A',0,0);
			} else {
				my $col = getRandResultColumn($idx);
				@rt = ('R',-1,$col);
			}
		}
	}
	return @rt;
}

sub getRandArgs {
	my ($nofArgs,$fidx,$tidx) = (@_);
	my $ii;
	my $rt_param = '';
	my $rt_tinput = '';

	for ($ii=0; $ii<$nofArgs; $ii++) {
		$rt_tinput = $rt_tinput . '#';
		if ($ii > 0) {
			$rt_param = $rt_param . ",";
		}
		my ($type,$idx,$subidx) = getRandArg( $fidx, $tidx);
#DEBUG: print "ARG[" . $ii . "] ". $idx . " " . $subidx . " TYPE " . $type . "\n";
		if ($type eq 'A')
		{
			$rt_param = $rt_param . '$(.)';
			if ($tidx < 0)
			{
				$rt_tinput = $rt_tinput . "NULL";
			}
			else
			{
				$rt_tinput = $rt_tinput . getTagElem( $tidx, "_IDX_");
			}
		}
		elsif ($type eq 'P')
		{
			$rt_param = $rt_param . '$PARAM.' . $idx;
			$rt_tinput = '$[0' . $idx . ']';
		}
		elsif ($type eq 'R')
		{
			if ($idx >= 0)
			{
				$rt_param = $rt_param . '$' . getResultName($idx) . '.' . $subidx;
				$rt_tinput = '$[' . $idx . '.' . $subidx . ']';
			}
			else
			{
				$rt_param = $rt_param . '$RESULT.' . $subidx;
				$rt_tinput = '$[' . $subidx . ']';
			}
		}
		elsif ($type eq 'N')
		{
			$rt_param = $rt_param . 'NULL';
		}
		else
		{
			die "INTERNAL: Illegal state in random parameter generator (" . $type . ")";
		}
	}
	if ($tidx >= 0)
	{
		my $ti = 0;
		my $xx = $rt_tinput;
		$rt_tinput = "";
		my $te = getTagNofElems( $tidx);
		for (;$ti < $te; ++$ti)
		{
			my $subst = $xx;
			$subst =~ s/_IDX_/$ti/g;
			if ($ti > 0)
			{
				$rt_tinput = $rt_tinput . "\n";
			}
			$rt_tinput = $rt_tinput . $subst;
		}
	}
	return ($rt_param,$rt_tinput);
}

sub getSubroutineResult {
	my ($idx) = (@_);
	return $subroutineResult[ $idx];
}

sub createRandInstruction {
	my $vm_into_index = -1;
	my $vm_foreach_class = "";
	my $vm_foreach_index = 0;
	my $vm_instruction_class = "";
	my $vm_instruction_index = 0;
	my $vm_encparam = "";
	my $rt = "";
	my $foreach_result_idx = getRandResultIdx();
	my $foreach_tag_idx = -1;
	if (getResultNofColumns( $foreach_result_idx) <= 0 || int(rand(10)) > 5)
	{
		$foreach_result_idx = -1;
	}
	if ($foreach_result_idx <= 0 && int(rand(10)) > 2)
	{
		$foreach_tag_idx = getRandTagIdx();
	}
	if ($foreach_tag_idx >= 0) {
		$rt = $rt . "FOREACH " . getTagName( $foreach_tag_idx) . " ";
		$vm_foreach_class = "T";
		$vm_foreach_index = $foreach_tag_idx;
	} elsif ($foreach_result_idx >= 0) {
		$rt = $rt . "FOREACH " . getResultName( $foreach_result_idx) . " ";
		$vm_foreach_class = "R";
		$vm_foreach_index = $foreach_result_idx;
	}
	if (int(rand(5)) > 1) {
		my $idx = getRandIntoIdx();
		$rt = $rt . "INTO " . getIntoName($idx) . " ";
		$vm_into_index = $idx;
	}
	my $nofRows = 0;
	if ($#subroutines > 0 && int(rand(5)) > 3) {
		my $idx = getRandSubroutineIdx();
		my ($rt_param,$rt_tinput) = getRandArgs( getSubroutineNofArgs($idx),$foreach_result_idx,$foreach_tag_idx);
		$rt = $rt . "DO " . getSubroutineName( $idx) . "(" . $rt_param . ")";
		$subroutineResult[ $#subroutineResult] = getSubroutineResult( $idx);
		$vm_instruction_class = "S";
		$vm_instruction_index = $idx;
		$vm_encparam = $rt_tinput;
	}
	else
	{
		my $idx = getRandActionIdx();
		my ($rt_param,$rt_tinput) = getRandArgs( getActionNofArgs($idx),$foreach_result_idx,$foreach_tag_idx);
		$rt = $rt . "DO " . getActionName($idx) . "(" . $rt_param . ")";
		if ($#subroutineResult >= 0) {
			$subroutineResult[ $#subroutineResult] = $idx;
		}
		$vm_instruction_class = "D";
		$vm_instruction_index = $idx;
		$vm_encparam = $rt_tinput;
	}
#DEBUG:	print "INSTRUCTION " . $rt . "\n";
	$rt = $rt . ";\n";

	createVirtualMachineInstruction( $vm_into_index, $vm_foreach_class, $vm_foreach_index, $vm_instruction_class, $vm_instruction_index, $vm_encparam);

	if (int(rand(5)) > 2)
	{
		$rt = $rt . "KEEP AS " . createResultName( $nofRows) . ";\n";
	}
	return $rt;
}

sub createRandSubroutine {
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
	@instructionResultName = ();
	@instructionResultAction = ();
	my $nn = int(rand(12))+1;
	my $ii;
	for ($ii=0; $ii<$nn; ++$ii) {
		$rt = $rt . createRandInstruction();
	}
	$rt = $rt . "END\n";
	createVirtualMachineOpReturn();
	return $rt;
}

sub createRandTransaction
{
	my $rt = "TRANSACTION test_transaction\n"; 
	$rt = $rt . "BEGIN\n";
	@instructionResultName = ();
	@instructionResultAction = ();
	setVirtualMachineExecutionStartAddress();
	my $nn = int(rand(12))+1;
	my $ii;
	for ($ii=0; $ii<$nn; ++$ii) {
		$rt = $rt . createRandInstruction();
	}
	$rt = $rt . "END\n";
	return $rt;
}

my $ii;
print "**\n";
print "**file:DBRES\n";
print "**file:DBIN.tdl\n";
for ($ii=0; $ii<$testSize; ++$ii) {
	print (createRandSubroutine() . "\n\n");
}
print (createRandTransaction() . "\n\n");

print "**file: test.dmap\n";
print "COMMAND(run) SKIP CALL(test_transaction) RETURN STANDALONE doc;\n";

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
	for ($kk=0; $kk<$nn; ++$kk) {
		print ">" . getTagName( $ii) . "\n";
		print "=" . getTagElem( $ii,$kk) . "\n";
		print "<" . getTagName( $ii) . "\n";
	}
}
print "<doc\n";
print "**outputfile:DBOUT\n";
print "**output\n";
executeVirtualMachine();
print "**end\n";
