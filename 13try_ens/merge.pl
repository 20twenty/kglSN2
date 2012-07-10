#!/usr/bin/perl

for ($m=1; $m < 4; $m++) {
   $oobmap{"workspace_m${m}/oob_predict_merge.csv"} = $m;
   $tstmap{"workspace_m${m}/tst_predict_merge.csv"} = $m;
}

@attr = ();
$col = 0;
foreach $f (sort {$oobmap{$a} <=> $oobmap{$b}} (keys %oobmap)) {
   print $f . "\n";
   open(INF,"<$f") or die "Can't open $f";
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

open(OUTF,">workspace/step2_train.csv") or die "Can't open workspace/step2_train.csv";
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
   open(INF,"<$f") or die "Can't open $f";
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

open(OUTF,">workspace/step2_test.csv") or die "Can't open workspace/step2_test.csv";
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

