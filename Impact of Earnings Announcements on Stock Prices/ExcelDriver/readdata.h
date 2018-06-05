#ifndef readdata_h
#define readdata_h
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "stock.h"
//#define INPUT_FILEPATH "BM_EPS_test.csv"
#define INPUT_FILEPATH "BM_EPS.csv"

void readdata(map<string, stock> &Data_beat, map<string, stock> &Data_meet, map<string, stock> &Data_miss) {
	string est, act, srp, name, ann, start, end;
	vector<double> price;

	// read from csv
	ifstream SP500List(INPUT_FILEPATH);
	getline(SP500List, name);  // skip header
	while (!SP500List.eof()) { //end of file
		getline(SP500List, name, ',');
		if (name == "")	continue;
		getline(SP500List, ann, ',');
		getline(SP500List, act, ',');
		getline(SP500List, est, ',');
		getline(SP500List, srp, ',');
		getline(SP500List, start, ',');
		getline(SP500List, end);
		stock stockTemp(atof(est.c_str()), atof(act.c_str()), atof(srp.c_str()), name, ann, start, end);
		if (atof(srp.c_str()) > 5)	Data_beat[name] = stockTemp;
		else if (atof(srp.c_str()) >= 0 && atof(srp.c_str()) <= 5)	Data_meet[name] = stockTemp;
		else if (atof(srp.c_str()) < 0)	Data_miss[name] = stockTemp;
	}

}

#endif 
#pragma once
