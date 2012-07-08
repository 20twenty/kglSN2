#!/usr/bin/perl

print " ---  $ARGV[0] ---- \n";

$fP_oob=shift @ARGV;
$fP_tst=shift @ARGV;
open(oobIN,"<$fP_oob") or die "Can't open oob predictions file $fP_oob";
open(oob_eIN,"<workspace/step1_train_edges.csv") or die "Can't open step1_train_edges.csv file";
open(tstIN,"<$fP_tst") or die "Can't open test predictions file $fP_tst";
open(tst_eIN,"<workspace/step1_test_edges.csv") or die "Can't open step1_test_edges.csv file";
open(vIN,"<workspace/step1_validate.csv") or die "Can't open validation file";
open(testIN,"<../data/test.csv") or die "Can't open ../data/test.csv file";
open(SUBV,">workspace/validate_submit.csv") or die "Can't open workspace/validate_submit.csv";
open(SUB,">workspace/submit.csv") or die "Can't open workspace/submit.csv";

<oob_eIN>; #edges file has header, but others do not;
while($_ = <oobIN>) {
   chomp($_);
   my @p_linesplit=split(/,/,$_);
   if($_ = <oob_eIN>) {
      chomp($_);
      my @e_linesplit=split(/,/,$_);
      $oob_predict{$e_linesplit[0]}{$e_linesplit[1]} = $p_linesplit[1];
   } else {
      die "oob edges file too short!";
   }
}
if(<oob_eIN>) {die "prediction file is too short: $_"};

<tst_eIN>; #edges file has header, but others do not;
while($_ = <tstIN>) {
   chomp($_);
   my $p = $_;
   if($_ = <tst_eIN>) {
      chomp($_);
      my @e_linesplit=split(/,/,$_);
      $tst_predict{$e_linesplit[0]}{$e_linesplit[1]} = $p;
   } else {
      die "tst edges file too short!";
   }
}
if(<tst_eIN>) {die "prediction file is too short: $_"};

while($_ = <vIN>) {
   chomp($_);
   @v_linesplit=split(/,/,$_);
   $source = shift @v_linesplit;
   while($dest = shift @v_linesplit) {
      $validate{$source}{$dest}=1;
   }
}

<testIN>; #test.csv has a header
print SUB "source_node,destination_nodes\n";
while($_ = <testIN>) {
   chomp($_);
   $source = $_+1-1;
   print SUB "$source";
   if(defined $tst_predict{$source}) {
      print SUB ",";
      @dests = keys %{$tst_predict{$source}};
      @dests = sort {$tst_predict{$source}{$b} <=> $tst_predict{$source}{$a}} @dests;
      $pos=0;
      foreach $dest (@dests) {
         if($source == $dest) {
            print "match at pos: $pos\n";
         } else {
            $pos++;
            if ($pos == 10) {last;}
            if($pos > 1) {print SUB " ";}
            print SUB "$dest";
         }
      }
   }
   print SUB "\n";
}
close(SUB);

%predict = %oob_predict;
@psources = keys %predict;
@vsources = keys %validate;

if ( $#psources != $#vsources ) {print "predict " . $#psources . " and validate " . $#vsources . " have different counts" . "\n";}

$totMap10 = 0;
$totCnt = 0;
print SUBV "source_node,destination_nodes\n";
foreach $source (@vsources) {
   $Map10=0;
   @totDestsA = keys %{$validate{$source}};
   $totDests = $#totDestsA;
   $totDests++;
   print SUBV "$source";
   if(defined $predict{$source}) {
      print SUBV ",";
      @dests = keys %{$predict{$source}};
      @dests = sort {$predict{$source}{$b} <=> $predict{$source}{$a}} @dests;
      $hit=0;
      $cnt=0;
      $pos=0;
      foreach $dest (@dests) {
         if($source == $dest) {
         } else {
            $pos++;
            if(defined $validate{$source}{$dest}) {
               $hit++;
               $Map10+=$hit/$pos;
            }
            if($pos==10) {last;}
            if($pos > 1) {print SUBV " ";}
            print SUBV "$dest";
         }
      }
      $Map10 = $Map10 / $totDests;
   }
   $totMap10+=$Map10;
   $totCnt++;
   #print "id: " . $source . "; Map10: " . $Map10 . "; Avg Map10: " . $totMap10/$totCnt . "\n";
   print SUBV "\n";
}
print "Avg Map10: " . $totMap10/$totCnt . "\n";


close(oobIN);
close(oob_eIN);
close(tstIN);
close(tst_eIN);
close(vIN);
close(testIN);
close(SUBV);
