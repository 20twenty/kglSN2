#include <iostream>
#include <string>
#include <unistd.h>
#include <ios>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include "xData.h"

using namespace std;

void process_mem_usage(double& vm_usage, double& resident_set)
{
   using std::ios_base;
   using std::ifstream;
   using std::string;

   vm_usage     = 0.0;
   resident_set = 0.0;

   // 'file' stat seems to give the most reliable results
   //
   ifstream stat_stream("/proc/self/stat",ios_base::in);

   // dummy vars for leading entries in stat that we don't care about
   //
   string pid, comm, state, ppid, pgrp, session, tty_nr;
   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   string utime, stime, cutime, cstime, priority, nice;
   string O, itrealvalue, starttime;

   // the two fields we want
   //
   unsigned long vsize;
   long rss;

   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
               >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
               >> utime >> stime >> cutime >> cstime >> priority >> nice
               >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

   stat_stream.close();

   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
   vm_usage     = vsize / 1024.0;
   resident_set = rss * page_size_kb;
}

int main(int argc, char** argv) {
   if(argc < 8 || argc > 9) {
      cerr << "args: <train_file(string)> <test_file(string)> <seed> <train_limit> <test_limit> <group> <groups> <cache_only>" << endl;
      return 1;
   }   
   bool cache_only = false;
   if(argc == 9) cache_only = (atoi(argv[8]) == 1) ? true : false;
   xData* pXD = new xData(argv[1],argv[2],atoi(argv[3]),atoi(argv[4]),atoi(argv[5]),atoi(argv[6]),atoi(argv[7]),cache_only);
   if (!pXD->isgood()) return 1;
   double vm, rss;
   process_mem_usage(vm, rss);
   cerr << "VM: " << vm << "; RSS: " << rss << endl;
   if(0) {
      cerr << "press any key and hit return... ";
      string tmp;
      cin >> tmp;
   }
   return 0;
}

