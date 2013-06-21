/*
 * =====================================================================================
 *
 *       Filename:  svd_model.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2013年05月26日 15时41分45秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
ModelParms::ModelParms(double _totalAvg, int uSize, int iSize,int _factorNum,
		double _alpha, double _beta):
	totalAvg(_totalAvg), 
	bu(uSize, 0),
	bi(iSize, 0),
	factorNum(_factorNum),
	pu(uSize, vector<double>(factorNum)), 
	pi(iSize, vector<double>(factorNum)),
	alpha(_alpha), 
	beta(_beta)
{
	slowRate = 1;
	assert(pu.size() == uSize);
	assert(pi.size() == iSize);
	assert(pu[0].size() == factorNum);
	assert(pi[0].size() == factorNum);
	srand((unsigned long)time(NULL));
	for( int i = 0; i < pu.size() ; i++ )
		std::for_each(pu[i].begin(), pu[i].end(), RandomValue(factorNum));
	assert(pu[0][0] < 1);
	for( int i = 0; i < pi.size() ; i++ )
		std::for_each(pi[i].begin(), pi[i].end(), RandomValue(factorNum));
	assert(pi[0][0] < 1);
}


void ModelParms::initModel(vector<vector<int> >& R)
{
	int uSize = R.size();
	int iSize = R[0].size();
	for( int j = 0; j < iSize  ; j++ )
	{
		int rateCount4Item = 0;
		for( int i = 0; i < uSize ; i++ )
		{
			if( R[i][j] > 0 )
			{
				rateCount4Item++;
				bi[j] += (R[i][j] - totalAvg);
			}
		}
		if( rateCount4Item >= 1 )
			bi[j] = bi[j] / (rateCount4Item + 25);
		else
			bi[j] = 0.0;
	}

	for( int i = 0; i < uSize ; i++ )
	{
		int rateCount4User = 0;
		for( int j = 0; j < iSize ; j++ )
		{
			if( R[i][j] > 0 )
			{
				rateCount4User++;
				bu[i] += (R[i][j] - totalAvg - bi[j]);
			}
		}
		if( rateCount4User >= 1 )
			bu[i] = bu[i] / (rateCount4User + 10);
		else
			bu[i] = 0.0;
	}
}


double ModelParms::getPredict(const int& userId, const int& itemId)
{
	double predictRate = totalAvg + bu[userId] + bi[itemId] + 
		inner_product(pu[userId].begin(), pu[userId].end(), pi[itemId].begin(), 0);
	return predictRate;
}


void ModelParms::setBu(const int& userId, const double& eui)
{
	bu[userId] += alpha * (eui - beta * bu[userId]);
}

void ModelParms::setU(const double& eui)
{
	totalAvg += alpha * (eui - beta * totalAvg);
}


void ModelParms::setBi(const int& itemId, const double& eui)
{
	bi[itemId] += alpha *(eui - beta * bi[itemId]);
}

void ModelParms::setPUI(const int& userId, const int& itemId,  const double& eui)
{
	for( int i = 0; i < factorNum ; i++ )
	{
		double temp = pu[userId][i];
		pu[userId][i] += alpha * (eui * pi[itemId][i] -  beta * pu[userId][i]);
		pi[itemId][i] += alpha * (eui * temp - beta * pi[itemId][i]);
	}
}


double ModelParms::getExtra(const int& userId, const int& itemId)
{
	double sum = 0;
	//sum += pow(bu[userId], 2) + pow(bi[itemId], 2);
	for( int i = 0; i < factorNum ; i++ )
	{
		sum += pow(pu[userId][i], 2) + pow(pi[itemId][i], 2);
	}
	return (beta)*sum;
}


void ModelParms::updateAlpha()
{
	alpha *= slowRate;
}
