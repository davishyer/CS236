#pragma once
#include <vector>
#include "Predicate.h"

using namespace std;

class RULE
{
public:
	PREDICATE head;
	vector<PREDICATE> body;

	RULE()
	{

	}
	~RULE()
	{

	}

	string ruleString()
	{
		string output;
		output = head.tostring();
		output += " :- ";
		for(unsigned int i = 0; i < body.size(); i++)
		{
			output += body[i].tostring();
			if(i < body.size() - 1)
			{
				output += ",";
			}
		}
		return output;
	}
};
