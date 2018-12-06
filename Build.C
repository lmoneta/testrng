// macro to built the test suite for random number based on TestU01

void Build() {

   TString cmd;
   int iret = 0;

   int ncpu = 8; 
   
   // check if testu01 exists
   TString dirName = "testU01";
   TString fullDirName =  dirName + "/" + "TestU01-1.2.3";
   TString tarFileName = "TestU01.zip";
   TString urlName = "http://simul.iro.umontreal.ca/testu01/TestU01.zip";
   TString libPathName = dirName + "/lib";
   TString incPathName = dirName + "/include";
   if (gSystem->AccessPathName(libPathName) || gSystem->AccessPathName(incPathName) ) {
      // testu01 installation does not exist
      if (gSystem->AccessPathName(tarFileName) ) {
         // tar file not existing - download
         cmd = "curl " + urlName + " -o " + tarFileName;
         gSystem->Exec(cmd);
      }
      if (gSystem->AccessPathName(tarFileName) ) {
         Error("Build","Error downloading TestU01 - exit");
         return;
      }
      // tar file exist
      // untar and build
      cmd = "unzip " + tarFileName + " -d " + dirName; 
      gSystem->Exec(cmd);
      // build the package
      // configure and build
      Info("Build","Now in directory %s/TestU01-1.2.3",dirName.Data() );
      TString cmd1 = "cd " + fullDirName;
     //  gSystem->Exec(cmd);
     // gSystem->Exec("pwd");
      std::string wkdir = gSystem->GetWorkingDirectory();
      dirName = TString(wkdir.c_str()) + "/" + dirName;
      fullDirName = TString(wkdir.c_str()) + "/" + fullDirName;
      TString cmd2 = "./configure --prefix="+dirName;
      cmd = cmd1 + "; " + cmd2;
      //iret = gSystem->Exec(fullDirName + "/configure --prefix " + dirName);
      iret = gSystem->Exec(cmd);
      if (iret) {
         Error("Build","Error configuring  TestU01 - exit");
         return;         
      }
      Info("Build","Now compiling TestU01.... ");
      cmd = cmd1 + TString::Format("; make -j %d",ncpu);
      iret = gSystem->Exec(cmd);
      if (iret) {
         Error("Build","Error building  TestU01 - exit");
         return; 
      }
      Info("Build","Now installing TestU01... ");
      cmd = cmd1 + "; make install";
      iret = gSystem->Exec(cmd);
      if (iret) {
         Error("Build","Error installing  TestU01 - exit");
         return; 
      }

      
      
      // test again 
      if (gSystem->AccessPathName(libPathName) || gSystem->AccessPathName(incPathName) ) {
         Error("Build","Error installing TestU01 - exit");
         return; 
      }
   }

   // build now ROOT test program 
   TString testProgram = "testu01_time";
   TString testProgram_obj = testProgram + ".o";
   TString testProgram_src = testProgram + ".cpp";
   TString libDirName = dirName + "/lib";
   TString incDirName = dirName + "/include";

   cmd = TString::Format("g++ -O3 -c  -I%s `root-config --cflags` %s -o %s",
                         incDirName.Data(),testProgram_src.Data(),testProgram_obj.Data() );
   iret = gSystem->Exec(cmd);
   cmd = TString::Format("g++ -O3 %s `root-config --libs` -L%s -ltestu01 -lprobdist -lmylib -o %s",
                         testProgram_obj.Data(),libDirName.Data(),testProgram.Data() );
   
   iret |= gSystem->Exec(cmd);
   if (iret) {
      Error("Build","Error building %s",testProgram.Data());
      return;
   }

   testProgram = "testu01_bigcrush";
   testProgram_src = testProgram + ".cpp";
   testProgram_obj = testProgram + ".obj";

   cmd = TString::Format("g++ -O3 -c  -I%s `root-config --cflags` %s -o %s",
                         incDirName.Data(),testProgram_src.Data(),testProgram_obj.Data() );
   iret = gSystem->Exec(cmd);
   cmd = TString::Format("g++ -O3 %s `root-config --libs` -L%s -ltestu01 -lprobdist -lmylib -o %s",
                         testProgram_obj.Data(),libDirName.Data(),testProgram.Data() );
   
   iret |= gSystem->Exec(cmd);
   if (iret) {
      Error("Build","Error building %s",testProgram.Data());
      return;
   }



   printf("Ready to run the test !\n");
   printf("type for example : \n ./testu01_time \n ./testu01_bigcrush -small  \n ");



   return;


}
 
