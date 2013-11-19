///
/// Autors : N. Winckler
/*
 FOR ROOT application
 Get and handle command line options. Arguments handled are removed
 from the argument array. The following arguments are handled:
    -b : run in batch mode without graphics
    -x : exit on exception
    -n : do not execute logon and logoff macros as specified in .rootrc
    -q : exit after processing command line macro files
    -l : do not show splash screen
 The last three options are only relevant in conjunction with TRint.
 The following help and info arguments are supported:
    -?       : print usage
    -h       : print usage
    --help   : print usage
    -config  : print ./configure options
    -memstat : run with memory usage monitoring

 */
#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cmath>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <algorithm>
#include <fstream>
#include "TLegend.h"
#include "TSpectrum.h"
#include "TGraph.h"
#include "TMath.h"
#include <sys/types.h>
#include <sys/stat.h>

#include "rootheaders.hh"
//#include "osc2012_tools.h"
//#include "Analysis.h"
//#include "Myfunction.h"
#include "TXTDATA.h"
#include "TRandom.h"
#include "PrepareLSAinput.h"
#include "Interpolation.h"
#include "Accumulator64B.h"
#include "LSAtoFPGA.h"
#include "LSAParameter.h"

using namespace std;
//using namespace boost;

void test();
uint8_t GetXbitFormat(int8_t param_in, unsigned int Xbit);
uint16_t GetXbitFormat(int16_t param_in, unsigned int Xbit);
void testBitwiseOperation();

void Get16BitIntVal(double value)
{
	cout<<"Value = "<< value <<endl;
}

/// //////////////////////////////////////////////////////////////////////////
/// ////////////////////// START OF MAIN /////////////////////////////////////
/// //////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	

	bool draw=true;
	TApplication app("App", &argc, argv);
	//unsigned __int128 z0=0;
	
	test();
	//testBitwiseOperation();
	
	TDatime* daend = new TDatime();
	cout<<"Analysis end : "<<daend->GetTime()<<endl;
	if(draw) app.Run();

        return 0;
}



/// /////////////////////////////////////////////////////////////////////////////////
/// ///////////////////////////    END OF MAIN      /////////////////////////////////
/// /////////////////////////////////////////////////////////////////////////////////

void test()
{
	
	string GSIdir("/Users/winckler/subversion/sids/codes/FunctionGenerator/");
	string homedir("/home/nicolas/subversion/sids/codes/test/FGtest/");
	//string dir=homedir;
	string dir=GSIdir;

	string testdataname=dir+"testData.dat";
	string lineardataname=dir+"LinearData.dat";
	string quadraticdataname=dir+"QuadraData.dat";
	string NonEquDistquadraticdataname=dir+"NonEquiDistQuadData.dat";
        string SinData=dir+"LSAsinData.txt";
	double ScalingFactor=1.0/(pow(2.0,15.0)-1.0);
        //ScalingFactor=1.0;
	//*
	
        bool correction=true;
        //Interpolation MyInterpolation(lineardataname, quadraticdataname, correction,ScalingFactor);
	//Interpolation MyInterpolation(lineardataname, NonEquDistquadraticdataname, true);
	Interpolation MyInterpolation(lineardataname, SinData, correction,ScalingFactor);
	
        
	MyInterpolation.Interpolate(49);
	
	MyInterpolation.GetAllRelativeDeviation(correction);
	//MyInterpolation.PrintParameters();
	// */
	
}

void testBitwiseOperation()
{
	
	int16_t significandA=3;
	int16_t exponentA=-2;
	int16_t significandB=3;
	int16_t exponentB=-3;
	
	int16_t significandC=-3;
	int16_t exponentC=-4;
	int16_t significandD=-3;
	int16_t exponentD=-5;
	
	cout<<"A : "<<endl;
	Accumulator64B A(significandA,exponentA);
	cout<<"B : "<<endl;
	Accumulator64B B(significandB,exponentB);
	cout<<"C : "<<endl;
	Accumulator64B C(significandC,exponentC);
	cout<<"D : "<<endl;
	Accumulator64B D(significandD,exponentD);
	
	
	
	
	Accumulator64B E=C+D;
	E.PrintValue();
	
	Accumulator64B F;
	cout<<"F : "<<endl;
	F=E;
	F.PrintValue();
	
}




uint8_t GetXbitFormat(int8_t param_in, unsigned int Xbit)
{
	unsigned int shift=8-Xbit;
	uint8_t param_out=(uint8_t)param_in;
	param_out<<=shift;
	param_out>>=shift;
	
	return param_out;

}


uint16_t GetXbitFormat(int16_t param_in, unsigned int Xbit)
{
	unsigned int shift=16-Xbit;
	uint16_t param_out=(uint16_t)param_in;
	param_out<<=shift;
	param_out>>=shift;
	
	return param_out;

}






