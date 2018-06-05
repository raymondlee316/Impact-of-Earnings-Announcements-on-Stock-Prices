#ifndef opOverload_h
#define opOverload_h

#include <iostream>
#include <vector>

using namespace std;

template <typename T>
ostream &operator<<(ostream &ostr, const vector<T> &vDouble) {
	int len = vDouble.size();
	for (int i = 0; i < len; ++i) {
		if (i % 10 == 0)
			cout << endl;
		ostr << vDouble[i] << '\t';
	}
	cout << endl;
	return ostr;
}

vector<double> &operator/=(vector<double> &leftVct, double rightNum) {
    if (rightNum) {
        int len = leftVct.size();
        for (int i = 0; i < len; ++i)
            leftVct[i] /= rightNum;
    }
    return leftVct;
}

vector<double> &operator+=(vector<double> &leftVct, const vector<double> &rightVct) {
    int lenL = leftVct.size(), lenR = rightVct.size();
    if (lenL == lenR)
        for (int i = 0; i < lenL; ++i)
            leftVct[i] += rightVct[i];
    return leftVct;
}

vector<double> operator-(const vector<double> &leftVct, const vector<double> &rightVct) {
    int lenL = leftVct.size(), lenR = rightVct.size();
    vector<double> temp(lenL);
    if (lenL == lenR)
        for (int i = 0; i < lenL; ++i)
            temp[i]=leftVct[i] - rightVct[i];
    return temp;
}

#endif // opOverload_h