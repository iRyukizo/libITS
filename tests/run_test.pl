#! /usr/bin/perl

print "Running test : $ARGV[0] \n";
open IN, "< $ARGV[0]";


my $title = <IN>;
my $statespace = ($title =~ /StateSpace /);
chomp $title;
my $call = <IN>;
chomp $call;

my $header;
my @nominal ;
my $exactstatcounter = 0;

while (my $line = <IN>) {
  if ($line =~ /Model ,\|S\| /) {
      $header = $line;
      $line = <IN> or die "Unexpected end of file after stats readout";
      chomp $line;
      @nominal = split (/\,/,$line);
      if (!$statespace) {
          last;
      }
  }
  if ($line =~ /Exact state count :/) {
      ($exactstatcounter) = ($line =~ /Exact state count : (\d+)/);
      last;
  }
}

close IN;

# Now run the tool# 

# print "syscalling : $call \n";
print "##teamcity[testStarted name='$title']\n";

my @tested;
my $testexactstatcounter = 0;

my @outputs = ();
my @results = `$call`;

while (my $line = shift(@results)) {
#  print "read : $line";
  push (@outputs,$line);
  if ($line =~ /Model ,\|S\| /) {
    $line = shift(@results) or die "Unexpected end of file after stats readout";
    chomp $line;
    @tested = split (/\,/,$line);
    if (!$statespace) {
        last;
    }
  }
  if ($line =~ /Exact state count :/) {
    ($testexactstatcounter) = ($line =~ /Exact state count : (\d+)/);
    last;
  }
}

# retrieve the exit value of the test
my $failure;
if ($? == 0) {
  $failure = 0;
} else {
  $failure = 1;
}

my $returnvalue;

if ( @nominal[1] != @tested[1] ) {
  print "@outputs\n";
  print "\n##teamcity[testFailed name='$title' message='regression detected' details='' expected='@nominal[1]' actual='@tested[1]'] \n";
  print "Expected :  @nominal[1]  Obtained :  @tested[1] \n";
  $returnvalue = 1;
} elsif ( $exactstatcounter != $testexactstatcounter ) {
  print "@outputs\n";
  print "\n##teamcity[testFailed name='$title' message='regression detected' details='Exact state count' expected='$exactstatcounter' actual='$testexactstatcounter'] \n";
  print "Expected : $exactstatcounter Obtained : $testexactstatcounter \n";
} elsif ( $failure ) {
  print "@outputs\n";
  print "\n##teamcity[testFailed name='$title' message='test did not exit properly' details='' expected='@nominal[1]' actual='@tested[1]'] \n";
  print "Expected :  @nominal[1]  Obtained :  @tested[1] \n"; 
  my $exitval = $? >> 8;
  print "test exited with value $exitval\n";
  $returnvalue = 99;
} else {
  print "##teamcity[buildStatisticValue key='testDuration' value='@tested[2]']\n";
  print "##teamcity[buildStatisticValue key='testMemory' value='@tested[3]']\n";
  print "Test successful : $title \n";
  print "Control Values/Obtained : \n$title\n@nominal\n@tested\n";
  $returnvalue = 0;
}

# print STDERR "Test name/Model, property, tool/technique, Time, Memory, Error\n";
open OUT2,">>test.csv";
print OUT2 "\"$title\",\"StateSpace\",\"its-reach\",@tested[2],@tested[3],$returnvalue\n";
close OUT2;    

print "##teamcity[testFinished name='$title']\n";

exit $returnvalue;
