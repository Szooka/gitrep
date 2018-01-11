#ifndef __MATRIX_H__
#define __MATRIX_H__
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <iostream>
using namespace std;

class CMatrix{
	struct rcmatrix;
	rcmatrix* data;
public:
	class IndexOutOfRange{};
	class WrongDim{};
	class bad_allocc;
	class Cref1;
	class Cref2;
	CMatrix(int, int, double, double);
	CMatrix(fstream&);
	CMatrix(const CMatrix&);
	~CMatrix();
	CMatrix& operator= (const CMatrix &temp);
	friend CMatrix operator* (const CMatrix&, const CMatrix&);
	Cref1 operator[] (int);
	double read(int i, int j) const;
	void write(int i, int j, double x);
	

	friend ostream & operator << (ostream & s, const CMatrix & temp);

};

struct CMatrix::rcmatrix{
	int rows;
	int cols;
	int n;
	double **matrix;
	
	rcmatrix(int i, int j, double a, double b): rows(i), cols(j)
	{
		n=1;
		matrix = matrix_alloc(i,j);
		for(int k = 0; k < rows; k++){
			for(int l = 0; l < cols; l++){
				if(k == l)
					this->matrix[k][l] = a;
				else
					this->matrix[k][l] = b;
			}
		}
	}

	rcmatrix(fstream & s)
	{
		n=1;
		int toInt;
		double toDouble;

		s >> toInt;
		rows = toInt;

		s >> toInt;		
		cols = toInt;

		matrix = matrix_alloc(rows,cols);

		// Przypisania 
		for(int k = 0; k < rows; k++){
			for(int l = 0; l < cols; l++){
					s >> toDouble;
					this->matrix[k][l] = toDouble;
			}
		}
	}

	rcmatrix(const rcmatrix& temp): rows(temp.rows), cols(temp.cols)
	{
		int k = 0;
		int l = 0;
		n=1;
		matrix = matrix_alloc(temp.rows,temp.cols);
		for(k = 0; k < rows; k++){
			for(l = 0; l < cols; l++){
				matrix[k][l] = temp.matrix[k][l];		
			}
		}
	}

  ~rcmatrix()
	{
		int k = 0;
		for(k=0; k<rows; k++){
			delete [] matrix[k];	
		}
		delete [] matrix;
	}
	
	rcmatrix* detach()											// Detach
	{
		if(n==1)
			return this;

		rcmatrix* t=new rcmatrix(*this);	
		n--;
		return t;
	}

	double** matrix_alloc(int ii, int jj){
		int k=0;
		double **matrix = new double*[ii];
		try
		{
			for(k = 0; k<ii; k++)
      	matrix[k] = new double[jj];
			return matrix;
			
		}catch (bad_alloc&)
		{
			for(int i = k-1; i >= 0; i--)
				delete [] matrix[k];

			delete [] matrix;

			throw bad_alloc();
		}
	}
};

class CMatrix::Cref2											// Cref2
	{
	
  	CMatrix& s;
  	int i;
		int j;
	public:
  	Cref2 (CMatrix& ss, int ii, int jj): s(ss), i(ii), j(jj) {};

 	Cref2& operator= (double x) 
  	{
    		s.write(i,j,x);
		return *this;
   	};
	operator double()
	{
		return s.read(i,j);
	}
};

	class CMatrix::Cref1											// Cref1
	{
		friend class CMatrix;
  	CMatrix& s;
  	int i;
	public:
  	Cref1 (CMatrix& ss, int ii): s(ss), i(ii) {};

 	 	Cref2 operator [] (int j) 
 	 	{
			if(j > s.data->cols) throw CMatrix::IndexOutOfRange();
  		return Cref2(s, i, j);
  	}
	
	};

CMatrix::Cref1 CMatrix::operator[](int i)
	{
		if(i > data->rows) throw IndexOutOfRange();
		return Cref1(*this, i);
	}


CMatrix::CMatrix(int i, int j, double a = 0.0, double b = 0.0)
{
	data = new rcmatrix(i,j,a,b);
}
CMatrix::CMatrix(fstream & s)
{
	data = new rcmatrix(s);
}

CMatrix::CMatrix(const CMatrix& temp)
{
	temp.data->n++;
	data = temp.data;
}

CMatrix::~CMatrix()
{
  if(--data->n==0)
    delete data;
}


CMatrix& CMatrix::operator= (const CMatrix &temp)
{
	temp.data->n++;
	if(--data->n == 0 )
		delete data;

	data = temp.data;
	return *this;
}

inline CMatrix
operator * (const CMatrix& s1, const CMatrix& s2)
{
	if(s1.data->cols != s2.data->rows) throw CMatrix::WrongDim();
	else{

		CMatrix temp(s1.data->rows, s2.data->cols);
		for(int k = 0; k < s1.data->rows; k++){
			for(int l = 0; l < s2.data->cols; l++){
					for(int m = 0; m < s1.data->cols; m++){
							temp.data->matrix[k][l] += s1.data->matrix[k][m] * s2.data->matrix[m][l];		
					} 
				}
		}
		return temp;
	}
}

double CMatrix::read(int i, int j) const
{
	return data->matrix[i][j];
}

void CMatrix::write(int i, int j, double x)
{
	data = data->detach();
	data->matrix[i][j] = x;
}

ostream & operator << (ostream & s, const CMatrix & temp)
  {
		for(int i = 0; i < temp.data->rows; i++){
			for(int j = 0; j < temp.data->cols; j++){
				s << temp.read(i,j) << "  " ;
			}
			s << "\n";
		}   
		
    return s;
  }

#endif /* __MATRIX_H__ */
