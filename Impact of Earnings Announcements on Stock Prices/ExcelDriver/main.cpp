// TestMatrixExcel.cpp
//
// Test output of a matrix in Excel. Here we 
// use the Excel Driver object directly.
//
// (C) Datasim Education BV 2006
//
#include "stdafx.h"
#include "BitsAndPieces\StringConversions.hpp"
#include "ExcelDriver.hpp"
#include "ExcelMechanisms.hpp"
#include "VectorsAndMatrices\NumericMatrix.cpp" // Template
#include "ExceptionClasses\DatasimException.hpp"
#include "menu.h"

#include <string>

#include "download.h"
#include "readdata.h"
#include "stock.h"
#include "cal.h"

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <thread>

using namespace std;

int main()
{
	map<string, stock> Data_beat, Data_meet, Data_miss;
	readdata(Data_beat, Data_meet, Data_miss);
	map<string, double> Data_SPY;//STRING+CLOSE NAME
	vector<vector<vector<double> > > result;

	cout << "     ________________________________________________________________________________________\n"
		<< "    /                                                                                        |\n"
		<< "    $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ |\n"
		<< "    $$     ________  _______   ________         ______    ______    ______    ______      $$ |\n"
		<< "    $$    /        |/       \\ /        |       /      \\  /      \\  /      \\  /      \\     $$ |\n"
		<< "    $$    $$$$$$$$/ $$$$$$$  |$$$$$$$$/       /$$$$$$  |/$$$$$$  |/$$$$$$  |/$$$$$$  |    $$ |\n"
		<< "    $$    $$ |__    $$ |__$$ |$$ |__          $$ \\__$$/ $$ \\__$$ |$$ \\__$$ |$$ ___$$ |    $$ |\n"
		<< "    $$    $$    |   $$    $$< $$    |         $$      \\ $$    $$< $$    $$<   /   $$<     $$ |\n"
		<< "    $$    $$$$$/    $$$$$$$  |$$$$$/          $$$$$$$  | $$$$$$  | $$$$$$  | _$$$$$  |    $$ |\n"
		<< "    $$    $$ |      $$ |  $$ |$$ |_____       $$ \\__$$ |$$ \\__$$ |$$ \\__$$ |/  \\__$$ |    $$ |\n"
		<< "    $$    $$ |      $$ |  $$ |$$       |      $$    $$/ $$    $$/ $$    $$/ $$    $$/     $$ |\n"
		<< "    $$    $$/       $$/   $$/ $$$$$$$$/        $$$$$$/   $$$$$$/   $$$$$$/   $$$$$$/      $$ |\n"
		<< "    $$                                                                                    $$ |\n"
		<< "    $$                                                                                    $$ |\n"
		<< "    $$                                                                                    $$ |\n"
		<< "    $$           _______                                                   __             $$ |\n"
		<< "    $$          /       \\                                                 /  |            $$ |\n"
		<< "    $$          $$$$$$$  | ______    ______      __   ______    _______  _$$ |_           $$ |\n"
		<< "    $$          $$ |__$$ |/      \\  /      \\    /  | /      \\  /       |/ $$   |          $$ |\n"
		<< "    $$          $$    $$//$$$$$$  |/$$$$$$  |   $$/ /$$$$$$  |/$$$$$$$/ $$$$$$/           $$ |\n"
		<< "    $$          $$$$$$$/ $$ |  $$/ $$ |  $$ |   /  |$$    $$ |$$ |        $$ | __         $$ |\n"
		<< "    $$          $$ |     $$ |      $$ \\__$$ |   $$ |$$$$$$$$/ $$ \\_____   $$ |/  |        $$ |\n"
		<< "    $$          $$ |     $$ |      $$    $$/    $$ |$$       |$$       |  $$  $$/         $$ |\n"
		<< "    $$          $$/      $$/        $$$$$$/__   $$ | $$$$$$$/  $$$$$$$/    $$$$/          $$ |\n"
		<< "    $$                                    /  \\__$$ |                                      $$ |\n"
		<< "    $$                                    $$    $$/                                       $$ |\n"
		<< "    $$                                     $$$$$$/                                        $$ |\n"
		<< "    $$                                                                                    $$ |\n"
		<< "    $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$/ \n";
	string buf;
	char c;
	cout << "press Enter to start ...   ";
	c = cin.peek();
	if (c == '\n')
		cin.get();
	else
		cin >> buf;
	bool flag_DlComplete = false;
	system("cls");
	while (true)
	{
		cout << "Please enter the number in front of the option you want to do." << endl;
		cout << "1. Retrieve historical price data for all stocks." << endl;
		cout << "2. Pull information for one stock from one group." << endl;
		cout << "3. Show ARR or CAAR for one group." << endl;
		cout << "4. Show the Excel graph with CAAR for all 3 groups." << endl;
		cout << "5. Quit this program." << endl;

		cin >> buf;
		if (buf.size() != 1) {
			cout << "Please enter a valid number." << endl << endl;
			continue;
		}
		c = buf[0];
		cout << "The option you choose is: ";

		if (c == '1') {
			clock_t t_start, t_end;
			t_start = clock();
			result = downloadData(Data_beat, Data_meet, Data_miss, Data_SPY);
			t_end = clock();
			int stocksNum = Data_beat.size() + Data_miss.size() + Data_meet.size();
			printf("All stocks downloaded in %f second(s)\n%d stock(s) meet requirement, %d stock(s) removed\n\n",
				(double)(t_end - t_start) / CLOCKS_PER_SEC, stocksNum, 500 - stocksNum);
			flag_DlComplete = true;
		}
		else if (!flag_DlComplete && c < '5' && c > '1') {
			cout << "Please download data first." << endl << endl;
			continue;
		}

		else if (c == '2')
			pullstockinfo(Data_beat, Data_meet, Data_miss);

		else if (c == '3')
			showAAR(result);

		else if (c == '4')
			showgraph(result);
		else if (c == '5')
		{
			cout << "Quit." << endl;
			break;
		}
		else
			cout << "Please enter a valid number." << endl << endl;
	}


	system("pause");
	return 0;
}