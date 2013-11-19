#ifndef LSATOFPGA_H
#define LSATOFPGA_H

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <cmath>
//#include "TMath.h"// need ROOT library!
#include "TXTDATA.h"// My class for reading text files need boost library!

#include "Accumulator64B.h"
#include "LSAParameter.h"
//#include "rootheaders.hh"

using namespace std;


class LSAtoFPGA 
{
public:

	LSAtoFPGA(string LinFilename, string QuadFilename);
        LSAtoFPGA(string LinFilename, string QuadFilename, bool correction);
        LSAtoFPGA(string LinFilename, string QuadFilename, bool correction, double scalingFactor);
        LSAtoFPGA(const vector< vector<double> >& LSAQuadData, bool correction);
        LSAtoFPGA(const vector< vector<double> >& LSAQuadData, bool correction, double scalingFactor);
        void PrintParameters(int index);
	~LSAtoFPGA();
        

private:

	int m_Nstep;
	double m_timefact;
        bool m_correction;
	double m_ScalingFactor;
	vector<int> m_Nfpga;
	int m_Nindex;
        
	
protected:
	
        void GetLSAData(string LinFilename, string QuadFilename);
        void InitAttribute();
        void GetSeqPar(int i, int N, int P);
        bool CheckNval(int N);
        uint16_t GetShiftOutput(unsigned int index);
        void GetCoutputs(unsigned int index);
        uint8_t GetNfreqOutput(unsigned int index);
        void ProcessOutput();
        
        uint16_t GetXbitFormat(int16_t param_in, unsigned int Xbit);
        uint8_t GetXbitFormat(uint8_t param_in, unsigned int Xbit);
        vector<int16_t> Get16BitIntValue(double value);
        
        vector< vector<double> > m_LSAQuadInput;	// LSA parameters
	vector< vector<double> > m_LSALineInput;	// LSA parameters
        
	vector< vector<double> > m_TheoValueSet;	// Sequence parameters (true values)
	vector< vector<double> > m_ApprValueSet;	// Sequence parameters (approx values)
	vector< vector<double> > m_DeviValueSet;
        
        vector< vector<int16_t> > m_a_intValSet;
        vector< vector<int16_t> > m_b_intValSet;
        vector<int32_t> m_c_intValSet;
        
        vector<int16_t> m_a_out;
        vector<int16_t> m_b_out;
        vector<int32_t> m_c_out;
	vector<uint16_t> m_cMSB_out;
	vector<uint16_t> m_cLSB_out;
	
	vector<uint16_t> m_ShiftAB_out;
	vector<uint8_t> m_NumbFreq_out;
        
        vector<vector<Accumulator64B> > m_accu64BSet;
};


#endif 
