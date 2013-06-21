#ifndef _MODEL_PARMS_
#define _MODEL_PARMS_
#include<assert.h>
#include<stdio.h>
#include<math.h>
#include<time.h>
#include<math.h>
#include<stdlib.h>
#include<numeric>
#include<algorithm>
#include<queue>
#include<set>
using namespace std;

class RandomValue
{
	public:
		RandomValue(int _factorNum):factorNum(_factorNum){}
		void operator()(double& value)
		{
			double temp = sqrt(factorNum);
			double temp1 = (rand()/ (double)RAND_MAX);
			value = 0.1 * temp1 / temp;
		}
	private:
		int factorNum;
};



class ModelParms
{
	private:
		double totalAvg;
		vector<double>bu;
		vector<double>bi;
		int factorNum;
		vector<vector<double> >pu;
		vector<vector<double> >pi;
		double alpha;
		double beta;
		double slowRate;
	public:
		ModelParms(double _totalAvg, int uSize, int iSize,int _factorNum,
				double _alpha, double _beta);
		void initModel(vector<vector<int> >& R);
		double getPredict(const int& userId, const int& itemId);
		void setBu(const int& userId, const double& eui);
		void setU(const double& eui);
		void setBi(const int& itemId, const double& eui);
		void setPUI(const int& userId, const int& itemId,  const double& eui);
		double getExtra(const int& userId, const int& itemId);
		void updateAlpha();
}; 


#include"svd_model.cpp"
#endif
