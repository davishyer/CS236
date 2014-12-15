#include "Scanner.h"
#include "Parser.h"
#include "Database.h"
#include <map>
#include <sstream>
#include <iostream>
#include <fstream>
#include <istream>

using namespace std;

//************************************************************************************

void printBottom(Parser parser, Database &database, ostream &out, Relation &r)
{
	set<Tuple>::iterator iter;
	for(iter = r.tuples.begin(); iter != r.tuples.end(); ++iter)
	{
		vector<string> v = *iter;
		if(r.scheme.names.size() != 0)
		{
			out << "  ";
		}
		for(unsigned int j = 0; j < r.scheme.names.size(); j++)
		{
			out << r.scheme.names[j] << "=\'" << v[j] << "\'";
			if(j != r.scheme.names.size() - 1)
			{
				if(j != r.scheme.names.size() && (r.scheme.names[j] != r.scheme.names[j+1]))
				{
					out << ", ";
				}
				else
				{
					break;
				}
			}
		}
		out << endl;
	}
}

//************************************************************************************

void print(Parser parser, Database &database, ostream &out, Relation &r)
{
	if(r.tuples.size() > 0)
	{
		out << "? Yes";
		out << "(" << r.tuples.size() << ")";
		if(r.scheme.names.size() != 0)
		{
			out << "\n";
		}
		printBottom(parser, database, out, r);
	}
	else
	{
		out << "? No\n";
	}
}

//************************************************************************************

void dealWithSchemes(Parser parser, Database &database, ostream &out)
{
	for(unsigned int i = 0; i < parser.data.schemes.size(); i++)
	{
		Relation r;
		r.name = parser.data.schemes[i].NAME;

		Scheme s;

		for(unsigned int j = 0; j < parser.data.schemes[i].parameters.size(); j++)
		{
			if(parser.data.schemes[i].parameters[j].name == "")
			{
				s.names.push_back(parser.data.schemes[i].parameters[j].value);
			}
			else
			{
				s.names.push_back(parser.data.schemes[i].parameters[j].name);
			}
		}

		r.scheme = s;

		database.relation.insert(pair<string, Relation> (r.name, r));
	}
}

//************************************************************************************

void dealWithFacts(Parser parser, Database &database, ostream &out)
{
	for(unsigned int i = 0; i < parser.data.facts.size(); i++)
		{
			Tuple t;
			for(unsigned int j = 0; j < parser.data.facts[i].parameters.size(); j++)
			{
				string temp;
				if(parser.data.facts[i].parameters[j].name == "")
				{
					temp += parser.data.facts[i].parameters[j].value;
					t.push_back(temp);
				}
				else
				{
					temp += parser.data.facts[i].parameters[j].name;
					t.push_back(temp);
				}
			}
			if(database.relation.find(parser.data.facts[i].NAME) != database.relation.end())
			{
				database.relation[parser.data.facts[i].NAME].tuples.insert(t);
			}
		}
}

//************************************************************************************

void duplic(Parser parser, Database &database, Relation &r, int temp, int temp2)
{
	for(unsigned int j = temp2 + 1; j < parser.data.queries[temp].parameters.size(); j++)
	{
		if(parser.data.queries[temp].parameters[temp2].value == parser.data.queries[temp].parameters[j].value)
		{
			r = r.doubleSelect(temp2, j, r);
		}
	}
}


//************************************************************************************


void dealWithQueries(Parser parser, Database &database, ostream &out)
{
	for(unsigned int i = 0; i < parser.data.queries.size(); i++)
	{
		Relation r = database.relation[parser.data.queries[i].NAME];
		for(unsigned int j = 0; j < parser.data.queries[i].parameters.size(); j++)
		{
			if(parser.data.queries[i].parameters[j].name != "")
			{
				r = r.select(j, parser.data.queries[i].parameters[j].name, r);
			}
			else
			{
				duplic(parser, database, r, i, j);
			}
		}

		vector<int> indexes;
		vector<string> names;

		for(unsigned int k = 0; k < parser.data.queries[i].parameters.size(); k++)
		{
			if(parser.data.queries[i].parameters[k].value != "")
			{
				indexes.push_back(k);
				names.push_back(parser.data.queries[i]. parameters[k].value);
			}
		}
		r = r.project(indexes, r);

		r = r.rename(names, r);

		out << parser.data.queries[i].tostring();
		print(parser, database, out, r);
	}
}

//************************************************************************************

int countTuples(Database &database)
{
	int n = 0;
	for(auto iterator = database.relation.begin(); iterator != database.relation.end(); ++iterator)
	{
		Relation r = iterator->second;
		n += r.tuples.size();
	}
	return n;
}

//************************************************************************************

void FPAbig(Parser parser, Database &database, vector<Relation> &relations, unsigned int &i, vector<int> &indexes, vector<string> &names)
{
	while(relations.size() != 1)
	{
		relations[0] = relations[0].join(relations, parser.data.rules[i].head.NAME);
		relations.erase(relations.begin() + 1);
	}

	names.clear();

	for(unsigned int l = 0; l < parser.data.rules[i].head.parameters.size(); l++)
	{
		if(parser.data.rules[i].head.parameters[l].value != "")
		{
			names.push_back(parser.data.rules[i].head.parameters[l].value);
		}
	}

	indexes.clear();

	for(unsigned int m = 0; m < names.size(); m++)
	{
		for(unsigned int n = 0; n < relations[0].scheme.names.size(); n++)
		{
			if(names[m] == relations[0].scheme.names[n])
			{
				indexes.push_back(n);
			}
		}
	}


	relations[0] = relations[0].project(indexes, relations[0]);
	relations[0] = relations[0].rename(names, relations[0]);

	for(auto iterator = relations[0].tuples.begin(); iterator != relations[0].tuples.end(); ++iterator)
	{
		database.relation[relations[0].name].tuples.insert(*iterator);
	}
}

//************************************************************************************

void FPAsmall(Parser parser, Database &database, vector<Relation> &relations, unsigned int &i, vector<int> &indexes, vector<string> &names)
{
	if(relations[0].tuples.size() != 0)
	{
		relations[0].name = parser.data.rules[i].head.NAME;

		names.clear();

		for(unsigned int l = 0; l < parser.data.rules[i].head.parameters.size(); l++)
		{
			names.push_back(parser.data.rules[i].head.parameters[l].value);
		}

		indexes.clear();

		for(unsigned int m = 0; m < names.size(); m++)
		{
			for(unsigned int n = 0; n < relations[0].scheme.names.size(); n++)
			{
				if(names[m] == relations[0].scheme.names[n])
				{
					indexes.push_back(n);
				}
			}
		}
		relations[0] = relations[0].project(indexes, relations[0]);
		relations[0] = relations[0].rename(names, relations[0]);

		for(auto iterator = relations[0].tuples.begin(); iterator != relations[0].tuples.end(); ++iterator)
		{
			database.relation[relations[0].name].tuples.insert(*iterator);
		}
	}
}

//************************************************************************************

void FPA(Parser parser, Database &database, vector<Relation> &relations, unsigned int &i, vector<int> &indexes, vector<string> &names)
{
	if(relations.size() > 1)
	{
		FPAbig(parser, database, relations, i, indexes, names);
	}
	else if(relations.size() == 1)
	{
		FPAsmall(parser, database, relations, i, indexes, names);
	}
}

//************************************************************************************

void stringSelect(Relation &r, vector<string> &literals, vector<int> &literalsIndex)
{
	for(unsigned int i = 0; i < literals.size(); i++)
	{
		r = r.select(literalsIndex[i], literals[i], r);
	}
}

//************************************************************************************

void dealWithRules(Parser parser, Database &database, ostream &out, int &numberofPasses)
{
	for(unsigned int i = 0; i < parser.data.rules.size(); i++)
	{
		vector<Relation> relations;
		vector<int> indexes;
		vector<string> names;
		vector<string> literals;
		vector<int> literalsIndex;
		for(unsigned int j = 0; j < parser.data.rules[i].body.size(); j++)
		{
			indexes.clear();
			names.clear();
			literals.clear();
			literalsIndex.clear();
			Relation r = database.relation[parser.data.rules[i].body[j].NAME];
			for(unsigned int k = 0; k < parser.data.rules[i].body[j].parameters.size(); k++)
			{
				if(parser.data.rules[i].body[j].parameters[k].value != "")
				{
					indexes.push_back(k);
					names.push_back(parser.data.rules[i].body[j].parameters[k].value);
				}
				else
				{
					literals.push_back(parser.data.rules[i].body[j].parameters[k].name);
					literalsIndex.push_back(k);
				}
				if(k == parser.data.rules[i].body[j].parameters.size() - 1)
				{
					stringSelect(r, literals, literalsIndex);
					r = r.project(indexes, r);
					r = r.rename(names, r);
					relations.push_back(r);
				}
			}
		}
		FPA(parser, database, relations, i, indexes, names);
	}
	numberofPasses++;
}

//************************************************************************************


void startInterpreting(Parser parser, ostream &out)
{
	int numberofPasses = 0;

	Database database;

	dealWithSchemes(parser, database, out);

	dealWithFacts(parser, database, out);

	int numberTuples = countTuples(database);
	dealWithRules(parser, database, out, numberofPasses);
	int newTuples = countTuples(database);

	while(newTuples != numberTuples)
	{
		numberTuples = countTuples(database);
		dealWithRules(parser, database, out, numberofPasses);
		newTuples = countTuples(database);
	}
	stringstream ss;
	ss << numberofPasses;
	string final = ss.str();
	out << "Converged after " << final << " passes through the Rules." << endl;

	dealWithQueries(parser, database, out);
}

//************************************************************************************

void startParse(Scanner scan, ostream &out)
{
	Parser parse(scan);

	parse.datalogProgram();

	startInterpreting(parse, out);
}


//************************************************************************************

int main(int argc, char* argv[])
{
	string inputfile = argv[1];
	string outputfile = argv[2];

	ifstream in;
	in.open(argv[1]);
	ofstream out;
	out.open(argv[2]);

	Scanner scanner;

	while(!in.eof())
	{
		scanner.scan(in, out);
		if(scanner.final == true)
		{
			break;
		}
	}
	for(unsigned int i = 0; i < scanner.tokens.size(); i++)
	{
		Token temp = scanner.tokens[i];
	}

	try
	{
		startParse(scanner, out);
	}
	catch(string e)
	{
		out << e;
		in.close();
		out.close();
		return 0;
	}

	in.close();
	out.close();

	return 0;
}
