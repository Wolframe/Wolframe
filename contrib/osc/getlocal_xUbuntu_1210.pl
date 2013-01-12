#!/usr/bin/perl -w

use XML::DOM;

my $ARCH = "amd64";
#my $ARCH = "i586";
 
my $parser = new XML::DOM::Parser;
my $doc = $parser->parsefile ("file.xml");

foreach my $p ( $doc->getElementsByTagName( 'bdep' ) ) {
  $package = $p->getAttribute( 'name' );
  $version = $p->getAttribute( 'version' );
  $release = $p->getAttribute( 'release' );
  $arch = $p->getAttribute( 'arch' );
  $n = "${package}_$version-${release}_$arch.deb";
  
  $rpm = "/var/tmp/osbuild-packagecache/Ubuntu\:12.10/standard/$ARCH/$n";
  
  if( ! -f $rpm ) {
#    print "$rpm not found\n";
    open POSSIBLES, "ls /var/tmp/osbuild-packagecache/Ubuntu\:12.10/standard/$ARCH/$package*_$arch.deb |";
    $nof = 0;
    foreach $possible (<POSSIBLES>) {
      if( $possible =~ m@^/var/tmp/osbuild-packagecache/Ubuntu\:12.10/standard/$ARCH/(.+)_([^\-]+)-([^\-]+)_([^\.]+)\.deb$@ ) {
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
      } elsif( $possible =~ m@^/var/tmp/osbuild-packagecache/Ubuntu\:12.10/standard/$ARCH/(.+)_([^\-]+)_([^\.]+)\.deb$@ ) {
        $new_package = $1;
        $new_version = $2;
        $new_release = "none";
        $new_arch = $3;
        
        next if( $new_package ne $package );
        next if( $new_arch ne $arch );
        print "\tversion: $version => $new_version" if( $version ne $new_version );
#        print "\trelease: $release => $new_release" if( $release ne $new_release );
        
        $p->setAttribute( 'version', $new_version );
#        $p->setAttribute( 'release', $new_release );

        print "\n";
        $nof++;
        if( $nof > 1 ) {
           print STDERR "ERROR: more than one package found for $n\n";
           exit 1;
        }
      } else  {
        print "UNKOWN FILE: $possible\n";
        exit 1;
      }
    } 
    if( $nof == 0 ) {
      $url_package = $package;
      if( $package =~ /^lib/ ) {
        $firstletter = substr( $package, 0, 4 );
      } elsif( $package =~ /^([^-]*)\-(.*)/ ) {
        $url_package = $1;
        $firstletter = substr( $package, 0, 1 );
      } else {
        $firstletter = substr( $package, 0, 1 );
      }
      
      print STDERR "INFO: no package found for $n, downloading..\n";
      `cd /var/tmp/osbuild-packagecache/Ubuntu\:12.10/standard/$ARCH && apt-get download $package`;
      if( $? != 0 ) {
        print STDERR "\nERROR: APT failed!!\n\n";
        exit 1;
      }
    }
  }
}

$doc->printToFile( "new.xml" );
