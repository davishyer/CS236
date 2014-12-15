#pragma once
#include <vector>
#include <string>
#include <sstream>
#include "Parameter.h"

using namespace std;

class PREDICATE
{
public:
	string NAME;
	vector <PARAMETER> parameters;
	PREDICATE()
	{

	}
	PREDICATE(string name, vector<PARAMETER> param)
	{
		NAME = name;
		parameters = param;
	}
	~PREDICATE()
	{

	}


	string tostring()
	{
		string out;
		out = NAME + "(";
		for(unsigned int i = 0; i < parameters.size(); i++)
		{
			if(parameters[i].hasName)
			{
				out += "\'";
				out += parameters[i].name;
				out += "\'";
			}

				out += parameters[i].value;

			if(i < (parameters.size() - 1))
			{
				out += ",";
			}
			else
			{
				out += ")";
			}
		}
		return out;
	}
};
