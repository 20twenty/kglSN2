#!/usr/bin/perl

$directory = shift @ARGV;
$pre = shift @ARGV;
$post = shift @ARGV;
opendir(DIR, $directory);
while($myfile = readdir(DIR)) {
   if($myfile =~ /^oob\w+$pre(\d+)$post\.csv$/) {  
      if($1 > 0) {
         $oobmap{$myfile} = $1;
      }
   }
   if($myfile =~ /^tst\w+$pre(\d+)$post\.csv$/) {  
      if($1 > 0) {
         $tstmap{$myfile} = $1;
      }
   }
}

@attr = ();
$col = 0;
foreach $f (sort {$oobmap{$a} <=> $oobmap{$b}} (keys %oobmap)) {
   print $f . "\n";
   open(INF,"<$directory/$f") or die "Can't open $directory/$f";
   $row = 0;
   while(<INF>) {
      if($_ =~ /^(\d),(\S+)$/) {
         $target[$row] = $1;
         $attr[$row][$col] = $2;
         $row++;
      } else {
         die "Bad format at row $row in file $f";
      }
   }
   close(INF);
   $col++;
}

open(OUTF,">$directory/step2_train.csv") or die "Can't open $directory/step2_train.csv";
print OUTF "Activity";
for ($c=0; $c < $col; $c++) {
   $cc = $c+1;
   print OUTF ",D$cc";
}
print OUTF "\n";
for ($r=0; $r < $row; $r++) {
   print OUTF $target[$r];
   for ($c=0; $c < $col; $c++) {
      print OUTF ",$attr[$r][$c]"
   }
   print OUTF "\n";
}
close(OUTF);

@attr = ();
$col = 0;
foreach $f (sort {$tstmap{$a} <=> $tstmap{$b}} (keys %tstmap)) {
   print $f . "\n";
   open(INF,"<$directory/$f") or die "Can't open $directory/$f";
   $row = 0;
   while(<INF>) {
      if($_ =~ /^(\S+)$/) {
         $attr[$row][$col] = $1;
         $row++;
      } else {
         die "Bad format at row $row in file $f";
      }
   }
   close(INF);
   $col++;
}

open(OUTF,">$directory/step2_test.csv") or die "Can't open $directory/step2_test.csv";
for ($c=0; $c < $col; $c++) {
   if($c != 0) { print OUTF ","; }
   $cc = $c+1;
   print OUTF "D$cc";
}
print OUTF "\n";
for ($r=0; $r < $row; $r++) {
   for ($c=0; $c < $col; $c++) {
      if($c != 0) { print OUTF ","; }
      print OUTF "$attr[$r][$c]"
   }
   print OUTF "\n";
}
close(OUTF);

