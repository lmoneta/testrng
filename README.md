# Tests for ROOT Random Numbers Generators

This repository provides test programs for testing ROOT based Pseudo-random number generators using the **TestU01** suite 
from  P. L'Ecuyer and R. Simard (see http://simul.iro.umontreal.ca/testu01/tu01.html )

It provides a utility macro to build the test program installing and building first the testU01 package and then 
it compiles the test program. 
The provided test programs are 

-  `testu01_time.cpp` : this program measures the time spent by each generatir

-  `testu01_bigcrush` : this program performs the big crush test Suite for a given specific generator. 
    Valid Parameters are: 
  * *small* : runs only the SmallCrush instead of BigCrush
  * **generator_name** (e.g. *TRandom3*, *MixMax17*, etc..) will run the test for the provided generator. By default it runs for MixMax240 


To build the tests a special ROOT macro, `Build.C` is provided that does the following: 
 - check first if you have a version of testu01 already installed
 - if it is not installed, it download testu01 and it installed in the local directory
 - it compiles then the test program against Testu01 and ROOT 
 
 In order to run the `Build.C` macro and compile the program is needed to have a versionof ROOT installed. 
 The command to run the macro is: 
 
 ``root -b -q Build.C``
 
