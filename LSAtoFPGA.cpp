#include "LSAtoFPGA.h"

LSAtoFPGA::LSAtoFPGA(string LinFilename, string QuadFilename)
{
	
	GetLSAData(LinFilename, QuadFilename);
	m_correction=false;
        InitAttribute();
	ProcessOutput();
}


LSAtoFPGA::LSAtoFPGA(string LinFilename, string QuadFilename, bool correction)
{
	
	GetLSAData(LinFilename, QuadFilename);
	m_correction=correction;
        InitAttribute();
	ProcessOutput();
}


LSAtoFPGA::LSAtoFPGA(string LinFilename, string QuadFilename, bool correction, double scalingFactor)
{
	
	GetLSAData(LinFilename, QuadFilename);
	m_correction=correction;
        m_ScalingFactor=scalingFactor;
        InitAttribute();
	ProcessOutput();
}



LSAtoFPGA::LSAtoFPGA(const vector< vector<double> >& LSAQuadData, bool correction)
{
	
	m_LSAQuadInput=LSAQuadData;
	m_correction=correction;
        InitAttribute();
	ProcessOutput();
}


LSAtoFPGA::LSAtoFPGA(const vector< vector<double> >& LSAQuadData, bool correction, double scalingFactor)
{
	
	m_LSAQuadInput=LSAQuadData;
	m_correction=correction;
        m_ScalingFactor=scalingFactor;
        InitAttribute();
	ProcessOutput();
}


LSAtoFPGA::~LSAtoFPGA() 
{
	
}

void LSAtoFPGA::GetLSAData(string LinFilename, string QuadFilename)
{
        /// Get data from txt file
        cout<<"READ FROM TEXT FILE START"<<endl;
	TXTDATA *LinData = new TXTDATA(LinFilename);// dt, b,a  y=ax+b
	TXTDATA *QuadData = new TXTDATA(QuadFilename);// dt, c ,b,a  y=ax2+bx+c
	
	m_LSALineInput = LinData->GetVector();
	m_LSAQuadInput = QuadData->GetVector();
	delete LinData;
	delete QuadData;
        cout<<"READ FROM TEXT FILE END"<<endl;
}

void LSAtoFPGA::InitAttribute()
{
	/// define some variables
	
	for(unsigned int k(0); k<8;++k) 
		m_Nfpga.push_back((int)(pow(2.0,(double)k)*250));
	
	int IntervalNumber=1;
	int NstepMax=1000;
	double dt_temp;
	
	m_timefact=1.0/1000000.0;// =1/1 MHz = 1mus
	
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Prepare the set of theoretical parameters for each input, and split when needed
	for(unsigned int i(0); i<m_LSAQuadInput.size();++i)
	{
		dt_temp=m_LSAQuadInput[i][0];
		m_Nstep=(int)(dt_temp/m_timefact);
		//*
                if(CheckNval(m_Nstep))
		{
			if(m_Nstep<=NstepMax)   // if e.g. N<=1000 normal conversion
                                GetSeqPar(i, 0, m_Nstep);
			else                    // else split N into k (=IntervalNumber) intervals of 1000 (=NstepMax) steps
			{
				IntervalNumber=m_Nstep/NstepMax;
				for(unsigned int k(0); k<IntervalNumber;++k) 
					GetSeqPar(i, k*NstepMax, NstepMax);
			}
		}
		else 
		{
			cout<<"*** Error : Derived number of interpolation steps N="<< m_Nstep <<" does not matched with the predefined FPGA values"<<endl;
			break;
		}
                 // */
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Initialisation of the intermediate and ouput parameters
	
	unsigned int lin=m_TheoValueSet.size();
        m_a_out.resize(lin);
        m_b_out.resize(lin);
        m_c_out.resize(lin);
	m_cMSB_out.resize(lin);
	m_cLSB_out.resize(lin);
	m_ShiftAB_out.resize(lin);
	m_NumbFreq_out.resize(lin);

}

void LSAtoFPGA::GetSeqPar(int index, int N, int P)
{
	if(CheckNval(P))
	{
		//LSAParameter par(m_LSAQuadInput[index][0],m_LSAQuadInput[index][1],m_LSAQuadInput[index][2],m_LSAQuadInput[index][3],N,P);
		LSAParameter par(m_LSAQuadInput[index][0],m_LSAQuadInput[index][1],m_LSAQuadInput[index][2],m_LSAQuadInput[index][3],m_ScalingFactor,N,P);
                if(m_correction) par.CorrectDeviation();
		m_TheoValueSet.push_back(par.GetTheoVal());
		m_a_intValSet.push_back(par.Get_a16b());
		m_b_intValSet.push_back(par.Get_b16b());
		m_c_intValSet.push_back(par.Get_c32b());
		m_ApprValueSet.push_back(par.GetApprVal());
		m_DeviValueSet.push_back(par.GetDeviVal());
		m_accu64BSet.push_back(par.GetAccu64BVal());
	}
}

void LSAtoFPGA::ProcessOutput()
{
        
        for(unsigned int i(0); i<m_TheoValueSet.size();++i)
	{
                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                /// final output To send to the FPGA
                // a
                m_a_out[i]=m_a_intValSet[i][0];

                // b
                m_b_out[i]=m_b_intValSet[i][0];

                // c
                GetCoutputs(i);

                //lambda_a lambda_b, and lambda_out : put signed 6 + 6 + 4 bits into a uint16_t
                m_ShiftAB_out[i]=GetShiftOutput(i);

                //N and sampling rate : put unsigned 3 + 3 bits into a uint8_t
                m_NumbFreq_out[i]=GetNfreqOutput(i);
		//if(i==0 || i==2) PrintParameters(i);
		//cout<<i<<endl;
                PrintParameters(i);
	}
}


bool LSAtoFPGA::CheckNval(int N)
{
	bool Nfound=false;
	if(m_Nfpga.size()>0)
	{
		for(unsigned int i(0); i<m_Nfpga.size();++i)
		{
			if(N==m_Nfpga[i])
			{
				Nfound=true;
				m_Nindex=i;
				break;
			}
		}
                if(!Nfound && N%1000==0)
                {
                        Nfound=true;
                        m_Nindex=5;
                }
	}
	return Nfound;
}




uint8_t LSAtoFPGA::GetNfreqOutput(unsigned int index)
{
	/*
	LSA gives dt
	dt x freq = N
	N tau = dt
	
	FPGA side:
	Array index : value		sample frequency:
	N (number of steps):		
	0:    32000			0:    Clk_in_Hz / 2000000  0.5 mu s
	1:    16000			1:    Clk_in_Hz / 1000000    1 mu s
	2:    8000			2:    Clk_in_Hz / 500000     2 mu s
	3:    4000			3:    Clk_in_Hz / 250000     4 mu s
	4:    2000			4:    Clk_in_Hz / 125000     8 mu s
	5:    1000			5:    Clk_in_Hz / 64000	     15.625 mu s
	6:    500			6:    Clk_in_Hz / 32000	     31.25 mu s
	7:    250			7:    Clk_in_Hz / 16000      62.5 mu s
	*/
	
	uint8_t NIndex=(uint8_t)m_Nindex; 
	uint8_t FreqIndex=1;// fixed at the moment
	
	uint8_t xNInd=GetXbitFormat(NIndex,3);
	uint8_t xfreqInd=GetXbitFormat(FreqIndex,3);
	uint8_t NandFContainer=0;
	
	// shift bits to left
	xNInd<<=3;
	NandFContainer=xNInd|xfreqInd;
	
	return NandFContainer;
}

uint16_t LSAtoFPGA::GetShiftOutput(unsigned int index)
{
	
	int16_t lambda_a=m_a_intValSet[index][1];	// shift of a 
	int16_t lambda_b=m_b_intValSet[index][1];	// shift of b
	
	uint16_t xa=GetXbitFormat(lambda_a,6);// 6 bit signed (two s complement) structure stored in unsigned 16
	uint16_t xb=GetXbitFormat(lambda_b,6);
	
	// Shift the ... shifts ... to prepare for "concatenation"
	xa<<=6;
	
	uint16_t ShiftAB=0;
	ShiftAB=xa|xb;// concatenate xa and xb in unsigned 16 bit storage -> the 2x6=12 MSB of shiftcontainer are for lambda_a and lambda_b
	
	//printBits(2, &Shiftcontainer);
	return ShiftAB;
}



void LSAtoFPGA::GetCoutputs(unsigned int index)
{
	uint32_t temp=(uint32_t)m_c_out[index];
	uint16_t MSB=0;
	uint16_t LSB=0;
	MSB=temp>>16;
	temp<<=16;
	temp>>=16;
	LSB|=temp;
	m_cMSB_out[index]=MSB;
	m_cLSB_out[index]=LSB;
}


uint16_t LSAtoFPGA::GetXbitFormat(int16_t param_in, unsigned int Xbit)
{
	unsigned int shift=16-Xbit;
	uint16_t param_out=(uint16_t)param_in;
	param_out<<=shift;
	param_out>>=shift;
	return param_out;
}


uint8_t LSAtoFPGA::GetXbitFormat(uint8_t param_in, unsigned int Xbit)
{
	unsigned int shift=8-Xbit;
	uint8_t param_out=(uint8_t)param_in;
	param_out<<=shift;
	param_out>>=shift;
	return param_out;
}


void LSAtoFPGA::PrintParameters(int i)
{
	
	printf("%d %d  %d  %d  %d  %d  %d \n",i,m_a_intValSet[i][0], m_a_intValSet[i][1], m_b_intValSet[i][0], m_b_intValSet[i][1], m_c_intValSet[i], m_Nstep);
	
}


vector<int16_t> LSAtoFPGA::Get16BitIntValue(double value)
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
	else temp=significand*pow(2.0,15);			// significand x 2^15  for negative values
	
	VectOut.push_back((int16_t)floor(temp + 0.5));	// store 16B int val
	VectOut.push_back((int16_t)(exponent-15));	// store shift
	
	return VectOut;
}

///////////////////////////////////////////////////////////////////// END /////////////////////////////////////////////////////////////////////




