#include <TRandom.h>
#include <TRandom1.h>
#include <TRandom2.h>
#include <TRandom3.h>
#include <TRandomGen.h>
#include "Math/MixMaxEngine.h"
#include "Math/MersenneTwisterEngine.h"
#include <TMath.h>
#include "Math/RanLuxEngine.h"
#include "TH1.h"
#include "TFile.h"

// for ranluxPP
#define USE_RANLUXPP
#include "Math/RanLuxPPEngine.h"


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
#include "scomp.h"
#include "smarsa.h"
#include "swalk.h"
#include "sknuth.h"
#include "smultin.h"
}


using namespace std;

//#define Nstreams 1

unsigned int seed_value = 0;
int test_number = -1;

// specific tests
bool run_snpair = false;
bool run_scomp = false;
bool run_nips = false; 
bool run_smarsa = false; 
bool run_swalk = false;
bool run_sknuth = false; 
bool run_smultin = false; 

bool run_rabbit = false;
bool run_small_crush = false;
bool run_medium_crush = false;

long nInput = -1;

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
#ifdef USE_TRNG2   // case of TRG2: split a value in two of ~31 bits 
   TRng2 <REngine>::SetEngine (seed_value, luxlevel);
   unif01_Gen *ugen = unif01_CreateExternGen01 ((char *) name, TRng2<REngine>::Rndm);
   std::cout << "Using split of generated number in 2 of 31 bits " << std::endl;
#else  
   TRng <REngine>::SetEngine (seed_value, luxlevel);
#ifdef SKIP_VALUES  // to skip values (i..e. skipping first 3 values of sequence as defined in TRng::Rndm3
   unif01_Gen *ugen = unif01_CreateExternGen01 ((char *) name, TRng<REngine>::Rndm3);
   std::cout << "Skip some values in generated sequence according to definition in Trng::Rndm3 " << std::endl;
#else
   // default case
   unif01_Gen *ugen = unif01_CreateExternGen01 ((char *) name, TRng<REngine>::Rndm);
   std::cout << "run generator in standard mode " << std::endl;
#endif
   //TRng <REngine>::SetMaxEvt (3);  // useonly one number in N-1
   
#endif
   
   //unif01_TimerGenWr(ugen,nevt,true);


   // test some generated numbers
   std::cout << "Print few generated numbers " << std::endl;
   for (int i = 0; i < 100; ++i) {
      double x = ugen->GetU01( ugen->param, ugen->state); 
      std::cout << " " << x  << " , ";
   }
   std::cout << std::endl;

   // run specific tests given by name
   if (run_snpair) {

      // define parameters for snpair test
      snpair_Res *  sres = snpair_CreateRes ();

      // apply the test
      long N =  5;
      long n = (nInput > 0) ? nInput : 2000000; // ma can be allocated 
      int r =  0;
      int t = 16;
      int p = 0;
      int m = 30;//  Torus =  TRUE
         
      snpair_ClosePairs (ugen, sres, N, n, r, t, p, m);
      //snpair_WriteResultCP(ugen,sres, N, n, r, t, p, m, true);

      // N = 2000000;
      // snpair_ClosePairsBitMatch (ugen, sres, N, n, r, t);
      
      snpair_DeleteRes (sres); 
      unif01_DeleteExternGen01 (ugen);
      return;
   }

   if (run_scomp) {

     // define parameters for snpair test
     scomp_Res *  sres = scomp_CreateRes ();
     
     // apply the test
     long N =  1;
     long n = (nInput > 0) ? nInput : 400020;
     int r =  0;
     int s = 1 ;
     
     scomp_LinearComp (ugen, sres, N, n, r, s);
     
     scomp_DeleteRes (sres); 
     unif01_DeleteExternGen01 (ugen);
     return;
   }
   if (run_smarsa) {
     // run smarsa birthday spacing test
          // apply the test
     long N =  1000;
     long n = (nInput > 0) ? nInput : 10000000;
     int r =  0;
     long d =  64;
     int t = 10;
     int p = 1;
     smarsa_Res *  sres = smarsa_CreateRes ();
     smarsa_BirthdaySpacings (ugen, sres->Pois, N, n, r, d, t, p);
     
     smarsa_DeleteRes (sres); 
     unif01_DeleteExternGen01 (ugen);
     return; 
   }
   if (run_swalk) {
     // run random walk test

     long N =  1;
     long n = (nInput > 0) ? nInput : 100000000;
     int r =  25;
     int s = 5;
     long L0 =  50;
     long L1 =  50;
     
     swalk_Res *  sres = swalk_CreateRes ();
     swalk_RandomWalk1 (ugen, sres, N, n, r, s, L0, L1);
     
     swalk_DeleteRes (sres); 
     unif01_DeleteExternGen01 (ugen);
     return; 
   }
   if (run_sknuth) {
     // run sknuth tests
     bool gap_test = true;
     bool serial_test = false;
     bool collision_test = false; 

     long N = 1;
     //long Nskip = 0;
     //long n = 100000000000;  // 10^11
     long n = (nInput > 0) ? nInput : 100000000;
     int r =  0;

     sknuth_Res1 *  sres = sknuth_CreateRes1 ();

     int gap_size = 4;
     if (gap_test) { 
       double alpha = 0;
       //double beta = 0.0625;          //1./16.
       double beta = 1./double(gap_size);          //1./16.
       //double beta = 0.00097656250;  // 1. / 2^10

       // when using a modified version use NSkip to skip numbers a
       //sknuth_Gap (ugen, sres->Chi, Nskip, n, r, alpha, beta);
       sknuth_Gap (ugen, sres->Chi, N, n, r, alpha, beta);
     }
     else if (serial_test) {
       long d = 100;
       long t = 3;

       sknuth_Serial (ugen, sres->Chi, N, n, r, d, t);
     }
     else if (collision_test) {
        sknuth_Res2 *  sres2 = sknuth_CreateRes2 ();
       long d = 2048;
       r = 0; 
       //n = 4.E8;
       N = 10;
       int t = 5;
       // this is exactly equivalente to smultin_Multinomial with sparse=true
       sknuth_Collision(ugen, sres2, N, n, r, d, t); 
       
       sknuth_DeleteRes2 (sres2);
     }
       
     // print frequency result
     if (gap_test) {
        int nb = sres->Chi->degFree+1; 
        auto h1 = new TH1D("hobs","Observed gap values",nb, 0, nb); 
        auto h0 = new TH1D("hexp","Expected gap values",nb, 0, nb); 
        std::cout << "Frequency result " << std::endl;
        for (int i = 0; i < nb; ++i) {
           std::cout << " ( " << sres->Chi->Count[i] << " , " << sres->Chi->NbExp[i] << " ) ,";
           h1->SetBinContent(i+1,sres->Chi->Count[i]);
           h0->SetBinContent(i+1,sres->Chi->NbExp[i]);
        }
        std::cout << std::endl;
        TString gname(name);
        gname.ReplaceAll(" ","");
        gname.ReplaceAll("Engine","");
        TString fileName = TString::Format("test_sknuth_gap%d_%s_%g.root",gap_size,gname.Data(),double(n));
        auto fout = TFile::Open(fileName,"RECREATE");
        h0->Write();
        h1->Write();
        fout->Close();
     }
     
     sknuth_DeleteRes1 (sres);
     unif01_DeleteExternGen01 (ugen);
     return; 
   }

   //multinomial test
   if (run_smultin) {
      bool sparse = true;
      smultin_Param * par = nullptr;
      smultin_Res * sres = smultin_CreateRes( par);
      long N = 10;
      long n = 4E7;
      long d = 8;
      int r = 0;
      int t = 16; 
      smultin_Multinomial(ugen, par, sres, N, n, r, d, t, sparse); 
      smultin_DeleteRes (sres);
      return;
   }

   // rabbit test 
   if (run_rabbit) {
     double nbits = TMath::Power(2,30);  // number of bit to use
     bbattery_Rabbit(ugen, nbits);
     unif01_DeleteExternGen01 (ugen);
     return;
   }
   if (run_nips) {
     bbattery_FIPS_140_2(ugen);
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
      if (run_small_crush) bbattery_SmallCrush (ugen);
      else if (run_medium_crush) bbattery_Crush (ugen);
      else  bbattery_BigCrush (ugen);

   }

   unif01_DeleteExternGen01 (ugen);

}

void run_test(int itype, const char * rng_name) { 

   std::cout <<  "Test generator " << rng_name << std::endl;

   // test TRandom (LCG)
   switch (itype) {

#ifndef USE_TRG2 // exclude when using the case of splitting 2 words in one of 32 bits 
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
#endif
   case 4: 
      //TestRng_BigCrush<TRandomMixMax>("TRandomMixMax (MixMax240)");
      TestRng_BigCrush<ROOT::Math::MixMaxEngine240>("MixMax 240");
      break;
   case 5: 
      TestRng_BigCrush<ROOT::Math::MixMaxEngine17>("MixMax 17");
      break;
   case 6: 
     TestRng_BigCrush<ROOT::Math::MixMaxEngine<256,2>>("MixMax 256-S2");
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
      TestRng_BigCrush<ROOT::Math::MixMaxEngine<8,0>>("MixMax 8 skip 0");
      break;      
   case 15:
      TestRng_BigCrush<ROOT::Math::MixMaxEngine<10,4>>("MixMax 10 skip 4");
      break;      
   case 16:
      TestRng_BigCrush<ROOT::Math::RanLuxPPEngine>("RanLux++ Engine");
      break;
   case 17:
      TestRng_BigCrush<ROOT::Math::MixMaxEngine44851>("MixMax 44851 Engine");
      break;
   case 18:
      TestRng_BigCrush<ROOT::Math::MixMaxEngine<256,0>>("MixMax 256-S0 Engine");
      break;
   case 19:
      TestRng_BigCrush<ROOT::Math::MixMaxEngine<16,0>>("MixMax 16 skip 0 Engine");
      break;
   case 20:
      TestRng_BigCrush<ROOT::Math::MixMaxEngine<44,0>>("MixMax 44 skip 0 Engine");
      break;
   case 21:
      TestRng_BigCrush<ROOT::Math::MixMaxEngine<88,0>>("MixMax 88 skip 0 Engine");
      break;
   case 22:
      TestRng_BigCrush<ROOT::Math::MixMaxEngine<10,14>>("MixMax 10 skip 14 Engine");
      break;
   case 23:
      TestRng_BigCrush<ROOT::Math::MixMaxEngine<16,11>>("MixMax 16 skip 11 Engine");
      break;
   case 24:
      TestRng_BigCrush<ROOT::Math::MixMaxEngine<44,8>>("MixMax 44 skip 8 Engine");
      break;
   case 25:
      TestRng_BigCrush<ROOT::Math::MixMaxEngine<88,6>>("MixMax 88 skip 6 Engine");
      break;
   case 26:
      TestRng_BigCrush<ROOT::Math::MixMaxEngine<256,5>>("MixMax 256 skip 5 Engine");
      break;
   case 27:
      TestRng_BigCrush<ROOT::Math::MixMaxEngine<1000,4>>("MixMax 1000 skip 4 Engine");
      break;
   case 28:
      TestRng_BigCrush<ROOT::Math::MixMaxEngine<10,0>>("MixMax 10 skip 0 Engine");
      break;
   case 29:
      TestRng_BigCrush<ROOT::Math::MixMaxEngine<10,1>>("MixMax 10 skip 1 Engine");
      break;
   case 30:
      TestRng_BigCrush<ROOT::Math::MixMaxEngine<10,2>>("MixMax 10 skip 2 Engine");
      break;
      
   default:
      TestRng_BigCrush<ROOT::Math::MixMaxEngine240>("MixMax 240 (default)");
      //TestRng_BigCrush<TRandomMixMax>("TRandomMixMax (MixMax240)");
      break;
   }

}

int main(int argc, char **argv)
{
   std::vector<TString> genNames = {"TRANDOM0","TRANDOM1","TRANDOM2","TRANDOM3",
                                    "MIXMAX","MIXMAX17","MIXMAX256","MT19937","RANLUX48",
                                    "RANLUXS","RANLUXD","RANLUXS0","RANLUXS2","RANLUXD2",
                                    "MIXMAX8","MIXMAX10","RANLUXPP","MIXMAX44851","MIXMAX256-0",
                                    "MIXMAX16-0","MIXMAX44-0","MIXMAX88-0",
                                    "MIXMAX10-14","MIXMAX16-11","MIXMAX44-8","MIXMAX88-6",
                                    "MIXMAX256-5","MIXMAX1000-4",
                                    "MIXMAX10-0","MIXMAX10-1","MIXMAX10-2"
                                    };

   int itype = -1;
   bool run_all = false; 
   // Parse command line arguments
   TString arg; 
   for (int i=1 ;  i<argc ; i++) {
      arg = argv[i] ;
      arg.ToUpper();
      if (arg.Contains("-SMALL")) {
         run_small_crush = true;
         continue;
      }
      if (arg.Contains("-MEDIUM")) {
         run_medium_crush = true;
         continue;
      }
      if (arg.Contains("-RABBIT")) {
         run_rabbit = true;
         continue;
      }
      if (arg.Contains("-NIPS")) {
         run_nips = true;
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
         if (testName.Contains("scomp")) run_scomp = true; 
         if (testName.Contains("smarsa")) run_smarsa = true; 
         if (testName.Contains("swalk")) run_swalk = true; 
         if (testName.Contains("sknuth")) run_sknuth = true; 
         if (testName.Contains("smultin")) run_smultin = true; 
      }
      if (arg.Contains("-N=")) {  // get number of n values to generate (to be used when a single test is run)
         TString n_str = arg(arg.First("=")+1,arg.Length());
	 nInput = std::stol( std::string(n_str) );
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
