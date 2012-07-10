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

open(OUTF,">$directory/oob_predict_merge.csv") or die "Can't open $directory/oob_predict_merge.csv";
for ($r=0; $r < $row; $r++) {
   $sum=0;
   for ($c=0; $c < $col; $c++) {
      $sum+=$attr[$r][$c];
   }
   $avg = $sum / $col;
   print OUTF "$target[$r],$avg\n";
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

open(OUTF,">$directory/tst_predict_merge.csv") or die "Can't open $directory/tst_predict_merge.csv";
for ($r=0; $r < $row; $r++) {
   $sum=0;
   for ($c=0; $c < $col; $c++) {
      $sum += $attr[$r][$c];
   }
   $avg = $sum / $col;
   print OUTF "$avg\n";
}
close(OUTF);

