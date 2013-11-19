#include "Interpolation.h"

Interpolation::Interpolation(string LinFilename, string QuadFilename) : LSAtoFPGA(LinFilename,QuadFilename)
{
	Init();
}

Interpolation::Interpolation(string LinFilename, string QuadFilename, bool correction) : LSAtoFPGA(LinFilename, QuadFilename, correction)
{
	Init();
}

Interpolation::Interpolation(string LinFilename, string QuadFilename, bool correction, double ScalingFactor) : LSAtoFPGA(LinFilename, QuadFilename, correction,ScalingFactor)
{
	Init();
}

Interpolation::~Interpolation()
{
	
}



void Interpolation::Interpolate(int index)
{
	double deviation1;
	double deviationcorr;
	double deviationFormulaVsSequence;
	double Xnp1_true;

	// Definition of the sequences
	// Q_0=b, X_0=C
	// Q_n+1 = Q_n + a
	// X_n+1 = X_n + Q_n
        
	int Nmax=(int)m_TheoValueSet[index][6];
	double timefact=m_TheoValueSet[index][5];
	m_hXnp1_true = new TH1D("X_{n}","X_{n}",Nmax+1,0.0,(double)(Nmax+1)*timefact);
	m_hXnp1_app = new TH1D("X_{n}^{ap}","X_{n}^{ap}",Nmax+1,0.0,(double)(Nmax+1)*timefact);
	m_hXnp1_corr = new TH1D("X_{n}^{c}","X_{n}^{c}",Nmax+1,0.0,(double)(Nmax+1)*timefact);
	
	m_hDeviation = new TH1D("deviation","#frac{#Delta X_{n}}{X_{n}}",Nmax+1,0.0,(double)(Nmax+1)*timefact);
	m_hLocalDeviation_corr = new TH1D("deviation-corr","#frac{#Delta X_{n}}{X_{n}}(corr)",Nmax+1,0.0,(double)(Nmax+1)*timefact);
	m_hDeviation2TheoVal = new TH1D("deviation-formula vs sequence","#frac{#Delta X_{n}}{X_{n}}(theo)",Nmax+1,0.0,(double)(Nmax+1)*timefact);
	
	/// // Initialisation
	//n=0
	double a_true=m_TheoValueSet[index][0];
	double b_true=m_TheoValueSet[index][1];
	double c_true=m_TheoValueSet[index][2];
	cout<<"a_true="<< a_true<<endl;
	cout<<"b_true="<< b_true<<endl;
	cout<<"c_true="<< c_true<<endl;
	cout<<"Delta XN="<<m_DeviValueSet[index][3]<<endl;
	cout<<"Delta XN/XN="<<m_DeviValueSet[index][4]<<endl;
	cout<<"Delta XN/N="<<m_DeviValueSet[index][3]/1000<<endl;
	
	
	vector<int16_t> b_notcorr=Get16BitIntValue(b_true);
	Accumulator64B a_app(m_accu64BSet[index][0]);
	Accumulator64B b_app(b_notcorr[0],b_notcorr[1]);
	Accumulator64B c_app(m_accu64BSet[index][2]);
	
	Accumulator64B Xn_D=c_app;	// X0 = c  Int16 value * 2^shiftC
	Accumulator64B Qn_D=b_app;	// Q0 = b
	Accumulator64B Xnp1_D;
	Accumulator64B Qnp1_D;
	
	
	Accumulator64B a_corr=a_app;
	Accumulator64B b_corr(m_accu64BSet[index][1]);
	Accumulator64B c_corr=c_app;
	
	Accumulator64B Xn_Dcorr=c_corr;
	Accumulator64B Qn_Dcorr=b_corr;
	Accumulator64B Xnp1_Dcorr;
	Accumulator64B Qnp1_Dcorr;
	
	double Xn_trueSeq=c_true;
	double Qn_trueSeq=b_true;
	double Xnp1_trueSeq;
	double Qnp1_trueSeq;
	
	
	//Nmax=10;
	for(int n=0;n<=Nmax ;n++)
    	{
		
		// Real world value
		Qnp1_D=Qn_D+a_app; // Q_n+1 = Q_n + a  (24 bits representation)
		Xnp1_D=Xn_D+Qn_D; // X_n+1 = Qn + Xn
		
		Qnp1_Dcorr=Qn_Dcorr+a_corr; // Q_n+1 = Q_n + a  (24 bit representation with corrected b parameter)
		Xnp1_Dcorr=Xn_Dcorr+Qn_Dcorr; // X_n+1 = Qn + Xn
		
		Xnp1_true=(double)((n+1)*(n+1))*a_true/2.0 +(double)(n+1)*(b_true-a_true/2.0)+c_true;
		
		
		Qnp1_trueSeq=Qn_trueSeq+a_true;
		Xnp1_trueSeq=Xn_trueSeq+Qn_trueSeq;
		double DX= (double)Xnp1_Dcorr.Get32BValue();//Xnp1_true-Xnp1_trueSeq;
                
		//printf("n=%d  a_true= %f  b_true= %f  c_true= %f  \n",n, a_true,b_true,c_true);
		//printf("Xnp1(true, formula) = %f  Xnp1(true, sequence) = %f   Xnp1(Accu) = %f \n \n",Xnp1_true, Xnp1_trueSeq, DX);
		
		
		//Xnp1_true=Xnp1_trueSeq;
		deviation1=(Xnp1_true-(double)Xnp1_D.Get32BValue())/Xnp1_true;
		
		deviationcorr=(Xnp1_true-(double)Xnp1_Dcorr.Get32BValue())/Xnp1_true;
		deviationFormulaVsSequence=(Xnp1_true-Xnp1_trueSeq)/Xnp1_true;
		
		// fill histograms
		m_hXnp1_true->SetBinContent(n+1,Xnp1_true);
		m_hXnp1_app->SetBinContent(n+1,(double)Xnp1_D.Get32BValue());
		m_hXnp1_corr->SetBinContent(n+1,(double)Xnp1_Dcorr.Get32BValue());
		m_hDeviation->SetBinContent(n+1,deviation1);
		m_hLocalDeviation_corr->SetBinContent(n+1,deviationcorr);
		m_hDeviation2TheoVal->SetBinContent(n+1,deviationFormulaVsSequence);
		
		
		// Increment
		Qn_D=Qnp1_D;
		Xn_D=Xnp1_D;
		
		Qn_Dcorr=Qnp1_Dcorr;
		Xn_Dcorr=Xnp1_Dcorr;
		
		Qn_trueSeq=Qnp1_trueSeq;
		Xn_trueSeq=Xnp1_trueSeq;
		
	}
	// end loop
	
	TCanvas * canvaa2 = new  TCanvas("canvaa2","canvaa2",1000,800);
	canvaa2->Divide(3,2);
		
	canvaa2->cd(1);
	m_hXnp1_app->Draw();
	m_hXnp1_true->Draw("same");
	m_hXnp1_true->SetLineColor(2);
		
	canvaa2->cd(2);
	m_hXnp1_corr->Draw();
	m_hXnp1_true->Draw("same");
		
	canvaa2->cd(4);
	m_hDeviation->Draw();
		
	canvaa2->cd(5);
	m_hLocalDeviation_corr->Draw();
	canvaa2->cd(6);
	m_hDeviation2TheoVal->Draw();
	
}


void Interpolation::InterpolateAll(bool correction)
{
        
        
        
}



void Interpolation::GetAllRelativeDeviation(bool correction)
{
	double RelError;
	
	if(correction) m_hAccuRelDeviation_corr = new TH1D("#Delta X_{n}^{c}/X_{n}","#Delta X_{n}^{c}/X_{n}",m_TheoValueSet.size(),0.0,(double)m_TheoValueSet.size());
	else m_hAccuRelDeviation_corr = new TH1D("#Delta X_{n}/X_{n}","#Delta X_{n}/X_{n}",m_TheoValueSet.size(),0.0,(double)m_TheoValueSet.size());
	for(unsigned int i(0); i<m_TheoValueSet.size();++i)
	{
		RelError=GetRelativeDeviation(i, correction);
		m_hAccuRelDeviation_corr->SetBinContent(i+1, RelError);
	}
	TCanvas * canvaRelDev = new  TCanvas("canvaRelDev","canvaRelDev",1000,800);
	m_hAccuRelDeviation_corr->Draw();
}




double Interpolation::GetRelativeDeviation(int index, bool correction)
{
	double deviation1;
	double deviationcorr;
	double Xnp1_true;
	int Nmax=(int)m_TheoValueSet[index][6];
	
	// Definition of the sequences
	// Q_0=b, X_0=C
	// Q_n+1 = Q_n + a
	// X_n+1 = X_n + Q_n
	
	/// // Initialisation
	//n=0
	// "true" value
	double a_true=m_TheoValueSet[index][0];
	double b_true=m_TheoValueSet[index][1];
	double c_true=m_TheoValueSet[index][2];
	
	// Construct accumulator with integer parameters without correction
	vector<int16_t> b_notcorr=Get16BitIntValue(b_true);
	Accumulator64B a_app(m_accu64BSet[index][0]);
	Accumulator64B b_app(b_notcorr[0],b_notcorr[1]);
	Accumulator64B c_app(m_accu64BSet[index][2]);
	
	Accumulator64B Xn_D=c_app;	// X0 = c  Int16 value * 2^shiftC
	Accumulator64B Qn_D=b_app;	// Q0 = b
	Accumulator64B Xnp1_D;
	Accumulator64B Qnp1_D;
	
	// Construct accumulator with integer parameters with correction
	Accumulator64B a_corr=a_app;
	Accumulator64B b_corr(m_accu64BSet[index][1]);
	Accumulator64B c_corr=c_app;
	
	Accumulator64B Xn_Dcorr=c_corr;
	Accumulator64B Qn_Dcorr=b_corr;
	Accumulator64B Xnp1_Dcorr;
	Accumulator64B Qnp1_Dcorr;
	
	//Simulate the interpolation done by the accumulator
	for(int n=0;n<=Nmax ;n++)
    	{
		// Real world value
		Qnp1_D=Qn_D+a_app; // Q_n+1 = Q_n + a  (24 bits representation)
		Xnp1_D=Xn_D+Qn_D; // X_n+1 = Qn + Xn
		
		Qnp1_Dcorr=Qn_Dcorr+a_corr; // Q_n+1 = Q_n + a  (24 bit representation with corrected b parameter)
		Xnp1_Dcorr=Xn_Dcorr+Qn_Dcorr; // X_n+1 = Qn + Xn
		
		Xnp1_true=(double)((n+1)*(n+1))*a_true/2.0 +(double)(n+1)*(b_true-a_true/2.0)+c_true;
		
		deviation1=(Xnp1_true-(double)Xnp1_D.Get32BValue())/Xnp1_true;
		deviationcorr=(Xnp1_true-(double)Xnp1_Dcorr.Get32BValue())/Xnp1_true;
		
		// Increment
		Qn_D=Qnp1_D;
		Xn_D=Xnp1_D;
		
		Qn_Dcorr=Qnp1_Dcorr;
		Xn_Dcorr=Xnp1_Dcorr;
	}
	// end loop
        
	//printf("i=%d  a_true= %f  b_true= %f  c_true= %f  \n",index, a_true,b_true,c_true);
        //printf("i=%d  a_acc= %f  b_acc= %f  c_acc= %f  \n",index, (double)a_app.Get32BValue(), (double)b_app.Get32BValue(), (double)c_app.Get32BValue());
        //printf("i=%d  a_acc_c= %f  b_acc_c= %f  c_acc_c= %f  \n",index, (double)a_corr.Get32BValue(), (double)b_corr.Get32BValue(), (double)c_corr.Get32BValue());
        
        //printf("Xnp1(true, formula) = %f  Xnp1(Accu) = %f   DXnp1(Accu) = %f \n \n",Xnp1_true, (double)Xnp1_D.Get32BValue(),deviationcorr);
	
	if(correction) return deviationcorr;
	else return deviation1;
}


void Interpolation::Init()
{
	m_Ntot=1;//1 for X0
	m_hXN = new TH1D("XN","XN",m_TheoValueSet.size(),0.0,(double)m_TheoValueSet.size());
	
	m_hTheoRelDeviation_corr = new TH1D("DX_{n}^{c}/X_{n}","DX_{n}^{c}/X_{n}",m_TheoValueSet.size(),0.0,(double)m_TheoValueSet.size());
	
	m_par_a = new TH1D("par_a","par_a",m_a_intValSet.size(),0.0,(double)m_a_intValSet.size());
	m_par_b = new TH1D("par_b","par_b",m_b_intValSet.size(),0.0,(double)m_b_intValSet.size());
	m_par_c = new TH1D("par_c","par_c",m_c_intValSet.size(),0.0,(double)m_c_intValSet.size());
	
	m_shift_a = new TH1D("shift_a","shift_a",m_a_intValSet.size(),0.0,(double)m_a_intValSet.size());
	m_shift_b = new TH1D("shift_b","shift_b",m_b_intValSet.size(),0.0,(double)m_b_intValSet.size());
	//m_shift_c = new TH1D("shift_c","shift_c",m_c_intValSet.size(),0.0,(double)m_c_intValSet.size());
	
	double XNmin=m_TheoValueSet[0][2];
	double XNmax=m_TheoValueSet[m_TheoValueSet.size()-1][2];
	double scaling=(XNmax-XNmin)/(pow(2.0,24.0)-1.0);
	
	for(unsigned int i(0); i<m_TheoValueSet.size();++i)
	{
		m_Ntot+=m_TheoValueSet[i][6];
		m_par_a->SetBinContent(i+1,m_a_intValSet[i][0]);
		m_par_b->SetBinContent(i+1,m_b_intValSet[i][0]);
		m_par_c->SetBinContent(i+1,m_c_intValSet[i]);
		
		m_shift_a->SetBinContent(i+1,m_a_intValSet[i][1]);
		m_shift_b->SetBinContent(i+1,m_b_intValSet[i][1]);
		
		m_hTheoRelDeviation_corr->SetBinContent(i+1,m_DeviValueSet[i][4]);
	}
	
	m_hXnALL = new TH1D("XN(all)","XN(all)",m_Ntot,0.0,(double)m_Ntot);
	
	cout<<"N tot = "<< m_Ntot <<endl;
	TCanvas * canvaa1 = new  TCanvas("canvaa1","canvaa1",1000,800);
	canvaa1->Divide(3,2);
	canvaa1->cd(1);
	m_hTheoRelDeviation_corr->Draw();
	
	canvaa1->cd(2);
	m_par_a->Draw();
	
	canvaa1->cd(3);
	m_par_b->Draw();
	
	canvaa1->cd(4);
	m_par_c->Draw();
	
	canvaa1->cd(5);
	m_shift_a->Draw();
	
	canvaa1->cd(6);
	m_shift_b->Draw();
}
