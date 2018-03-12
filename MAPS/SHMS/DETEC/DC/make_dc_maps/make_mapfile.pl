#!/usr/bin/perl -w

use Data::Dumper;

my %cables;
my %tdc;
my $refchan=127;   # NOTE this assumption is a hack for the ESB setup (BDS)

## Return the first channel of a TDC connector (A0, A1, .. ,D1)
sub conn2ch($) {
  my $conn = shift;

  $conn =~ tr/ABCD/0123/;
  if($conn =~ /([0-3])([01])/) {
    return $1*32 + $2*16;
  } else {
    die "Invalid connector argument: $conn\n";
  }
}


while(<>) {
  next if(/^\s*$/);
  next if(/^#/);
  chomp;
  my @f=split;

  if($f[0] eq "tdc") {
    my $slot=$f[1];
    my $conn=$f[2];
    my $cable=$f[3];
    my @refs=();
    @refs = split(/[, ]+/, $f[4]) if($#f == 4);

    die("TDC dup:'$_'\n") if( exists($tdc{$slot}{$conn}) );

    $tdc{$slot}{$conn}{"cable"} = $cable;
    $tdc{$slot}{$conn}{"refs"}  = \@refs;

    $tdc{$slot}{"refs"} = [] if( !exists($tdc{$slot}{"refs"}) );
    my @chans = map( conn2ch($conn) + $_, @refs );
    push(@{$tdc{$slot}{"refs"}}, @chans);

    next;
  }

  if($f[0] eq "cable") {
    my $cable=$f[1];
    my $w_start=$f[2];
    my $w_end=$f[3];
    my @NC=();
    @NC = split(/[, ]+/, $f[4]) if($#f == 4);

    die("Cable dup:'$_'\n") if( exists($cables{$cable}{"w_start"}) );

    $cables{$cable}{"w_start"} = $w_start;
    $cables{$cable}{"w_end"} = $w_end;
    $cables{$cable}{"NC"} = \@NC;

    die("Invalid pin count '$_'\n") if( 16 != abs($w_end - $w_start) + 1 + $#NC + 1);

    next;
  }
}

my %wiremap;
my @planes = qw( U U' X X' V V' );

sub numerically { $a <=> $b };

foreach my $sl ( sort numerically keys %tdc ) {
  foreach my $conn (keys %{$tdc{$sl}}) {
    next if($conn eq "refs");

    my $conn2chn = conn2ch($conn);

    my $cab = $tdc{$sl}{$conn}{"cable"};
    next unless ( $cab =~ /(.)\d?c\d+(p)?/ );
    my $p = $1;
    $p .= "'" if(defined($2));

    my $w_st  = $cables{$cab}{"w_start"};
    my $w_end = $cables{$cab}{"w_end"};

    my @wires = $w_st < $w_end ?
              $cables{$cab}{"w_start"} .. $cables{$cab}{"w_end"} :
      reverse($cables{$cab}{"w_end"}   .. $cables{$cab}{"w_start"});

    for my $i (0 .. $#wires) {
      $wiremap{$p}[$wires[$i]] = [ $sl, $conn2chn + $i ];
    }
  }
}

sub print_wiremap {
  my %wiremap = %{$_[0]};
  print "====== WIREMAP ======\n";
  foreach my $p (sort keys %wiremap) {
    print "------- Plane $p -------\n";
    printf("%6s %6s %6s\n","wire","slot","chan");
    my @wires = @{$wiremap{$p}};
    foreach $i (0 .. $#wires) {
      printf("%6d %6d %6d\n", $i, @{$wires[$i]});
    }
  }
  print "\n";
}

sub print_replaymap {
  my %wiremap = %{$_[0]};
  print "====== WIREMAP ======\n";
  my $lastslot = -1;
  foreach my $p (sort keys %wiremap) {
      my $pnum=1;
      foreach $pname (@planes) {
          last if($pname eq $p);
          $pnum++;
      }
      my @wires = @{$wiremap{$p}};
      foreach $i (0 .. $#wires) {
          my $slot= ${$wires[$i]}[0];
          my $chan= ${$wires[$i]}[1];
          my $wirenum=$i+1;
          if ($slot != $lastslot) {
              print "SLOT=$slot,$refchan\n";
          }
          print "$chan,$pnum,$wirenum ! Plane $p, wire $wirenum\n";
          $lastslot=$slot;
      }
  }
  print "\n";
}

sub print_tdcs {
  my %tdc = %{$_[0]};
  print "====== TDCs ======\n";
  foreach my $sl (sort numerically keys %tdc) {
    print "TDC Slot $sl\n";
    if( ${@{$tdc{$sl}{"refs"}}} > 1 ) {
      print "\tRefs: " . join(", ", sort numerically @{$tdc{$sl}{"refs"}}) . "\n";
    }
    foreach my $conn (sort keys %{$tdc{$sl}}) {
      next if($conn eq "refs");
      printf("%4s  :  %6s", $conn, $tdc{$sl}{$conn}{cable});
      if( ${@{$tdc{$sl}{"refs"}}} > 1 ) {
        print "\t\tRef: " .  join(", ",  @{$tdc{$sl}{$conn}{"refs"}}) . "\n";
      } else {
        print "\n";
      }
    }
  }
  print "\n";
}

sub print_cables {
  my %cables = %{$_[0]};
  print "====== CABLES ======\n";
  printf("  %-6s %9s %9s    :   %s\n", "Cable", "wire_st", "wire_end", "Not Connected");
  foreach my $cab (sort keys %cables) {
    printf("  %-6s %9d %9d    :   %s\n", 
      $cab, $cables{$cab}{"w_start"}, $cables{$cab}{"w_end"},
      join(", ", @{$cables{$cab}{"NC"}}));
  }
  print "\n";
}


#print_tdcs(\%tdc);
#print_cables(\%cables);
print_replaymap(\%wiremap);

