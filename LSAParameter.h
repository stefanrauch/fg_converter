#ifndef LSAPARAMETER_H
#define LSAPARAMETER_H

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <stdint.h>
#include "Accumulator64B.h"
//#include "rootheaders.hh"

using namespace std;


class LSAParameter 
{
public:

        LSAParameter();
        LSAParameter(double dt, double C, double B, double A);
        LSAParameter(double dt, double C, double B, double A, int N, int P);
	LSAParameter(double dt, double C, double B, double A, double ScalingFactor, int N, int P);
        ~LSAParameter();

        void ConvertLSAData(int N, int P);
        void CorrectDeviation();
        void Set(double dt, double C, double B, double A, int N, int P);
        void PrintAll();
        vector<int16_t> Get_a16b();
        vector<int16_t> Get_b16b();
        int32_t Get_c32b();
        vector<double> GetTheoVal();
        vector<double> GetApprVal();
        vector<double> GetDeviVal();
        vector<Accumulator64B> GetAccu64BVal();
        double GetAccuDeviation(const vector<Accumulator64B>& ParABC);

private:

        vector<int16_t> Get16BitIntVal(double value);// store 16 bit val and shift into vector
        double GetXN(double a, double b, double c, double N);
        void InitAttribute(double dt, double C, double B, double A);
        vector <double> FillTheoValues(double a, double b, double c,double XN, double dt, double tau, double N);
	int m_Nstep;
	double m_timefact;
	double m_ScalingFactor;
	
        double m_dt_in;
        double m_A_in;
        double m_B_in;
        double m_C_in;
        
	vector<double> m_TheoValues;	// Sequence parameters (true values)
	vector<double> m_ApprValues;	// Sequence parameters (approx values)
	vector<double> m_Deviation;
        
        vector<int16_t> m_a_intVal;
        vector<int16_t> m_b_intVal;
        int32_t m_c_intVal;
        vector<Accumulator64B> m_Accu64BData;//Accu a,b,c
        
       
};
#endif 
