#ifndef cal_h
#define cal_h

#include "stock.h"
#include <cstdlib>
#include <ctime>

#define SAMPLE_SIZE 60  // sample 60 stocks for each group
#define REPEAT 30       // repeat calculation for 30 times
using namespace std;

class SPYBase {
	friend class AllGroupRepeatCal;
private:
	map<string, double> SPY_R;

	SPYBase() = default;

	~SPYBase() = default;

	SPYBase(map<string, double> &SPY) {
		auto itr = SPY.begin();
		double lastPrice = itr->second;
		for (++itr; itr != SPY.end(); ++itr) {  // first item of SPY_R is price
			// SPY_R[t] = (P_t - P_{t-1}) / P_{t-1}
			SPY_R[itr->first] = (itr->second - lastPrice) / lastPrice;
			lastPrice = itr->second;
		}
	}
public:
	const map<string, double> &getSPYR() { return SPY_R; }
};

class OneGroupCal {
	friend class AllGroupRepeatCal;
private:
	vector<vector<double> > stocksMtx, SPYMtx;
	vector<bool> sampleVct;
	vector<double> AAR, CAAR;

	OneGroupCal() = default;

	~OneGroupCal() = default;

	OneGroupCal(SPYBase &SPY, map<string, stock> &dataMap) {
		sampleVct.resize(dataMap.size());
		AAR.resize(DATA_SIZE, 0);
		CAAR.resize(DATA_SIZE, 0);
		vector<double> tempVctD;
		for (auto itr = dataMap.begin(); itr != dataMap.end(); ++itr) {  // go through pricemap
			// stocksMtx
			tempVctD = itr->second.getPrice();
			for (int i = 0; i < DATA_SIZE; ++i)
				// stocks_R[t] = (P_{t+1} - P_t) / P_t
				tempVctD[i] = (tempVctD[i + 1] - tempVctD[i]) / tempVctD[i];
			tempVctD.resize(DATA_SIZE);
			stocksMtx.push_back(tempVctD);
			// SPYMtx
			tempVctD.clear();
			auto iitr = SPY.getSPYR().find(itr->second.getStart());  // iitr: SPY
			++iitr;  // match index for SPY_R and stocks_R
			for (int i = 0; i < DATA_SIZE; ++i, ++iitr)
				tempVctD.push_back(iitr->second);
			SPYMtx.push_back(tempVctD);
		}
	}

	void randomVector() {
		int len = sampleVct.size(), randNum;
		sampleVct.assign(len, FALSE);
		int count = 0;
		while (count < SAMPLE_SIZE) {
			randNum = rand() % len;
			if (!sampleVct[randNum]) {
				sampleVct[randNum] = TRUE;
				++count;
			}
		}
	}

	void sampleMtxCalAAR() {
		auto itrV = sampleVct.begin();
		auto itrMstk = stocksMtx.begin(), itrMspy = SPYMtx.begin();
		for (; itrV != sampleVct.end(); ++itrV, ++itrMspy, ++itrMstk)
			if (*itrV)
				AAR += *itrMstk - *itrMspy;  // AAR = sum(AR) = sum(-)
		AAR /= SAMPLE_SIZE;
	}

	void calculateOnce() {
		randomVector();
		AAR.assign(DATA_SIZE, 0);
		sampleMtxCalAAR();  // AAR get
		CAAR[0] = AAR[0];
		for (int j = 1; j < DATA_SIZE; ++j)
			CAAR[j] = CAAR[j - 1] + AAR[j];  // CAAR get
	}
};

class AllGroupRepeatCal{
private:
	SPYBase preCalSPY;
	OneGroupCal calBeat, calMeet, calMiss;
	vector<vector<vector<double> > > rslt;
	/*             rslt
	*           vector ^ 3
	*    +----------+-----------+
	*    | beat AAR | beat CAAR |
	*    |  vector  |   vector  |
	*    +----------+-----------+
	*    | meet AAR | meet CAAR |
	*    |  vector  |   vector  |
	*    +----------+-----------+
	*    | miss AAR | miss CAAR |
	*    |  vector  |   vector  |
	*    +----------+-----------+
	*/
public:
	AllGroupRepeatCal() = default;

	~AllGroupRepeatCal() = default;

	AllGroupRepeatCal(map<string, double> &SPY, map<string, stock> &beat, map<string, stock> &meet, map<string, stock> &miss) :
		preCalSPY(SPY), calBeat(preCalSPY, beat), calMeet(preCalSPY, meet), calMiss(preCalSPY, miss) {
		rslt.resize(3);
		rslt[0].resize(2);
		rslt[1].resize(2);
		rslt[2].resize(2);
		rslt[0][0].resize(DATA_SIZE, 0);
		rslt[0][1].resize(DATA_SIZE, 0);
		rslt[1][0].resize(DATA_SIZE, 0);
		rslt[1][1].resize(DATA_SIZE, 0);
		rslt[2][0].resize(DATA_SIZE, 0);
		rslt[2][1].resize(DATA_SIZE, 0);
	}

	void calculateAll() {
		srand((unsigned)time(NULL));
		for (int i = 0; i < REPEAT; ++i) {
			calBeat.calculateOnce();
			rslt[0][0] += calBeat.AAR;
			rslt[0][1] += calBeat.CAAR;
			calMeet.calculateOnce();
			rslt[1][0] += calMeet.AAR;
			rslt[1][1] += calMeet.CAAR;
			calMiss.calculateOnce();
			rslt[2][0] += calMiss.AAR;
			rslt[2][1] += calMiss.CAAR;
		}
		rslt[0][0] /= REPEAT;
		rslt[0][1] /= REPEAT;
		rslt[1][0] /= REPEAT;
		rslt[1][1] /= REPEAT;
		rslt[2][0] /= REPEAT;
		rslt[2][1] /= REPEAT;
	}

	const vector<vector<vector<double>>> &getRslt() { return rslt; }
};

#endif // cal_h
