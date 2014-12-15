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

void print(Parser parser, Database &database, ostream &out, Relation r)
{
	if(r.tuples.size() > 0)
	{
		out << "? Yes";
		out << "(" << r.tuples.size() << ")\n";
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
			//if(r.scheme.names.size() != 0)
			//{
				out << endl;
			//}
		}
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

bool duplicate(Parser parser, Database &database, Relation &r, int temp)
{
	for(unsigned int i = 0; i < r.scheme.names.size() - 1; i++)
	{
		for(unsigned int j = i; j < r.scheme.names.size(); j++)
		{
			if(parser.data.queries[temp].parameters[i].value == parser.data.queries[temp].parameters[i].value)
			{
				return true;
			}
		}
	}
	return false;
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
				/*if(r.scheme.names.size() != 0)
				{
					if(duplicate(parser, database, r, i))
					{
						r.doubleSelect(j, parser.data.queries[i].parameters[j].value, r);
					}
				}*/
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

void startInterpreting(Parser parser, ostream &out)
{
	Database database;

	dealWithSchemes(parser, database, out);

	dealWithFacts(parser, database, out);

	dealWithQueries(parser, database, out);
}

//************************************************************************************

void startParse(Scanner scan, ostream &out)
{
	Parser parse(scan);

	parse.datalogProgram();

	//out << parse.data.toString();

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
		//out << "(" << temp.getKind() << "," << temp.getValue() << "," << temp.getLine() << ")\n";
	}


	/*if(scanner.final == true)
	{
		//out << "Input Error on line " << scanner.number << "\n";
	}
	else
	{
		//out << "Total Tokens = " << scanner.tokens.size() << endl;
	}*/

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

