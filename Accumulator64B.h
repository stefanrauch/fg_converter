#ifndef ACCUMULATOR64B_H
#define ACCUMULATOR64B_H

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "TMath.h"
#include <stdint.h>
#include <iostream>

using namespace std;


class Accumulator64B 
{
public:

        Accumulator64B();
        Accumulator64B(const Accumulator64B &Accu);
	Accumulator64B(double input);
        Accumulator64B(int32_t input);
        Accumulator64B(int16_t significand, int16_t exponent);
	~Accumulator64B();
        
        Accumulator64B operator+(const Accumulator64B& v1);
	Accumulator64B operator-(const Accumulator64B& v1);
        
        void PrintValue();
        int32_t Get32BValue();// in fact return the 24 bit output in a 32 bit variable
	double GetRealValue();
        
private:     
    
    void Init();
    void PrintBits(size_t const size, void const * const ptr);
    void RoundingToNearest();
    
    // option
    bool m_verbose;
    bool m_RoundingCorrection;
    uint64_t m_RoundingThreshold;
    
    //input
    double m_value;
    int16_t m_sig16;
    int16_t m_exp16;
    
    //Accumulator 64 bits signed integer (2's complement)
    int64_t m_saccu;
    uint16_t m_RadixPoint;// define a radix point on the accumulator
    
    //output
    int32_t m_32IntVal;
    
    
    
};


#endif 
