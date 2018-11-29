#include <TRandom.h>
#include <TRandom1.h>
#include <TRandom2.h>
#include <TRandom3.h>
#include <TRandomGen.h>
#include "Math/MixMaxEngine.h"
#include "Math/MersenneTwisterEngine.h"
#include <TMath.h>
#include "TSysten.h"

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


template<class REngine>
void TestRng(long nevt, const char * name="Generic") {

   cout<<"******************************************"<<endl;
   cout<<"Test for " << name << " generator ..."<<endl;
   cout<<"******************************************"<<endl;

   
   TRng<REngine>::SetEngine ();

   unif01_Gen *ugen = unif01_CreateExternGen01 ((char *) name, TRng<REngine>::Rndm); 
   unif01_TimerGenWr(ugen,nevt,true); 

   unif01_DeleteExternGen01 (ugen);

}


int main(int argc, char **argv){


   long nevt = 1000000000;   // 10^7 evts

   TString arg; 
   for (int i=1 ;  i<argc ; i++) {
      arg = argv[i] ;
      if (arg.Contains("-nevt")) {
         TString nevt_str = arg(arg.First("=")+1,arg.Length());
         nevt = nevt_str.Atoi();
      }
   }

   printf(" Test generation time for %g events \n",double(nevt));

   // get system info
   SysInfo_t sys_info;
   gSystem->GetSysInfo(&sys_info);
   std::cout << " CPU type used for this test\n";
   std::cout << sys_info.fModel << " type " << sys_info.fCpuType << " @ " << sys_info.fCpuSpeed << std::endl;
   std::cout << "number of available cores = " << sys_info.fCpus << std::endl;

   // test TRandom (LCG)
   TestRng<TRandom>(nevt,"TRandom (LCG)");
   TestRng<TRandom3>(nevt,"TRandom3 (Mersenne-Twister)");
   TestRng<TRandom2>(nevt,"TRandom2 (Tausworth)");
   TestRng<TRandomMixMax>(nevt,"TRandomMixMax (MixMax240)");
   TestRng<ROOT::Math::MixMaxEngine240>(nevt,"MixMax 240");
   TestRng<ROOT::Math::MixMaxEngine17>(nevt,"MixMax 17");
   TestRng<ROOT::Math::MixMaxEngine256>(nevt,"MixMax 256");
   TestRng<ROOT::Math::StdEngine<std::mt19937_64>>(nevt,"Mersenne-Twister 64 from std");
   TestRng<TRandom1>(0.1*nevt,"TRandom1 (RanLux)");
   TestRng<ROOT::Math::StdEngine<std::ranlux48>>(0.1*nevt,"RanLux 48 from std");

   return 0;

}
