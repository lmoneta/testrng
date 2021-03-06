#include <TRandom.h>
#include <TRandom1.h>
#include <TRandom2.h>
#include <TRandom3.h>
#include <TRandomGen.h>
#include "Math/MixMaxEngine.h"
#include "Math/MersenneTwisterEngine.h"
#include <TMath.h>

#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>


extern "C" {
  #include "gdef.h"
  #include "unif01.h"
  #include "bbattery.h"
}

using namespace std;

#define Nstreams 1


// wrapper class for ROOT Rng
template<class REngine>
struct TRng {

   ~TRng() {
      if (fgEngine) delete fgEngine;
      fgEngine = nullptr;
   }
   static void SetEngine(int seed = 0) {
      if (fgEngine) delete fgEngine; 
      fgEngine = new REngine(); 
      fgEngine->SetSeed(seed);
   }
   static double Rndm() {
      return fgEngine->Rndm(); 
   }
   static REngine * fgEngine; 
};

template <typename REngine>
REngine * TRng<REngine>::fgEngine = nullptr;

// these are not needed
// TRng<TRandom>::fgEngine = nullptr;
// TRng<TRandom3>::fgEngine = nullptr;
// TRandom2 * tausw_eng = nullptr;
// TRandom3 * mstw_eng = nullptr;
// TRandom * generic_eng = nullptr;
// TRandomMixMax * mixmax_eng = nullptr; 
// ROOT::Math::MixMaxEngine17 * mixmax17_eng = nullptr;
// ROOT::Math::MixMaxEngine240 * mixmax240_eng = nullptr;
// ROOT::Math::MixMaxEngine256 * mixmax256_eng = nullptr;
// ROOT::Math::StdEngine<std::mt19937_64> * mt_std_eng = nullptr;
// ROOT::Math::StdEngine<std::ranlux48> * ranlux_std_eng = nullptr;

bool only_small_crush = false;
unsigned int seed_value = 0;

template<class REngine>
void TestRng_BigCrush(const char * name="Generic") {

   cout<<"******************************************"<<endl;
   cout<<"Test for " << name << " generator ..."<<endl;
   cout<<"   using seed = "  << seed_value <<endl;
   cout<<"******************************************"<<endl;


   if (seed_value == 0) {
      // generate a random seed using TRandom123
      TRandom3 r(0);
      unsigned long MaxInt = (1UL << 32)-1;
      seed_value = r.Integer(MaxInt);
      std::cout << "Actual seed value used is " << seed_value << std::endl;
   }

   
   TRng <REngine>::SetEngine (seed_value);

   unif01_Gen *ugen = unif01_CreateExternGen01 ((char *) name, TRng<REngine>::Rndm); 
   //unif01_TimerGenWr(ugen,nevt,true);

   bbattery_SmallCrush (ugen);
   if (!only_small_crush)  bbattery_BigCrush (ugen);


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
   default:
      TestRng_BigCrush<TRandomMixMax>("TRandomMixMax (MixMax240)");
      break;
   }

}

int main(int argc, char **argv)
{
   std::vector<TString> genNames = {"TRANDOM0","TRANDOM1","TRANDOM2","TRANDOM3",
                                    "MIXMAX","MIXMAX17","MIXMAX256","MT19937","RANLUX48"};
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
