#ifndef TXTDATA_H
#define TXTDATA_H
//#include "DATA.h"

#include <string>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <dirent.h>
#include <errno.h>
#include "rootheaders.hh"


#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>



using namespace std;
using namespace boost;

/**
  * class TXTDATA
  */

//class TXTDATA : virtual public DATA
class TXTDATA 
{
public:

	// Constructors/Destructors
	//  


	/**
	 * Empty Constructor
	 */
	TXTDATA ( );

        //TXTDATA (string filename, const CONFIG configfile);
        //TXTDATA ( const CONFIG configfile);
	TXTDATA (string filename);
	/**
	 * Empty Destructor
	 */
	virtual ~TXTDATA ( );

	// Static Public attributes
	//  

	// Public attributes
	//  


	// Public attribute accessor methods
	//  


	// Public attribute accessor methods
	//  



	/**
	 */
	void ReadFile ( );


	/**
	 * @param  FileName
	 */
	void ReadFile (string FileName );

        
        void PrintFile ( );
        
        /**
         */
        void PrintVector();
        
        
        /**
	 */
	
	//virtual TH1D* Get1Dhisto ( );        
        TH1D* Get1Dhisto ( ); 
	void Fillmy1Dhisto (TH1D* h, double unit);
        
        /**
	 */
	//virtual TH2D* Get2Dhisto ( );
        TH2D* Get2Dhisto ( );
        /**
	 */
	//virtual TTree* GetTree ( );
        TTree* GetTree ( );

        /**
	 */
	//virtual vector< vector<double> > GetVector ( );        
        vector< vector<double> > GetVector ( );
        
        //virtual TGraph* SetGraph();
        

        /**
	 */
        
protected:

	// Static Protected attributes
	//  

	///////////////////////////////////////
        // Protected attributes (PARENT CLASS)
        // TH1D* m_1Dhisto;
        // TH2D* m_2Dhisto;
        // TTree* m_Tree;
        // vector< vector<double> > m_Vector;
        // CONFIG m_configfile;
        ///////////////////////////////////////
        
        // Protected attributes
	//  
        
        
	// Protected attribute accessor methods
	//  


	// Protected attribute accessor methods
	//  

        
private:

	// Static Private attributes
	//  

	// Private attributes
	//  
	TH1D* m_1Dhisto;
	TH2D* m_2Dhisto;
	TTree* m_Tree;
	vector< vector<double> > m_Vector;
        bool m_Statut;
	string m_FileName;
        vector< vector<string> > m_FileContent;
	// Private attribute accessor methods
	//  


	// Private attribute accessor methods
	//  
        
        void initAttributes();
        void initAttributes(const string filename);

};









/// ///////////////////////// other functions///////////
bool is_number(const std::string& s);



#endif // TXTDATA_H
