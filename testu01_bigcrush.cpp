#include <TRandom.h>
#include <TRandom1.h>
#include <TRandom2.h>
#include <TRandom3.h>
#include <TRandomGen.h>
#include "Math/MixMaxEngine.h"
#include "Math/MersenneTwisterEngine.h"
#include <TMath.h>
#include "Math/RanLuxEngine.h"

#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>

// wrapper class defining free functions which can be
// used by testu01
#include "TRng.h"

extern "C" {

#include "gdef.h"
#include "unif01.h"
#include "bbattery.h"

// specific tests
#include "snpair.h"
   
}


using namespace std;

//#define Nstreams 1

bool only_small_crush = false;
unsigned int seed_value = 0;
int test_number = -1;
bool run_snpair = false; 

template<class REngine>
void TestRng_BigCrush(const char * name="Generic", int luxlevel = -1) {

   cout<<"******************************************"<<endl;
   cout<<"Test for " << name << " generator ..."<<endl;
   cout<<"   using seed = "  << seed_value <<endl;
   cout<<"******************************************"<<endl;


   if (seed_value == 0) {
      // generate a random seed using TRandom123
      TRandom3 r(0);
      // use 2^31-1 for ranlux
      unsigned long MaxInt = (1UL << 31)-1;
      seed_value = r.Integer(MaxInt);
      std::cout << "Actual seed value used is " << seed_value << std::endl;
   }

   
   TRng <REngine>::SetEngine (seed_value, luxlevel);

   unif01_Gen *ugen = unif01_CreateExternGen01 ((char *) name, TRng<REngine>::Rndm); 
   //unif01_TimerGenWr(ugen,nevt,true);


   // run specific tests given by name
   if (run_snpair) {

      // define parameters for snpair test
      snpair_Res *  sres = snpair_CreateRes ();

      // apply the test
      long N =  10;
      long n = 1000000;
      int r =  0;
      int t = 10;
      int p = 0;
      int m = 40;//  Torus =  TRUE
         
      snpair_ClosePairs (ugen, sres, N, n, r, t, p, m);
      //snpair_WriteResultCP(ugen,sres, N, n, r, t, p, m, true);

      snpair_ClosePairsBitMatch (ugen, sres, N, n, r, t);
      
      snpair_DeleteRes (sres); 
      unif01_DeleteExternGen01 (ugen);
      return;
   }

   // run a single test
   if (test_number > 0) {
      std::vector<int> vtest_numbers(110);
      vtest_numbers[test_number]=1;
      bbattery_RepeatBigCrush (ugen, vtest_numbers.data());      
   }
   // run all tests 
   else { 
      bbattery_SmallCrush (ugen);
      if (!only_small_crush)  bbattery_BigCrush (ugen);

   }

   unif01_DeleteExternGen01 (ugen);

}

void run_test(int itype, const char * rng_name) { 

   std::cout <<  "Test generator " << rng_name << std::endl;

   // test TRandom (LCG)
   switch (itype) {
   case 0:
      TestRng_BigCrush<TRandom>("TRandom (LCG)");
      break;
   case 1:
      TestRng_BigCrush<TRandom1>("TRandom1 (RanLux)");
      break;
   case 2: 
      TestRng_BigCrush<TRandom2>("TRandom2 (Tausworth)");
      break;
   case 3: 
      TestRng_BigCrush<TRandom3>("TRandom3 (Mersenne-Twister)");
      break;
   case 4: 
      //TestRng_BigCrush<TRandomMixMax>("TRandomMixMax (MixMax240)");
      TestRng_BigCrush<ROOT::Math::MixMaxEngine240>("MixMax 240");
      break;
   case 5: 
      TestRng_BigCrush<ROOT::Math::MixMaxEngine17>("MixMax 17");
      break;
   case 6: 
      TestRng_BigCrush<ROOT::Math::MixMaxEngine256>("MixMax 256");
      break;
   case 7: 
      TestRng_BigCrush<ROOT::Math::StdEngine<std::mt19937_64>>("Mersenne-Twister 64 from std");
      break;
   case 8: 
      TestRng_BigCrush<ROOT::Math::StdEngine<std::ranlux48>>("Ranlux48 from std");
      break;
   case 9: 
      TestRng_BigCrush<ROOT::Math::RanLuxSEngine>("New Ranlux24 version (Lx=1)",1);
      break;
   case 10:
      TestRng_BigCrush<ROOT::Math::RanLuxDEngine>("New Ranlux48 version (Lx=1)",1);
      break;
   case 11: 
      TestRng_BigCrush<ROOT::Math::RanLuxSEngine>("New Ranlux24 version (Lx=0)",0);
      break;
   case 12: 
      TestRng_BigCrush<ROOT::Math::RanLuxSEngine>("New Ranlux24 version (Lx=2)",2);
      break;
   case 13:
      TestRng_BigCrush<ROOT::Math::RanLuxDEngine>("New Ranlux48 version (Lx=2)",2);
      break;
   case 14:
      TestRng_BigCrush<ROOT::Math::MixMaxEngine<8,0>>("MiMaxEngine 8 skip 0");
      break;      
   case 15:
      TestRng_BigCrush<ROOT::Math::MixMaxEngine<10,0>>("MiMaxEngine 10 skip 0");
      break;      
   default:
      TestRng_BigCrush<TRandomMixMax>("TRandomMixMax (MixMax240)");
      break;
   }

}

int main(int argc, char **argv)
{
   std::vector<TString> genNames = {"TRANDOM0","TRANDOM1","TRANDOM2","TRANDOM3",
                                    "MIXMAX","MIXMAX17","MIXMAX256","MT19937","RANLUX48",
                                    "RANLUXS","RANLUXD","RANLUXS0","RANLUXS2","RANLUXD2",
                                    "MIXMAX8","MIXMAX10"};
   int itype = -1;
   bool run_all = false; 
   // Parse command line arguments
   TString arg; 
   for (int i=1 ;  i<argc ; i++) {
      arg = argv[i] ;
      arg.ToUpper();
      if (arg.Contains("-SMALL")) {
         only_small_crush = true;
         continue;
      }
      if (arg.Contains("-ALL")) {
         run_all=true;
         continue;
      }
      if (arg.Contains("-SEED")) {
         TString seed_str = arg(arg.First("=")+1,arg.Length());
         int seed = seed_str.Atoi();
         seed_value = seed;
      }
      if (arg.Contains("-TEST")) {
         TString number_str = arg(arg.First("=")+1,arg.Length());
         int number = number_str.Atoi();
         test_number = number;
      }
      if (arg.Contains("-NAME")) {  // run dedicated test specified by the name
         TString testName = arg(arg.First("=")+1,arg.Length());
         testName.ToLower();
         if (testName.Contains("snpair")) run_snpair = true; 
      }
      for (size_t i = 0; i < genNames.size(); ++i) {
         if (arg.Contains(genNames[i]))  itype = i;
         if (arg.Contains("LCG"))  itype = 0;      
      }
   }

   if (!run_all) run_test(itype, genNames[itype]);
   else {
      for (size_t i = 0; i < genNames.size(); ++i) {
         run_test(i,genNames[i]);
      }
   }

   return 0;

}
