///
/// Authors : N. Winckler

#include <stdint.h>
#include <iostream>
#include <string>
#include <cmath>
#include <stdlib.h>
#include <vector>

#include "rootheaders.hh"
#include "TXTDATA.h"
#include "Interpolation.h"
#include "Accumulator64B.h"
#include "LSAtoFPGA.h"
#include "LSAParameter.h"

using namespace std;
//using namespace boost;

void test();
void convert(string input, string output, int scalingPow);

/// //////////////////////////////////////////////////////////////////////////
/// ////////////////////// START OF MAIN /////////////////////////////////////
/// //////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	string InputFileName;
        string OutputFileName;
        int scalingPow=1;// bit size 
	if (argc > 1)
	{
		while(--argc > 1 && **++argv == '-') switch (toupper(argv[0][1]))  // WARNING : USER's OPTIONs HAVE PRIORITY OVER CONFIG FILE
		{
                        case 'I':
				InputFileName = (--argc,*++argv);
				break;
                        case 'O':
                                OutputFileName = (--argc,*++argv);
                                break;
                        case 'P':
				scalingPow = strtol((--argc,*++argv), (char **)0, 0);
                                break;
                        
                                
		}
	}
        
	bool draw=true;
	TApplication app("App", &argc, argv);
	
        if(!InputFileName.empty() && !OutputFileName.empty())
        {
            convert(InputFileName, OutputFileName, scalingPow);
        }
	else test();
	
	if(draw) app.Run();

        return 0;
}



/// /////////////////////////////////////////////////////////////////////////////////
/// ///////////////////////////    END OF MAIN      /////////////////////////////////
/// /////////////////////////////////////////////////////////////////////////////////
void convert(string inputname, string outputname, int scalingPow)
{
        double exp=(double)(scalingPow-1);//-1 because two's complement

        double ScalingFactor=1.0;
        if(exp>1.0) ScalingFactor=1.0/(pow(2.0,exp)-1.0);
        
	bool verbose=false;
        bool correction=true;
        bool writeToFile=true;
        
        LSAtoFPGA MyConversion(inputname, correction, ScalingFactor, verbose);
        if(writeToFile) MyConversion.WritetToFile(outputname);
}


void test()
{
	
	string GSIdir("/Users/winckler/subversion/sids/codes/FunctionGenerator/");
	string dir=GSIdir;

	string testdataname=dir+"testData.dat";
	string lineardataname=dir+"LinearData.dat";
	string quadraticdataname=dir+"QuadraData.dat";
	string NonEquDistquadraticdataname=dir+"NonEquiDistQuadData.dat";
        string SinData=dir+"LSAsinData.txt";
        
        double exp=15.0;
        exp=23.0;
        double ScalingFactor=1.0/(pow(2.0,exp)-1.0);
        
        //ScalingFactor=1.0;
	//*
	bool verbose=false;
        bool correction=true;
        Interpolation MyInterpolation( quadraticdataname, correction,1.0);
	//Interpolation MyInterpolation(lineardataname, NonEquDistquadraticdataname, correction,1.0);
	//Interpolation MyInterpolation(lineardataname, SinData, correction,ScalingFactor, verbose);
	
        
	MyInterpolation.Interpolate(99);
	
	MyInterpolation.GetAllRelativeDeviation(correction);
	//MyInterpolation.PrintParameters();
	// */
	
}
