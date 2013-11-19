#ifndef INTERPOLATION_H
#define INTERPOLATION_H
#include "rootheaders.hh"
#include "LSAtoFPGA.h"


using namespace std;

class Interpolation : public LSAtoFPGA
{
public:
	Interpolation(string LinFilename, string QuadFilename);
        Interpolation(string LinFilename, string QuadFilename, bool correction);
	Interpolation(string LinFilename, string QuadFilename, bool correction, double ScalingFactor);
	
        ~Interpolation();
	void Interpolate(int index);
        void Interpolate2(int index);
        void GetRelDeviation();
	double GetRelativeDeviation(int index, bool correction);
	void InterpolateAll(bool correction);
        void GetAllRelativeDeviation(bool correction);
	
private:

        unsigned int m_Ntot;
        void Init();
    
        TH1D* m_hXnp1_true;
        TH1D* m_hXnp1_app;
	TH1D* m_hXnp1_corr;
        
        TH1D* m_hDeviation;
	TH1D* m_hTheoRelDeviation_corr;
        TH1D* m_hAccuRelDeviation_corr;
	TH1D* m_hDeviation2TheoVal;
	TH1D* m_hLocalDeviation_corr;
        
        TH1D* m_par_a;
        TH1D* m_par_b;
	TH1D* m_par_c;
        
        TH1D*  m_hXN;
	TH1D* m_hXnALL;
        
	TH1D* m_shift_a;
	TH1D* m_shift_b;
	TH1D* m_shift_c;
	
};

#endif // INTERPOLATION_H
