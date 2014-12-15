#pragma once
#include <vector>
#include <string>
#include <cstdlib>
#include <set>
#include "Rule.h"
#include "Parameter.h"
#include "Predicate.h"

using namespace std;

class Datalog
{
public:
	vector<PREDICATE> schemes;
	vector<PREDICATE> facts;
	vector<RULE> rules;
	vector<PREDICATE> queries;
	set<string> domain;

	Datalog()
	{

	}
	~Datalog()
	{

	}

	string toString()
	{
		stringstream ss;
		string output;
		string temp;
		output = "Success!\n";
		output += "Schemes(";
		ss << schemes.size();
		temp = ss.str();
		output += temp;
		output += "):\n";
		for(unsigned int i = 0; i < schemes.size(); i++)
		{
			output += "  ";
			output += schemes[i].tostring();
			output += "\n";
		}

		output += "Facts(";
		stringstream s;
		s << facts.size();
		temp = s.str();
		output += temp;
		output += "):\n";
		for(unsigned int i = 0; i < facts.size(); i++)
		{
			output += "  ";
			output += facts[i].tostring();
			output += "\n";
		}

		output += "Rules(";
		stringstream ssss;
		ssss << rules.size();
		temp = ssss.str();
		output += temp;
		output += "):\n";
		for(unsigned int i = 0; i < rules.size(); i++)
		{
			output += "  ";
			output += rules[i].ruleString();
			output += "\n";
		}

		output += "Queries(";
		stringstream sss;
		sss << queries.size();
		temp = sss.str();
		output += temp;
		output += "):\n";
		for(unsigned int i = 0; i < queries.size(); i++)
		{
			output += "  ";
			output += queries[i].tostring();
			output += "\n";
		}

		output += "Domain(";
		stringstream a;
		a << domain.size();
		temp = a.str();
		output += temp;
		output += "):\n";
		vector<string> temporary(domain.begin(), domain.end());
		for(unsigned i = 0; i < temporary.size(); i++)
		{
			output += " ";
			output += "\'";
			output += temporary[i];
			output += "\'";
			output += "\n";
		}
		return output;
	}
};
