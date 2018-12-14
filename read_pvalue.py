#!/usr/bin/env python
#
#script to read p-values from a file
#

import sys, re, os
import ROOT


class Analyzer : 

    def __init__(self,name) :

        self.fileName = name
        return

    def process(self, valueList):

        print "processing file   " + self.fileName



#        oldString2 = "namespace Anaphe"
#        newString2 = "namespace pi"
        
        fileIn = open(self.fileName,'r')
        
        inputList = fileIn.readlines()

        #find lines in file containining p-value
        vp = []
        i= 0
        nfailure = 0
        for line in inputList:
            ipos = line.find('p-value of test')
            if (ipos != -1) :  
                i += 1
                svalues = line.split()
                spval = svalues[len(svalues)-1]
                if (spval == "*****") :
                    spval =  svalues[len(svalues)-2]
                    print "test ",i," failed with p -value",spval
                    nfailure += 1
                pval = float(spval)
                pval = spval
                vp.append(pval)
                

        print "found in file - pvalues ",len(vp), " number of failures = ",nfailure 

        valueList.append(vp)
        #writing in a file
#        lineOut = "{ "
#        i = 0
#        for p in vp:
#            i+=1
#            lineOut += str(p)
#            if (i < len(vp) ):
#                lineOut += " , "
                

#        lineOut += " }\n\n"
#        print lineOut
#        fileOut.write(lineOut)
        fileIn.close()
        
        return


def main() :

    
# if given do single file otherwise all files in current directory
#
        valueList = []
        if ( len(sys.argv) == 2 ) :
            rep = Analyzer(sys.argv[1])
            #fileOut = open("test_pvalues.dat",'w')
            rep.process(valueList)


        else:
            if ( sys.argv[1] == "all") : 
                #target = "html"
                name = sys.argv[2]
                print "process all files in " + os.getcwd()," for ", name
                #print "Look for files with type", target
                #fileOutName = name + "_pvalues.dat"
                #fileOut = open(fileOutName,'w')
                
                for f in os.listdir(".") :
                    if os.path.isfile(f) :
                        if (f.find(name) != -1): 
                            sep = f.split(".")
                            ok = False
                            for s in sep:
                                if (s.find("run") ): ok = True
                                if (sep[len(sep)-1] != "log"): ok = False

                                
                            if (ok) : 
                                rep = Analyzer(f)
                                
#                                rep.process(fileOut)
                                rep.process(valueList)



#                fileOut.close()
                print "\n done "
        
        nexps = len(valueList)
        if (nexps == 0 ) : return
        ntests = len(valueList[0])
        if (ntests == 0) : return
        # write and analyze p values using ROOT
        print "found ",nexps," experiments with ",ntests," tests"
        m = ROOT.TMatrixD(ntests, nexps );
        h2 = ROOT.TH2D("h2","p Values",ntests,0.5,ntests+0.5,nexps,0.,1.)
        i = 0; 
        for  exp in valueList :
            j = 0
            for t in exp :            
                h2.Fill( float(j+1), float(t) )
                m[j][i] = float(t)
                j+= 1
            i+=1

        fileOutName = name + "_pvalues.root"
        fileOut = ROOT.TFile.Open(fileOutName,"RECREATE")
        m.Write("pmat")
        h2.Write("h2")
        fileOut.Close()
        return
        
if ( __name__ == "__main__" ) :
    main()
    
