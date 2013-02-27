#!/usr/bin/perl -w

use XML::DOM;

my $ARCH = "x86_64";
#my $ARCH = "i586";
 
my $parser = new XML::DOM::Parser;
my $doc = $parser->parsefile ("file.xml");

foreach my $p ( $doc->getElementsByTagName( 'bdep' ) ) {
  $package = $p->getAttribute( 'name' );
  $version = $p->getAttribute( 'version' );
  $release = $p->getAttribute( 'release' );
  $arch = $p->getAttribute( 'arch' );
  $n = "$package-$version-$release.$arch.rpm";
  
  $rpm = "/var/tmp/osbuild-packagecache/CentOS:CentOS-5/standard/$ARCH/$n";
  
  if( ! -f $rpm ) {
#    print "$rpm not found\n";
    open POSSIBLES, "ls /var/tmp/osbuild-packagecache/CentOS\:CentOS-5/standard/$ARCH/$package*.$arch.rpm |";
    $nof = 0;
    foreach $possible (<POSSIBLES>) {
      if( $possible =~ m@^/var/tmp/osbuild-packagecache/CentOS\:CentOS-5/standard/$ARCH/(.+)-([^\-]+)-([^\-]+)\.([^\.]+)\.rpm$@ ) {
        $new_package = $1;
        $new_version = $2;
        $new_release = $3;
        $new_arch = $4;
        
        next if( $new_package ne $package );
        next if( $new_arch ne $arch );
        print "\tversion: $version => $new_version" if( $version ne $new_version );
        print "\trelease: $release => $new_release" if( $release ne $new_release );
        
        $p->setAttribute( 'version', $new_version );
        $p->setAttribute( 'release', $new_release );

        print "\n";
        $nof++;
        if( $nof > 1 ) {
           print STDERR "ERROR: more than one package found for $n\n";
           exit 1;
        }
      } elsif( $possible =~ m@/var/tmp/osbuild-packagecache/CentOS\:CentOS-5/standard/$ARCH/([^\-]+)-((devel)|(libs))-([^\-]+)-([^\-]+)\.([^\.]+)\.rpm@ ) {
        print "PROBLEM HERE: $possible\n";
        exit 1;
      } else  {
        print "UNKOWN FILE: $possible\n";
        exit 1;
      }
    } 
    if( $nof == 0 ) {
      print STDERR "ERROR: no package found for $n\n";
      exit 1;
    }
  }
}

$doc->printToFile( "new.xml" );