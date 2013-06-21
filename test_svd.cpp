/*
 * =====================================================================================
 *
 *       Filename:  uv.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2013¿?05¿?18¿? 00¿?18¿?44¿?
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include<iostream>
#include<vector>
#include<algorithm>
#include<fstream>
#include<string>
#include<sstream>
#include"svd_model.h"
using namespace std;
/*
 * movieLen 10M : ITERM_SIZE-10681 ; USER_SIZE-71567
 * movieLen 100k: ITERM_SIZE-1682 ; USER_SIZE-943
 */
struct Node{
	int id;
	double value;
};
const int ITERM_SIZE= 1682;
const int USER_SIZE= 943;
const int STEPS = 5000;
const int random_size = 100;
const int top_k = 10;
//the M matrix
inline double myround(double num);
void readFile(const char* fileName,vector<vector<int> >& R );
double getTotalAvg(const vector<vector<int> >& R);
double getMSE(const vector<vector<int> >&R, ModelParms& model);
void svd(ModelParms& model,const vector<vector<int> >&R);
void ReadTestFile(const char* fileName, const char* outPutFile, ModelParms& model);

int main(int argc, char *argv[])
{
	assert(argv[1] != NULL);
	//normalize();
	vector<vector<int> >r(USER_SIZE, vector<int>(ITERM_SIZE, 0));
	readFile(argv[1], r);
	assert(USER_SIZE == r.size());
	assert(ITERM_SIZE == r[0].size());

	double totalAvg = getTotalAvg(r);
	cout << totalAvg << endl;
	assert(totalAvg > 0 && totalAvg <5);
	ModelParms model(totalAvg,USER_SIZE, ITERM_SIZE, 2, 0.002, 0.02 );
	model.initModel(r); //NOTE
	svd(model, r);
	assert(argv[2] != NULL && argv[3] != NULL);
	ReadTestFile(argv[2],argv[3] , model);

	return 0;
}

inline double myround(double num)
{
	double num1 = num - floor(num);
	double num2 = ceil(num) - num;

	if (num1 > num2)
		return ceil(num);
	else
		return floor(num);
}


void readFile(const char* fileName,vector<vector<int> >& R )
{
	ifstream ifs(fileName);
	string line;
	int userId = 0, itemId, rate;
	char ch;
	int lineCount = 0;
	int testNotZero = 0;
	cout << "readFile begin ........" << endl;
	while( getline(ifs,line) )
	{
		istringstream strstream(line);
		strstream >> userId >> itemId >> rate;
		assert(rate >= 1 && rate <= 5);
		R[userId-1][itemId-1] = rate;
		lineCount++;
		line.clear();
	}
	cout <<" readFile lineCount:" << lineCount << endl;
	ifs.close();
}



double getTotalAvg(const vector<vector<int> >& R)
{
	int uSize = R.size();
	int iSize = R[0].size();
	int sum = 0, rateCount = 0;
	for( int i = 0; i < uSize ; i++ )
	{
		for( int j = 0; j < iSize ; j++ )
		{
			assert(R[i][j] >= 0 && R[i][j] <= 5);
			if( R[i][j] > 0 )
			{
				sum += R[i][j];
				rateCount++;
			}
		}
	}
	cout << rateCount << endl;
	return (double)sum/rateCount;
}


double getMSE(const vector<vector<int> >&R, ModelParms& model)
{
	int uSize = R.size();
	int iSize =  R[0].size();
	double sum = 0;
	long rateCount = 0;
	for( register int i = 0; i < uSize ; i++ )
	{
		for( register int j = 0; j < iSize ; j++ )
		{
			if( R[i][j] > 0 )
			{
				rateCount++;
				double predictRate = model.getPredict(i ,j);
				sum += pow(predictRate - R[i][j], 2);
				//sum += model.getExtra(i,j);
			}
		}
	}
	//cout << rateCount << endl;
	return (sum / rateCount);
}

void svd(ModelParms& model,const vector<vector<int> >&R)
{
	int totalSteps = 2000;
	int uSize = R.size();
	int iSize = R[0].size();
	double lastMse = 10000000.0;
	int bigThan = 0;
	//double totalAvg = getTotalAvg(R);
	for( int step = 0; step < totalSteps; step++)
	{
		for( int i = 0; i < uSize ; i++ )
		{
			for( int j = 0; j < iSize ; j++ )
			{
				if( R[i][j] == 0 )
					continue;
				double predictRate = model.getPredict(i ,j);
				double eij = R[i][j] - predictRate;
				model.setBu(i,eij);
				model.setBi(j, eij);
				model.setPUI(i, j, eij);
			}
		}
		double mse = getMSE(R, model);
		cout << mse << endl;
		if( mse > lastMse )
		{
			break;
		}
		lastMse = mse;
	}
}

bool cmp(Node a, Node b)
{
	return a.value > b.value;
}

void ReadTestFile(const char* fileName, const char* outPutFile, ModelParms& model)
{
	ifstream ifs(fileName);
	ofstream ofs(outPutFile);
	string line;
	int userId, itemId, rate;
	char ch;
	long lineCount = 0;
	int hits = 0;
	while( getline(ifs,line) )
	{
		istringstream strstream(line);
		strstream >> userId >> itemId ; 
		Node predict_result[random_size + 1];
		for( int i = 0; i < random_size + 1 ; i++ )
		{
			int no_rate_item;
			strstream >> no_rate_item;
			double predictRate = model.getPredict(userId -1, no_rate_item - 1);
			predict_result[i].id = no_rate_item;
			predict_result[i].value = predictRate;
		}
		sort(predict_result, predict_result + random_size + 1, cmp);
		bool find = false;
		for( int i = 0; i < top_k ; i++ )
		{
			if( predict_result[i].id == itemId )
			{
				find = true;
				break;
			}
		}
		if( find == true )
		{
			cout << "hit"<<endl;
			hits++;
		}
		else
		{
			cout << "not hit" << endl;
		}
		lineCount++;
		line.clear();
	}
	cout << lineCount << endl;
	cout << "hits: "<<hits << endl;
	ofs << (double) hits / lineCount << endl;
	ifs.close();
	ofs.close();
}
