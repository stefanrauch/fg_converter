#include "TXTDATA.h"

// Constructors/Destructors
//  

TXTDATA::TXTDATA ( ) 
{
	
}
/*

TXTDATA::TXTDATA ( const CONFIG configfile) : DATA(configfile)
{
	initAttributes(m_FileName);
}


TXTDATA::TXTDATA (, const CONFIG configfile) : DATA(configfile)
{
	m_FileName=filename;
	initAttributes(m_FileName);
}
*/


TXTDATA::TXTDATA (string filename) 
{
	m_FileName=filename;
	initAttributes(m_FileName);
}
//*
TXTDATA::~TXTDATA ( ) 
{ 
	//cout<<"TXTDATA destructor"<<endl;
	/*
	delete m_Tree;
	delete m_1Dhisto;
	delete m_2Dhisto;
	
	m_Tree=0;
	m_1Dhisto=0;
	m_2Dhisto=0;
	//*/
}

//  
// Methods
//  


// Accessor methods
//  


// Other methods
//  


void TXTDATA::ReadFile ( ) {

}


void TXTDATA::ReadFile (string FileName ) {

}



void TXTDATA::PrintFile ( )
{
	for(unsigned int i(0);i<m_Vector.size();++i)
        {
            for(unsigned int j(0);j<m_Vector[i].size();++j)  
		    cout<<m_Vector[i][j]<<" ";
	    cout<<endl;
	}
}

TH1D* TXTDATA::Get1Dhisto ( ) {
	
	return m_1Dhisto;
}        
        



void TXTDATA::Fillmy1Dhisto (TH1D* h, double unit)
{
	double Xvar;
	for(unsigned int i(0); i<m_Vector.size();++i)
	{
		Xvar=unit*m_Vector[i][0];
		h->Fill(Xvar);
		
	}
}


TH2D* TXTDATA::Get2Dhisto ( ){
	
	return m_2Dhisto;
}
        
	
TTree* TXTDATA::GetTree ( ){
	
	return m_Tree;
}
        


vector< vector<double> > TXTDATA::GetVector ( ){
	
	return m_Vector;
}



void TXTDATA::PrintVector()
{
	for(unsigned int i(0);i<m_Vector.size();++i)
        {
            for(unsigned int j(0);j<m_Vector[i].size();++j)  
		    cout<<m_Vector[i][j]<<" ";
	    cout<<endl;
	}
}

void TXTDATA::initAttributes ( ){
	
	
}



void TXTDATA::initAttributes(const string filename)
{
	
	
        string ligne;
        ifstream fichier(filename.c_str());
        m_Statut=true;
	
	
	/////////////////////////////////////////////////////////////////////////
	/// Read and store file containt 
	/////////////////////////////////////////////////////////////////////////
        if (!fichier.is_open())
        {
            cout<<"File "<< filename.c_str() <<" cannot be read"<<endl;
            m_Statut=false;
        }

        while (getline(fichier, ligne))
        {
            if(!ligne.empty())
            {
                vector<string> tableau;
                typedef tokenizer<char_separator<char> > tokenizer;
                char_separator<char> sep("\t ;,|" );
                tokenizer tokens(ligne,sep);
                tokenizer::iterator tok_iter = tokens.begin();
                tok_iter = tokens.begin();
                for(; tok_iter != tokens.end();++tok_iter)
                {
                    tableau.push_back(lexical_cast<string>((*tok_iter).c_str()));
                }
                m_FileContent.push_back(tableau);
            }
        }
        fichier.close();
	
	
	/////////////////////////////////////////////////////////////////////////
	/// convert file containt into numbers and fill vector
	/////////////////////////////////////////////////////////////////////////
	
	for(unsigned int i(0);i<m_FileContent.size();++i)
        {
		//cout<<"m_FileContent : "<< m_FileContent[i][0] <<endl;
		vector<double> vtemp;
		for(unsigned int j(0);j<m_FileContent[i].size();++j)
			if(is_number(m_FileContent[i][j]))
				vtemp.push_back(lexical_cast<double>(m_FileContent[i][j]));
			else cout<<"Line "<<i<<"  "<<m_FileContent[i][j]<<" is not a Number"<<endl;
		if(vtemp.size()>0) m_Vector.push_back(vtemp);
	}
	
}






// Other function



bool is_number(const string& s)
{
    bool is_a_number = false;
    try
    {
        lexical_cast<double>(s);
        is_a_number = true;
    }
    catch(bad_lexical_cast &)
    {
        // if it throws, it's not a number.
    }
    return is_a_number;
}






//*/


