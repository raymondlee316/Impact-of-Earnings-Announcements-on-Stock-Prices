#ifndef menu_h
#define menu_h
#include "stdafx.h"
#include "stock.h"
#include "cal.h"
#include "download.h"
#include <cstdlib>
#include <map>
#include <vector>
#include <string>
#include <list>
#include <iostream>
#include <thread>
#include "ExcelDriver.hpp"
#include "VectorsAndMatrices\numericMatrix.cpp" // Template
#include "BitsAndPieces\StringConversions.hpp"
#include "ExceptionClasses\DatasimException.hpp"
#include "ExcelMechanisms.hpp"

using namespace std;

vector<vector<vector<double> > > downloadData(map<string, stock> & Data_beat, map<string, stock> & Data_meet, map<string, stock> & Data_miss, map<string,double>&Data_SPY) {
	cout << "Retrieve historical price data for all stocks." << endl;
	thread thBeat(yahoo, ref(Data_beat));
	Sleep(2000);
	thread thMeet(yahoo, ref(Data_meet));
	Sleep(2000);
	thread thMiss(yahoo, ref(Data_miss));
	Sleep(2000);
	thread thSPY(yahoo_SPY, ref(Data_SPY));
	thBeat.join();
	thMeet.join();
	thMiss.join();
	thSPY.join();
	AllGroupRepeatCal calAll(Data_SPY, Data_beat, Data_meet, Data_miss);
	calAll.calculateAll();
	return calAll.getRslt();
}

void pullstockinfo(map<string, stock> & Data_beat, map<string, stock> & Data_meet, map<string, stock> & Data_miss)
{
	cout << "Pull information for the stock you choose" << endl;
	cout << "Input stockname:" << endl;
	string stockname;
	cin >> stockname;
	auto it1 = Data_beat.find(stockname);
	auto it2 = Data_meet.find(stockname);
	auto it3 = Data_miss.find(stockname);
	if (it1 != Data_beat.end()) std::cout << stockname << "is in group beat." << it1->second << endl;
	else if (it2 != Data_meet.end()) std::cout << stockname << "is in group meet." << it2->second << endl;
	else if (it3 != Data_miss.end()) std::cout << stockname << "is in group miss." << it3->second << endl;
	else cout << "The stock you enter is not in the list." << endl;
}

void showAAR(const vector<vector<vector<double>>>& Result)
{
	cout << "Show AAR or CAAR for one group you choose" << endl;
	int groupname, dataname;
	string buf;
	cout << "Choose group beat: Input 1" << endl;
	cout << "Choose group meet: Input 2" << endl;
	cout << "Choose group miss: Input 3" << endl;
	while (true) {
		cin >> buf;
		if (buf.size() != 1 || buf[0] > '3' || buf[0] < '1') {
			cout << "invalid input, try again ...\n";
			continue;
		}
		groupname = buf[0] - '0';
		break;
	}

	cout << "Show AAR: Input 1" << endl;
	cout << "Show CAAR: Input 2" << endl;
	while (true) {
		cin >> buf;
		if (buf.size() != 1 || buf[0] > '2' || buf[0] < '1') {
			cout << "invalid input, try again ...\n";
			continue;
		}
		dataname = buf[0] - '0';
		break;
	}
	NumericMatrix<double, long> matrixBad(1, 90);
	for (long i = 1; i <= 90; i++)
		matrixBad(1, i) = Result[groupname - 1][dataname - 1][i - 1];
	Vector<double, long> xarr(1);
	Vector<double, long> yarr(90);
	xarr[1] = 1;
	yarr[yarr.MinIndex()] = 1;
	for (long i = yarr.MinIndex() + 1; i <= yarr.MaxIndex(); i++)
		yarr[i] = yarr[i - 1] + 1;
	vector<vector<string>>sheetname;
	sheetname.resize(2);
	for (int i = 0; i<2; i++)
		sheetname[i].resize(3);
	sheetname[0][0] = "beat AAR";
	sheetname[0][1] = "meet AAR";
	sheetname[0][2] = "miss AAR";
	sheetname[1][0] = "beat CAAR";
	sheetname[1][1] = "meet CAAR";
	sheetname[1][2] = "miss CAAR";
	printMatrixInExcel(matrixBad, xarr, yarr, sheetname[dataname - 1][groupname - 1]);

}

void showgraph(const vector<vector<vector<double>>>& Result)
{
	Vector<double, long> xarr2(90);
	Vector<double, long> ybeat(90);
	Vector<double, long> ymeet(90);
	Vector<double, long> ymiss(90);

	xarr2[xarr2.MinIndex()] = -59.0;
	for (long i = xarr2.MinIndex() + 1; i <= xarr2.MaxIndex(); i++)
	{
		xarr2[i] = xarr2[i - 1] + 1;
	}
	for (long i = ybeat.MinIndex(); i <= ybeat.MaxIndex(); i++)
	{
		ybeat[i] = Result[0][1][i - 1];
		ymeet[i] = Result[1][1][i - 1];
		ymiss[i] = Result[2][1][i - 1];
	}
	cout << "Show the Excel graph with CAAR for all 3 groups" << endl;
	const list<string> labels = { "beat", "meet", "miss" };
	const list<Vector<double, long>> yValues = { ybeat, ymeet, ymiss };
	printInExcel(xarr2, labels, yValues, "CAAR", "Days", "Return");

}

#endif
