// wrapper class for ROOT Rng defining free functions
// that can be used by testu01


#ifndef ROOT_TestU01_TRNG
#define ROOT_TestU01_TRNG

template<class REngine>
struct TRng {

   ~TRng() {
      if (fgEngine) delete fgEngine;
      fgEngine = nullptr;
   }
   static void SetEngine(int seed = 0, int lux = 0) {
      if (fgEngine) delete fgEngine; 
      fgEngine = new REngine(); 
      fgEngine->SetSeed(seed);
   }
   static void SetMaxEvt(int n) {
      fgMaxEvt = n;
      fgCounter = 0;
   }
   static double Rndm() {
      return fgEngine->Rndm(); 
   }
   static double Rndm2() {
      if (fgCounter % fgMaxEvt == 0) fgEngine->SetSeed( fgCounter);
      fgCounter++;
      return fgEngine->Rndm(); 
   }
   static double Rndm3() {
      const int SIZE=8;
      double val =  fgEngine->Rndm();
      fgCounter++;      
//      while ( (fgCounter % fgMaxEvt) != 0) {
//      while ( (fgCounter <= 1 ) ) { // || (fgCounter >6 && fgCounter < 8) ) ) {
//      while ( ( fgCounter <= 3 ) || (fgCounter >= 6 && fgCounter <= 10  ) || (fgCounter >= 14 && fgCounter <= 16)) { // || (fgCounter >6 && fgCounter < 8) ) ) {
      while ( ( fgCounter <= 3 ) ) {
         val = fgEngine->Rndm();
         fgCounter++;
         if (fgCounter == 2*SIZE) fgCounter = 0; 
      }
      if (fgCounter == 2*SIZE) {
         fgCounter = 0;
      }
      return val; 
   }
   static REngine * fgEngine;
   static uint64_t fgCounter;
   static uint64_t fgMaxEvt;
};



// specialization for Ranlux for setting luxury level

#include "TRandom1.h"
#include "Math/RanLuxEngine.h"


template<>
struct TRng<TRandom1> {
   typedef TRandom1 REngine; 
   ~TRng() {
      if (fgEngine) delete fgEngine;
      fgEngine = nullptr;
   }
   static void SetEngine(int seed = 0, int lux = 3) {
      if (fgEngine) delete fgEngine;
      if (lux < 0) lux = 3;   // default value
      std::cout << "Create ranlux engine with luxury level " << lux << std::endl;
      fgEngine = new REngine(seed,lux); 
      fgEngine->SetSeed(seed);
   }
   static void SetMaxEvt(int n) {
      fgMaxEvt = n;
      fgCounter = 0;
   }
   static double Rndm() {
      return fgEngine->Rndm(); 
   }
   static double Rndm2() {
      if (fgCounter % fgMaxEvt == 0) fgEngine->SetSeed( fgCounter);
      fgCounter++;
      return fgEngine->Rndm(); 
   }
   static double Rndm3() { return 0; }

   static REngine * fgEngine; 
   static uint64_t fgCounter;
   static uint64_t fgMaxEvt;
};

template<>
struct TRng<ROOT::Math::RanLuxSEngine> {
   typedef ROOT::Math::RanLuxSEngine REngine; 

   ~TRng() {
      if (fgEngine) delete fgEngine;
      fgEngine = nullptr;
   }
   static void SetEngine(int seed = 0, int lux = 1) {
      if (fgEngine) delete fgEngine;
       if (lux < 0) lux = 1;   // default value
      std::cout << "Create ranlux engine with luxury level " << lux << std::endl;
      fgEngine = new REngine(seed,lux); 
      fgEngine->SetSeed(seed);
   }
   static void SetMaxEvt(int n) {
      fgMaxEvt = n;
      fgCounter = 0;
   }
   static double Rndm() {
      return fgEngine->Rndm(); 
   }
   static double Rndm2() {
      if (fgCounter % fgMaxEvt == 0) fgEngine->SetSeed( fgCounter);
      fgCounter++;
      return fgEngine->Rndm(); 
   }
   static double Rndm3() { return 0; }
   
   static REngine * fgEngine; 
   static uint64_t fgCounter;
   static uint64_t fgMaxEvt;
};

template<>
struct TRng<ROOT::Math::RanLuxDEngine> {

   typedef ROOT::Math::RanLuxDEngine REngine; 

   ~TRng() {
      if (fgEngine) delete fgEngine;
      fgEngine = nullptr;
   }
   static void SetEngine(int seed = 0, int lux = 1) {
      if (fgEngine) delete fgEngine;
       if (lux < 0) lux = 1;   // default value
      std::cout << "Create ranlux engine with luxury level " << lux << std::endl;
      fgEngine = new REngine(seed,lux); 
      fgEngine->SetSeed(seed);
   }
   static void SetMaxEvt(int n) {
      fgMaxEvt = n;
      fgCounter = 0;
   }
   static double Rndm() {
      return fgEngine->Rndm(); 
   }
   static double Rndm2() {
      if (fgCounter % fgMaxEvt == 0) fgEngine->SetSeed( fgCounter);
      fgCounter++;
      return fgEngine->Rndm(); 
   }
   static double Rndm3() { return 0; }

   static REngine * fgEngine; 
   static uint64_t fgCounter;
   static uint64_t fgMaxEvt;
};


template<class REngine>
struct TRng2 {

   ~TRng2() {
     if (fgEngine) delete fgEngine;
     fgEngine = nullptr;
   }
   static void SetEngine(int seed = 0, int lux = 0) {
      if (fgEngine) delete fgEngine; 
      fgEngine = new REngine(); 
      fgEngine->SetSeed(seed);
   }
   static double Rndm() {
     if (fgFirst) {
       static const uint64_t MASK30 = (1UL << 30) -1;
       static const uint64_t MASK31 = (1UL << 31) -1;
       static const uint64_t MASK32 = (1UL << 32) -1;
       static const double kConv32 =  2.3283064365386963e-10; // 2 ^ -32
       static const double kConv31 = 4.6566128730773926e-10;   // 1./ 2^31
       static const double kConv30 = 9.3132257461547852e-10;   // 1./2^30

       uint64_t val = fgEngine->IntRndm();

       if (fgCounter > 61) fgCounter = 0 ; 
       if (fgCounter == 0) 
	 fgExtraVal = fgEngine->IntRndm();

       uint64_t v1 = val & MASK31;
       uint64_t v2_tmp = (val >> 31) & MASK30; // this are the extra 30 random bits       
       // now I miss one bit - take from extra value 
       uint64_t mask = 1UL << fgCounter; 
       uint64_t v2 = (v2_tmp << 1) + ( ( fgExtraVal &  mask) >> fgCounter );
       fgCounter++; 

       static double kConv = kConv31;

       fgCacheVal = v2 * kConv; 
       fgFirst = false;
       return v1 * kConv;
     }
     fgFirst = true; 
     return fgCacheVal; 
   }
   static REngine * fgEngine; 
   static double fgCacheVal;
   static uint64_t fgExtraVal; // value used to generate extra bit
   static int   fgCounter; 
   static bool   fgFirst;
};


// initialization of the static members
template <typename REngine>
REngine * TRng<REngine>::fgEngine = nullptr;
TRandom1 * TRng<TRandom1>::fgEngine = nullptr;
ROOT::Math::RanLuxSEngine * TRng<ROOT::Math::RanLuxSEngine>::fgEngine = nullptr;
ROOT::Math::RanLuxDEngine * TRng<ROOT::Math::RanLuxDEngine>::fgEngine = nullptr;

// uint64_t TRng<REngine>::fgCounter = 0;
// TRandom1 * TRng<TRandom1>::fgEngine = nullptr;
template <typename REngine>
uint64_t TRng<REngine>::fgCounter = 0;
uint64_t TRng<TRandom1>::fgCounter = 0;
uint64_t TRng<ROOT::Math::RanLuxSEngine>::fgCounter = 0;
uint64_t TRng<ROOT::Math::RanLuxDEngine>::fgCounter = 0;


template <typename REngine>
uint64_t TRng<REngine>::fgMaxEvt = 100000000000000;
uint64_t TRng<TRandom1>::fgMaxEvt = 100000000000000;
uint64_t TRng<ROOT::Math::RanLuxSEngine>::fgMaxEvt = 100000000000000;
uint64_t TRng<ROOT::Math::RanLuxDEngine>::fgMaxEvt = 100000000000000;

template <typename REngine>
REngine * TRng2<REngine>::fgEngine = nullptr;
template <typename REngine>
bool TRng2<REngine>::fgFirst = true;
template <typename REngine>
double TRng2<REngine>::fgCacheVal = 0;
template <typename REngine>
int TRng2<REngine>::fgCounter = 0;
template <typename REngine>
uint64_t TRng2<REngine>::fgExtraVal = 0;


#endif
