#include "BasicOptionPresentation.hpp"
//#include "ExcelDriver\excelmechanisms.hpp"

void BasicOptionPresentation::displayinExcel( OptionValueType yval)
{

	string text("Value");
	if (yval == Delta)
	{
		text = string("Delta");
	}

	Vector<double, long> yarr = calculate(yval);
	//printOneExcel(XARR, yarr, text);
}