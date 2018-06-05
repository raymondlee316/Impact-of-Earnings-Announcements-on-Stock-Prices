#pragma once
#ifndef download_h
#define download_h
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <string> 
#include <iostream>
#include <sstream>  
#include <vector>
#include <locale>
#include <iomanip>
#include "curl.h"
#include "readdata.h"

using namespace std;

struct MemoryStruct {
	char *memory;
	size_t size;
};

void *myrealloc(void *ptr, size_t size)
{
	/* There might be a realloc() out there that doesn't like reallocing
	NULL pointers, so we take care of it here */
	if (ptr)
		return realloc(ptr, size);
	else
		return malloc(size);
}


size_t write_data2(void *ptr, size_t size, size_t nmemb, void *data)//把数据存在函数中
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)data;

	mem->memory = (char *)myrealloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory) {
		memcpy(&(mem->memory[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	}
	return realsize;
}

string getTimeinSeconds(string Time)
{
	std::tm t = { 0 };
	std::istringstream ssTime(Time);
	char time[100];
	memset(time, 0, 100);
	if (ssTime >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S"))
	{
		sprintf(time, "%ld", mktime(&t));
		return string(time);
	}
	else
	{
		cout << "Parse failed\n";
		return "";
	}
}

int yahoo(map<string, stock> &data)//string:ticker
{
	vector<double> adjprice;

	struct MemoryStruct stockdata;
	stockdata.memory = NULL;
	stockdata.size = 0;
	//file pointer to create file that store the stockdata
	FILE *fp;

	/* declaration of an object CURL */
	CURL *handle;

	/* result of the whole process */
	CURLcode result;

	/* the first functions */
	/* set up the program environment that libcurl needs */
	curl_global_init(CURL_GLOBAL_ALL);

	/* curl_easy_init() returns a CURL easy handle that you're gonna reuse in other easy functions*/
	handle = curl_easy_init();
	/* if everything's all right with the easy handle... */
	if (handle)
	{
		string sCookies, sCrumb;
		if (sCookies.length() == 0 || sCrumb.length() == 0)
		{
			curl_easy_setopt(handle, CURLOPT_URL, "https://finance.yahoo.com/quote/A/history");
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
			curl_easy_setopt(handle, CURLOPT_COOKIEJAR, "cookies.txt");
			curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "cookies.txt");//generate cookie

			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&stockdata);

			/* perform, then store the expected code in 'result'*/
			result = curl_easy_perform(handle);

			/* Check for errors */
			if (result != CURLE_OK)
			{
				/* if errors have occured, tell us what is wrong with 'result'*/
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
				return 1;
			}

			char cKey[] = "CrumbStore\":{\"crumb\":\""; 
			char *ptr1 = strstr(stockdata.memory, cKey);
			char *ptr2 = ptr1 + strlen(cKey);
			char *ptr3 = strstr(ptr2, "\"}");
			if (ptr3 != NULL)
				*ptr3 = NULL;

			sCrumb = ptr2;

			fp = fopen("cookies.txt", "r");
			char cCookies[100];
			if (fp) {
				while (fscanf(fp, "%s", cCookies) != EOF);
				fclose(fp);
			}
			else
				cerr << "cookies.txt does not exists" << endl;

			sCookies = cCookies;
			free(stockdata.memory);
			stockdata.memory = NULL;
			stockdata.size = 0;
		}

		for (map<string, stock>::iterator itr = data.begin(); itr != data.end();)//put in price into datamap, if there are no 91 prices or 0, delete that stock
		{
			int redoCount = 10;
			while (TRUE) {
				// stocks
				string urlA = "https://query1.finance.yahoo.com/v7/finance/download/";
				string symbol = string(itr->first);
				string urlB = "?period1=";
				string urlC = "&period2=";
				string urlD = "&interval=1d&events=history&crumb=";
				string url = urlA + symbol + urlB + getTimeinSeconds(itr->second.getStart()) + urlC + getTimeinSeconds(itr->second.getEnd()) + urlD + sCrumb;
				const char * cURL = url.c_str();
				const char * cookies = sCookies.c_str();
				curl_easy_setopt(handle, CURLOPT_COOKIE, cookies);   // Only needed for 1st stock
				curl_easy_setopt(handle, CURLOPT_URL, cURL);

				curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
				curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&stockdata);
				result = curl_easy_perform(handle);

				/* Check for errors */
				if (result != CURLE_OK)
				{
					/* if errors have occurred, tell us what is wrong with 'result'*/
					fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
					return 1;
				}

				stringstream sData;
				sData.str(stockdata.memory);
				string sValue, sDate;
				double dValue = 0;
				string line;
				getline(sData, line);// store sData in line
				while (getline(sData, line)) {
					sDate = line.substr(0, line.find_first_of(','));
					line.erase(line.find_last_of(','));
					sValue = line.substr(line.find_last_of(',') + 1);
					dValue = strtod(sValue.c_str(), NULL);
					adjprice.push_back(dValue);
				}
				printf(" ... get %2d data from [%-5s]\n", adjprice.size(), itr->first.c_str());
				if (adjprice.size() || !redoCount) {
					bool removeFlag = FALSE;
					if (adjprice.size() != DATA_SIZE_1) {
						printf("[%s] has invalid data length: %d ... removed\n", itr->first.c_str(), adjprice.size());
						removeFlag = TRUE;
					}
					else {
						for (auto itrV = adjprice.begin(); itrV != adjprice.end(); ++itrV) {
							if (*itrV <= 0) {
								printf("[%s] has zero or negative value ... removed\n", itr->first.c_str());
								removeFlag = TRUE;
								break; // break for loop
							}
						}
					}
					if (removeFlag)
						data.erase(itr++);
					//auto temp = itr;					//itr++					//data.erase(temp)
					else {
						itr->second.setPrice(adjprice);
						++itr;
					}
					adjprice.clear();//
					// free stockdata
					free(stockdata.memory);
					stockdata.memory = NULL;
					stockdata.size = 0;
					break; // break while loop
				}
				//Sleep(789);
				--redoCount;
			}
		}
	}
	else
	{
		fprintf(stderr, "Curl init failed!\n");
		return 1;
	}

	/* cleanup since you've used curl_easy_init */
	curl_easy_cleanup(handle);

	/* this function releases resources acquired by curl_global_init() */
	curl_global_cleanup();
	return 0;
}

// Download data for SPY
int yahoo_SPY(map<string, double> &data)
{
	struct MemoryStruct stockdata;
	stockdata.memory = NULL;
	stockdata.size = 0;
	//file pointer to create file that store the stockdata
	FILE *fp;

	/* declaration of an object CURL */
	CURL *handle;

	/* result of the whole process */
	CURLcode result;

	/* the first functions */
	/* set up the program environment that libcurl needs */
	curl_global_init(CURL_GLOBAL_ALL);

	/* curl_easy_init() returns a CURL easy handle that you're gonna reuse in other easy functions*/
	handle = curl_easy_init();
	/* if everything's all right with the easy handle... */
	if (handle)
	{
		string sCookies, sCrumb;
		if (sCookies.length() == 0 || sCrumb.length() == 0)
		{
			curl_easy_setopt(handle, CURLOPT_URL, "https://finance.yahoo.com/quote/A/history");
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
			curl_easy_setopt(handle, CURLOPT_COOKIEJAR, "cookies.txt");
			curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "cookies.txt");

			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&stockdata);

			/* perform, then store the expected code in 'result'*/
			result = curl_easy_perform(handle);

			/* Check for errors */
			if (result != CURLE_OK)
			{
				/* if errors have occured, tell us what is wrong with 'result'*/
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
				return 1;
			}

			char cKey[] = "CrumbStore\":{\"crumb\":\"";
			char *ptr1 = strstr(stockdata.memory, cKey);
			char *ptr2 = ptr1 + strlen(cKey);
			char *ptr3 = strstr(ptr2, "\"}");
			if (ptr3 != NULL)
				*ptr3 = NULL;

			sCrumb = ptr2;

			fp = fopen("cookies.txt", "r");
			char cCookies[100];
			if (fp) {
				while (fscanf(fp, "%s", cCookies) != EOF);
				fclose(fp);
			}
			else
				cerr << "cookies.txt does not exists" << endl;

			sCookies = cCookies;
			free(stockdata.memory);
			stockdata.memory = NULL;
			stockdata.size = 0;
		}

		// stocks
		string startTime = getTimeinSeconds("2016-06-01T16:00:00");
		string endTime = getTimeinSeconds("2018-04-20T16:00:00");
		string urlA = "https://query1.finance.yahoo.com/v7/finance/download/";
		string symbolsp = "SPY";
		string urlB = "?period1=";
		string urlC = "&period2=";
		string urlD = "&interval=1d&events=history&crumb=";
		string url = urlA + symbolsp + urlB + startTime + urlC + endTime + urlD + sCrumb;
		const char * cURL = url.c_str();
		const char * cookies = sCookies.c_str();
		curl_easy_setopt(handle, CURLOPT_COOKIE, cookies);   // Only needed for 1st stock
		curl_easy_setopt(handle, CURLOPT_URL, cURL);

		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&stockdata);
		result = curl_easy_perform(handle);

		/* Check for errors */
		if (result != CURLE_OK)
		{
			/* if errors have occurred, tell us what is wrong with 'result'*/
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
			return 1;
		}

		stringstream sData;
		sData.str(stockdata.memory);
		string sValue, sDate;
		double dValue = 0;
		string line;
		getline(sData, line);
		while (getline(sData, line)) {
			sDate = line.substr(0, line.find_first_of(','));
			sDate = sDate + "T16:00:00";
			line.erase(line.find_last_of(','));
			sValue = line.substr(line.find_last_of(',') + 1);
			dValue = strtod(sValue.c_str(), NULL);
			data[sDate] = dValue;
		}

		// free stockdata
		free(stockdata.memory);
		stockdata.memory = NULL;
		stockdata.size = 0;
	}
	else
	{
		fprintf(stderr, "Curl init failed!\n");
		return 1;
	}

	/* cleanup since you've used curl_easy_init */
	curl_easy_cleanup(handle);

	/* this function releases resources acquired by curl_global_init() */
	curl_global_cleanup();
	return 0;
}

#endif // !download_h
#pragma once
