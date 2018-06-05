#ifndef stock_h
#define stock_h
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "opOverload.h"

#define DATA_SIZE 90    // length of AAR, CAAR, etc...
#define DATA_SIZE_1 91  // length of price
using namespace std;


class stock {
private:
	double est, act, srp; // estimated EPS, actual EPS, surprise EPS (i.e. est/act-1)
	string name, ann, start, end; // ticker name, announcement date, start date, end date
	vector<double> price; // adjusted close price
public:
	stock() {};

	stock(double est_, double act_, double srp_, string name_, string ann_, string start_, string end_) :
		est(est_), act(act_), srp(srp_), name(name_), ann(ann_), start(start_), end(end_) {}

	stock(const stock &stock_) :
		est(stock_.est), act(stock_.act), srp(stock_.srp), name(stock_.name),
		ann(stock_.ann), start(stock_.start), end(stock_.end), price(stock_.price) {}

	~stock() {}
	
	stock operator=(const stock &stock_) {
		est = stock_.est;
		act = stock_.act;
		srp = stock_.srp;
		name = stock_.name;
		ann = stock_.ann;
		start = stock_.start;
		end = stock_.end;
		price = stock_.price;
		return *this; //op=(*this, stock_)
	}
	
	friend ostream &operator<<(ostream &ostr, const stock &stock_) {//call by reference
		ostr << "stockName: " << stock_.name << endl
			<< "estimate: " << stock_.est << endl
			<< "actual: " << stock_.act << endl
			<< "surprice: " << stock_.srp << endl
			<< "announce: " << stock_.ann << endl
			<< "startDate: " << stock_.start << endl
			<< "endDate: " << stock_.end << endl
			<< "Price:" << stock_.price << endl << endl;
		return ostr;
	}

	void setPrice(const vector<double> &price_) {
		price.resize(price_.size());
		for (unsigned int i = 0; i < price_.size(); ++i)
			price[i] = price_[i];
	}

	double getEst() { return est; }

	double getAct() { return act; }

	double getSrp() { return srp; }

	string getName() { return name; }

	string getAnn() { return ann; }

	string getStart() { return start; }

	string getEnd() { return end; }

	const vector<double> &getPrice() { return price; }
};
#endif
#pragma once
