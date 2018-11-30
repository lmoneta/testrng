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
   static double Rndm() {
      return fgEngine->Rndm(); 
   }
   static REngine * fgEngine; 
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
   static double Rndm() {
      return fgEngine->Rndm(); 
   }
   static REngine * fgEngine; 
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
   static double Rndm() {
      return fgEngine->Rndm(); 
   }
   static REngine * fgEngine; 
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
   static double Rndm() {
      return fgEngine->Rndm(); 
   }
   static REngine * fgEngine; 
};

// initialization of the static members
template <typename REngine>
REngine * TRng<REngine>::fgEngine = nullptr;
TRandom1 * TRng<TRandom1>::fgEngine = nullptr;
ROOT::Math::RanLuxSEngine * TRng<ROOT::Math::RanLuxSEngine>::fgEngine = nullptr;
ROOT::Math::RanLuxDEngine * TRng<ROOT::Math::RanLuxDEngine>::fgEngine = nullptr;

#endif
