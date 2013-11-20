#include "LSAParameter.h"
// comment

LSAParameter::LSAParameter()
{
	m_ScalingFactor=1.0;
        InitAttribute(0, 0, 0, 0);
	ConvertLSAData(0, 1);
}

LSAParameter::LSAParameter(double dt, double C, double B, double A)
{
	m_ScalingFactor=1.0;
        InitAttribute(dt, C, B, A);
	ConvertLSAData(0, m_Nstep);
}

LSAParameter::LSAParameter(double dt, double C, double B, double A, int N, int P)
{
	m_ScalingFactor=1.0;
        InitAttribute(dt, C, B, A);
	ConvertLSAData(N, P);
}


LSAParameter::LSAParameter(double dt, double C, double B, double A, double ScalingFactor, int N, int P)
{
	m_ScalingFactor=ScalingFactor;
        InitAttribute(dt, C, B, A);
	ConvertLSAData(N, P);
}

LSAParameter::~LSAParameter() 
{
	
}


void LSAParameter::Set(double dt, double C, double B, double A, int N, int P)
{
        m_ScalingFactor=1.0;
        InitAttribute(dt, C, B, A);
	ConvertLSAData(N, P);
}

void LSAParameter::InitAttribute(double dt, double C, double B, double A)
{
	/// define some variables
	
	m_dt_in=dt;
        m_A_in=A;
        m_B_in=B;
        m_C_in=C;
        m_timefact=1.0/1000000.0;// =1/1 MHz = 1mus
        m_Nstep=(int)(m_dt_in/m_timefact);
	
}


void LSAParameter::ConvertLSAData(int N, int P)
{
	double A_temp;
	double B_temp;
	double C_temp;
	double dt_temp;
	double XN_temp;
	double XNpP_temp;
	double DeltaXNpP;
	double RelDev;
	double N_temp=(double)N;
	double P_temp=(double)P;
	m_Nstep=P;
	dt_temp=m_timefact*P_temp;
	
        m_TheoValues.clear();
        m_a_intVal.clear();
        m_b_intVal.clear();
        //m_c_intVal;
        m_ApprValues.clear();
        m_Deviation.clear();
        m_Accu64BData.clear();
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// convert the LSA parameters to sequence parameters
	A_temp=(2*m_A_in*m_timefact*m_timefact)/m_ScalingFactor;
	B_temp=(m_A_in*m_timefact*m_timefact+m_B_in*m_timefact)/m_ScalingFactor;
	C_temp=m_C_in/m_ScalingFactor;
	XN_temp=GetXN(A_temp, B_temp, C_temp, N_temp);//XN
	
	C_temp=XN_temp;
	B_temp+=A_temp*N_temp;
	XNpP_temp=GetXN(A_temp, B_temp, C_temp, P_temp);//XN+P
	
	m_TheoValues=FillTheoValues(A_temp, B_temp, C_temp, XNpP_temp, dt_temp, m_timefact, P_temp);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// store sequence parameter values (int16_t, and int32_t)
                
        // store a_int and shift
	m_a_intVal=Get16BitIntVal(A_temp);
	
        // store b_int and shift
	m_b_intVal=Get16BitIntVal(B_temp);
	
        // store c_int
	m_c_intVal=(int32_t)floor(C_temp+0.5);	
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// store approximative values
	
	// a approx
        m_ApprValues.push_back((double)(m_a_intVal[0])*pow(2.0,m_a_intVal[1])); 
        // b approx
        m_ApprValues.push_back((double)(m_b_intVal[0])*pow(2.0,m_b_intVal[1])); 
        // c approx
        m_ApprValues.push_back((double) m_c_intVal);
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// store deviation
        
        // Delta a, Delta b, Delta c, 
	for(unsigned int i(0); i<3;++i)         // [0]->Da, [1]->Db, [2]->Dc
		m_Deviation.push_back(m_TheoValues[i]-m_ApprValues[i]);
	
        // Delta XN
	DeltaXNpP=GetXN(m_Deviation[0], m_Deviation[1], m_Deviation[2], P_temp);
	// DeltaXN/XN
        RelDev=DeltaXNpP/XNpP_temp;
        
	m_Deviation.push_back(DeltaXNpP);
	m_Deviation.push_back(RelDev);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Store accumulator
        Accumulator64B a_app(m_a_intVal[0],m_a_intVal[1]);
	Accumulator64B b_app(m_b_intVal[0],m_b_intVal[1]);
	Accumulator64B c_app(m_c_intVal);
	m_Accu64BData.push_back(a_app);
	m_Accu64BData.push_back(b_app);
	m_Accu64BData.push_back(c_app);
}

void LSAParameter::CorrectDeviation()
{
        // correct the input values
        bool correctA=false;
        bool correctB=true;
        bool correctAccB=false;
        
        double AccuDeviation=this->GetAccuDeviation(m_Accu64BData);
        
        // a'=a+K with correction term K= 2 Delta XN/(N(N-1))
        double A_prime=m_TheoValues[0]+2*m_Deviation[3]/((double)(m_Nstep*(m_Nstep-1)));
        
        // b'=b+K with correction term K= Delta XN/N
        double B_prime=m_TheoValues[1]+m_Deviation[3]/((double)m_Nstep);

        
        if(correctA)
        {
                // store a_int and shift
                m_a_intVal.clear();
                m_a_intVal=Get16BitIntVal(A_prime);
                m_ApprValues[0]=(double)(m_a_intVal[0])*pow(2.0,m_a_intVal[1]);
                m_Deviation[0]=m_TheoValues[0]-m_ApprValues[0];
                m_Deviation[3]=GetXN(m_Deviation[0], m_Deviation[1], m_Deviation[2], m_Nstep);
                m_Deviation[4]=m_Deviation[3]/m_TheoValues[3];
                Accumulator64B a_app(m_a_intVal[0],m_a_intVal[1]);
                m_Accu64BData[0]=a_app;
        }
        
        
        if(correctB)
        {
                // store b_int and shift
                m_b_intVal.clear();
                m_b_intVal=Get16BitIntVal(B_prime);
                m_ApprValues[1]=(double)(m_b_intVal[0])*pow(2.0,m_b_intVal[1]);
                m_Deviation[1]=m_TheoValues[1]-m_ApprValues[1];
                m_Deviation[3]=GetXN(m_Deviation[0], m_Deviation[1], m_Deviation[2], m_Nstep);
                m_Deviation[4]=m_Deviation[3]/m_TheoValues[3];
                Accumulator64B b_app(m_b_intVal[0],m_b_intVal[1]);
                m_Accu64BData[1]=b_app;
        }
        
        if(correctAccB)
        {
                // store b_int and shift
                B_prime=m_TheoValues[1]+AccuDeviation/((double)m_Nstep);
                m_b_intVal.clear();
                m_b_intVal=Get16BitIntVal(B_prime);
                m_ApprValues[1]=(double)(m_b_intVal[0])*pow(2.0,m_b_intVal[1]);
                m_Deviation[1]=m_TheoValues[1]-m_ApprValues[1];
                m_Deviation[3]=GetXN(m_Deviation[0], m_Deviation[1], m_Deviation[2], m_Nstep);
                m_Deviation[4]=m_Deviation[3]/m_TheoValues[3];
                Accumulator64B b_app(m_b_intVal[0],m_b_intVal[1]);
                m_Accu64BData[1]=b_app;
        }
}


vector<int16_t> LSAParameter::Get_a16b()
{
        return m_a_intVal;
}

vector<int16_t> LSAParameter::Get_b16b()
{
        return m_b_intVal;
}

int32_t LSAParameter::Get_c32b()
{
        return m_c_intVal;
}

vector<double> LSAParameter::GetTheoVal()
{
        return m_TheoValues;
}

vector<double> LSAParameter::GetApprVal()
{
        return m_ApprValues;
}

vector<double> LSAParameter::GetDeviVal()
{
        return m_Deviation;
}

vector<Accumulator64B> LSAParameter::GetAccu64BVal()
{
	return m_Accu64BData;
}

vector<int16_t> LSAParameter::Get16BitIntVal(double value)
{
	/// /////////////////////////// Keep the 15 most significant bits 
	//signed integer
	// A = significand x 2^exponent = significand x 2^15 x 2^-15 x 2^exponent
	// A = B x 2^(exponent-15) = B x 2^shift
	vector<int16_t> VectOut;
	double significand; 
	double temp=0.0;
	int exponent; 
	int16_t param_out;
	significand=frexp(value,&exponent);			// get significand x 2^exponent from input A_temp
	if(significand>=0.0) temp=significand*(pow(2.0,15.0)-1);	// significand x [2^15 - 1] for positive or zero values
	else temp=significand*pow(2.0,15.0);			// significand x 2^15  for negative values
	
	VectOut.push_back((int16_t)floor(temp + 0.5));	// store 16B int val
	VectOut.push_back((int16_t)(exponent-15));	// store shift
	
	return VectOut;
}


double LSAParameter::GetXN(double a, double b, double c, double N)
{
	double result=a*N*N/2.0+(b-a/2.0)*N+c;
	return result;
}

vector <double> LSAParameter::FillTheoValues(double a, double b, double c,double XN, double dt, double tau, double N)
{
	vector <double> vect_temp;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// store "true" sequence parameter values (double)
	vect_temp.push_back(a);		// 0
	vect_temp.push_back(b);		// 1
	vect_temp.push_back(c);		// 2
	vect_temp.push_back(XN);	// 3
	vect_temp.push_back(dt);	// 4
	vect_temp.push_back(tau);	// 5
	vect_temp.push_back(N);		// 6
	
	return vect_temp;
}


void LSAParameter::PrintAll()
{
	cout<<"------------- Print all parameters -------------"<<endl;
	cout<<"Theo : ";
	for(unsigned int i(0); i<m_TheoValues.size();++i)
		cout<<m_TheoValues[i]<<" ";
	cout<<endl;
	
	cout<<"Approx : ";
	for(unsigned int i(0); i<m_ApprValues.size();++i)
		cout<<m_ApprValues[i]<<" ";
	cout<<endl;
	
	cout<<"Deviation : ";
	for(unsigned int i(0); i<m_Deviation.size();++i)
		cout<<m_Deviation[i]<<" ";
	cout<<endl;
	
	cout<<"aint : ";
	for(unsigned int i(0); i<m_a_intVal.size();++i)
		cout<<m_a_intVal[i]<<" ";
	cout<<endl;
	
	cout<<"bint : ";
	for(unsigned int i(0); i<m_b_intVal.size();++i)
		cout<<m_b_intVal[i]<<" ";
	cout<<endl<<endl;
}


double LSAParameter::GetAccuDeviation(const vector<Accumulator64B>& ParABC)
{
        double deviation=0.0;
        if(ParABC.size()==3)
        {
                double Xnp1_true;
                int Nmax=(int)m_TheoValues[6];

                // Definition of the sequences
                // Q_0=b, X_0=C
                // Q_n+1 = Q_n + a
                // X_n+1 = X_n + Q_n

                /// // Initialisation (n=0)
                // "true" value
                double a_true=m_TheoValues[0];
                double b_true=m_TheoValues[1];
                double c_true=m_TheoValues[2];
                //Accu
                Accumulator64B Xn_D=ParABC[2];	// X0 = c  Int16 value * 2^shiftC
                Accumulator64B Qn_D=ParABC[1];	// Q0 = b
                Accumulator64B Xnp1_D;
                Accumulator64B Qnp1_D;
                
                //Simulate the interpolation done by the accumulator
                for(int n=0;n<=Nmax ;n++)
                {
                        // Real world value
                        Qnp1_D=Qn_D+ParABC[0]; // Q_n+1 = Q_n + a  (24 bits representation)
                        Xnp1_D=Xn_D+Qn_D; // X_n+1 = Qn + Xn
                        
                        Xnp1_true=(double)((n+1)*(n+1))*a_true/2.0 +(double)(n+1)*(b_true-a_true/2.0)+c_true;
                        deviation=(Xnp1_true-(double)Xnp1_D.Get32BValue())/Xnp1_true;
                        
                        // Increment
                        Qn_D=Qnp1_D;
                        Xn_D=Xnp1_D;
                }
                
        }
        else
        {
                cout<<"*** ERROR: Accumulator parameters incomplete"<<endl;
        }
        return deviation;
}
///////////////////////////////////////////////////////////////////// END /////////////////////////////////////////////////////////////////////




