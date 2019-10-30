void plot_sknuth_result(TString fileName, int i1 = -1, int i2 = -1 ) {

   auto file = TFile::Open(fileName);
   if (!file) return;

   auto hexp = (TH1*) file->Get("hexp");
   auto hobs = (TH1*) file->Get("hobs");

   if (!hexp || !hobs) {
      std::cout << "histogram not found in file " << std::endl;
      return;
   }
   // ratio plot divide by error of first histogram

   TString title = hobs->GetTitle() + TString(" from " ) + TString(fileName);
   hobs->SetTitle(title);
   hexp->SetLineColor(kRed);


   auto c1 = new TCanvas("c1", "A ratio example");
   auto rp = new TRatioPlot(hobs, hexp, "diffsig");

   rp->Draw();

   // print my self the residuals

   auto residuals = [&](int i1, int i2){ for (int i = i1; i <= i2; ++i){ double r = (hobs->GetBinContent(i)-hexp->GetBinContent(i))/sqrt(hexp->GetBinContent(\
i) ); std::cout << "bin : " << i << " residual = " << r << std::endl; } };

   if (i1 > 0 && i2 > 0)  
      residuals( i1, i2 );
   else
      residuals(1, hobs->GetNbinsX() );

   // compute tests in given range
   if (i1 > 0 && i2 > 0)
      hexp->GetXaxis()->SetRange(i1,i2); 
   if (i1 > 0 && i2 > 0)
      hobs->GetXaxis()->SetRange(i1,i2);

   /// make sure for the test statistics that error is zeor in expected histo
   for (int i = 1; i <= hexp->GetNbinsX(); ++i) { hexp->SetBinError(i, 0 ); } 

   
   hobs->Chi2Test(hexp, "UW P");
   auto pvalue = hobs->KolmogorovTest(hexp,"P"); 
   std::cout << "KS test - pvalue = " << pvalue << std::endl;
   pvalue  = hobs->AndersonDarlingTest(hexp); 
   std::cout << "AD test - pvalue = " << pvalue << std::endl;
}
   
