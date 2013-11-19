#include "Accumulator64B.h"

Accumulator64B::Accumulator64B()
{
	Init();
	
}

Accumulator64B::Accumulator64B(const Accumulator64B &Accu)
{
	 // option
	this->m_verbose=Accu.m_verbose;
	this->m_RoundingCorrection=Accu.m_RoundingCorrection;
	this->m_RoundingThreshold=Accu.m_RoundingThreshold;

	//input
	this->m_value=Accu.m_value;
	this->m_sig16=Accu.m_sig16;
	this->m_exp16=Accu.m_exp16;

	//Accumulator 64 bits signed integer (2's complement)
	this->m_saccu=Accu.m_saccu;
	this->m_RadixPoint=Accu.m_RadixPoint;

	//output
	this->m_32IntVal=Accu.m_32IntVal;
}

Accumulator64B::Accumulator64B(double input)
{
	Init();// TODO : init from double number with frexp
}

Accumulator64B::Accumulator64B(int32_t input)
{
	Init();
	m_32IntVal=input;
	int64_t temp=(int64_t)input;
	temp<<=m_RadixPoint;
	m_saccu=temp;
}


Accumulator64B::Accumulator64B(int16_t significand, int16_t exponent)
{
	Init();
	m_sig16=significand;
	m_exp16=exponent;
	m_value=(double)m_sig16*TMath::Power(2,(double)m_exp16);
	uint16_t shift;
	m_saccu=(int64_t)significand;
	
	if(m_verbose) 
	{
		cout<<"Cast int16_t to int64_t"<<endl;
		PrintBits(8,&m_saccu);
	}
	m_saccu<<=m_RadixPoint;
	if(m_verbose) 
	{
		cout<<"Left shift of "<< m_RadixPoint << " Bits" <<endl;
		PrintBits(8,&m_saccu);
	}
	
	if(exponent>=0)
	{
		shift=(uint16_t)exponent;
		m_saccu<<=shift;
	}
	else
	{
		shift=(uint16_t)abs(exponent);
		m_saccu>>=shift;
	}
	
	if(m_verbose) 
	{
		cout<<"Shift of "<< exponent << " Bits" <<endl;
		PrintBits(8,&m_saccu);
	}
}


Accumulator64B::~Accumulator64B()
{
	
}


Accumulator64B Accumulator64B::operator+(const Accumulator64B& v1) 
{
	Accumulator64B result;
        //int32_t sum=this->Get32BValue()+v1.Get32BValue();
        //if(sum<-8388608 || sum>8388607) cout<<"OVERFLOW!!"<<endl;
	result.m_saccu = this->m_saccu+v1.m_saccu;
	result.m_value = this->m_value+v1.m_value;
	
        return result;
}


Accumulator64B Accumulator64B::operator-(const Accumulator64B& v1) 
{
        Accumulator64B result;
        result.m_saccu = this->m_saccu-v1.m_saccu;
        result.m_value = this->m_value-v1.m_value;
        
	return result;
}


int32_t Accumulator64B::Get32BValue()
{
	int64_t temp=m_saccu;
	temp>>=m_RadixPoint;
	m_32IntVal=(int32_t)temp;
	
	if(m_RoundingCorrection) this->RoundingToNearest();
	
	int32_t temp32=m_32IntVal;
	
	return temp32;
}


double Accumulator64B::GetRealValue()
{
	double temp=m_value;
	return temp;
}


void Accumulator64B::RoundingToNearest()
{
	uint64_t FractionalPart=(uint64_t)m_saccu;
	uint16_t FracShift=64-m_RadixPoint;
	
	FractionalPart<<=FracShift;
	FractionalPart>>=FracShift;
	if( FractionalPart>=m_RoundingThreshold) m_32IntVal+=1;
	
}

void Accumulator64B::PrintValue()
{
	cout<<"--- Print Values --- " <<endl;
	cout<<"real value (double) = "<<m_value<<endl;
	this->Get32BValue();
	if(m_RoundingCorrection) cout<<"Rounding to the nearest number : option ON"<<endl;
	else cout<<"Rounding to the nearest number : option OFF"<<endl;
	cout<<"signed Value (32 bit integer) = "<<m_32IntVal<<endl;
}



void Accumulator64B::PrintBits(size_t const size, void const * const ptr) //assuming little endian
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i=size-1;i>=0;i--)
    {
        for (j=7;j>=0;j--)
        {
            byte = b[i] & (1<<j);
            byte >>= j;
            printf("%u", byte);
        }
    }
    puts("");
}




void Accumulator64B::Init()
{
	m_verbose=false;
	m_RoundingCorrection=true;
	m_saccu=0;
	m_value=0;
	m_32IntVal=0;
	m_RadixPoint=40;// =32 for 32 bit output; =40 for 24 bit output; =48 for 16 bit output; =64-Nb for Nb bit output
	m_RoundingThreshold=TMath::Power(2,(double)m_RadixPoint-1);
}


////////////////////////////////////////////////////////////////////  END /////////////////////////////////////////////////////////////////////////////////////


