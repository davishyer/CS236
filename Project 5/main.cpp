#include "Scanner.h"
#include "Parser.h"
#include "Database.h"
#include "Driver.h"
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

void stringSelect(Relation &r, vector<string> &literals, vector<int> &literalsIndex)
{
	for(unsigned int i = 0; i < literals.size(); i++)
	{
		r = r.select(literalsIndex[i], literals[i], r);
	}
}

//************************************************************************************

string createName(string type, unsigned int i)
{
	string output = type;
	stringstream ss;
	ss << i;
	string temp = ss.str();
	output += temp;
	return output;
}

//************************************************************************************

void DFS(string name, Graph &dependency, int &post, vector<Node> &nodes)
{
	dependency.graph[name].mark = true;
	for(auto iter = dependency.graph[name].adj.begin(); iter != dependency.graph[name].adj.end(); ++iter)
	{
		string nextName = *iter;
		Node m = dependency.graph[nextName];
		if(!m.mark)
		{
			DFS(m.ID, dependency, post, nodes);
		}
	}
	dependency.graph[name].postOrder = post++;
	nodes.push_back(dependency.graph[name]);
}

//************************************************************************************

void resetNodes(Graph &dependency)
{
	for(auto iterator = dependency.graph.begin(); iterator != dependency.graph.end(); ++iterator)
	{
		Node n = iterator->second;
		dependency.graph[n.ID].mark = false;
		dependency.graph[n.ID].postOrder = -1;
	}
}

//************************************************************************************

void backwardsEdges(vector<Node> &nodes, vector<vector<Node>> &backwards, Graph &dependency)
{
	sort(nodes.rbegin(), nodes.rend());

	for(int i = 0; i < nodes.size(); i++)
	{
		string id = nodes[i].ID;
		bool first = true;
		vector<Node> temp;
		for(auto iterator = dependency.graph[id].adj.begin(); iterator != dependency.graph[id].adj.end(); ++iterator)
		{
			string nextid = *iterator;
			if(dependency.graph[id].postOrder <= dependency.graph[nextid].postOrder)
			{
				if(first)
				{
					temp.push_back(dependency.graph[id]);
				}
				first = false;
				temp.push_back(dependency.graph[nextid]);
			}
		}
		if(temp.size() != 0)
		{
			sort(temp.begin() + 1, temp.end());
			sort(temp.rbegin(), temp.rend() - 1);
			backwards.push_back(temp);
		}
	}
}

//************************************************************************************

void startPostOrder(Parser &parser, ostream &out, Graph &dependency, Database &database, int i, vector<Node> &nodes, vector<vector<Node>> &backwards, vector<Node> &rules, vector<RULE> &qRules)
{
	int post = 1;
	resetNodes(dependency);
	DFS(createName("Q", i + 1), dependency, post, nodes);
	//------------------------------------------------------------------------------
	out << endl << parser.data.queries[i].tostring() << "?";
	vector<Node> postorder = nodes;
	sort(postorder.begin(), postorder.end());
	//------------------------------------------------------------------------------
	out << "\n\n  Postorder Numbers\n";
	for(int j = (postorder.size() - 1); j >= 0; j--)
	{
		Node n = postorder[j];
		out << "    " << n.ID << ": " << n.postOrder << endl;
	}
	//------------------------------------------------------------------------------
	out << "\n  Rule Evaluation Order\n";
	for(int j = 0; j < nodes.size() - 1; j++)
	{
		Node n = nodes[j];
		out << "    " << n.ID << endl;
	}
	for(int j = 0; j < nodes.size() - 1; j++)
	{
		Node n = nodes[j];
		qRules.push_back(parser.data.rules[n.number]);
	}
	//------------------------------------------------------------------------------
	out << "\n  Backward Edges\n";
	rules = nodes;
	backwardsEdges(nodes, backwards, dependency);
	sort(nodes.begin(), nodes.end());
	for(int j = 0; j < backwards.size(); j++)
	{
		out << "    " << backwards[j][0].ID << ":";
		for(int k = 1; k < backwards[j].size(); k++)
		{
			out << " " << backwards[j][k].ID;
		}
		out << endl;
	}
	out << endl;
}

//************************************************************************************

void finishPostOrder(Parser &parser, ostream &out, Graph &dependency, int &numberofPasses, Database &database, Relation &r, int i, vector<Node> &nodes, vector<Node> &rules)
{
	rules.erase(rules.begin() + rules.size());
	//------------------------------------------------------------------------------
	out << "  Rule Evaluation\n";
	for(int j = 0; j < numberofPasses; j++)
	{
		for(int k = 0; k < rules.size(); k++)
		{
			Node n = rules[k];
			out << "    " <<  parser.data.rules[n.number].ruleString() << endl;
		}
	}
	out << endl;
	//------------------------------------------------------------------------------
	out << parser.data.queries[i].tostring();
	print(parser, database, out, r);
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

void runRules(Parser &parser, Database &database, ostream &out, int &numberofPasses, vector<vector<Node>> &backwards)
{
	int numberTuples;
	int newTuples;
	if(backwards.size() == 0)
	{
		dealWithRules(parser, database, out, numberofPasses);
	}
	else
	{
		do 						//adds tuples to the Database
		{
			numberTuples = countTuples(database);
			dealWithRules(parser, database, out, numberofPasses);
			newTuples = countTuples(database);
		}while(newTuples > numberTuples);
	}
}

//************************************************************************************

void dealWithQueries(Parser parser, Database &database, ostream &out, Graph &dependency)
{

	for(unsigned int i = 0; i < parser.data.queries.size(); i++) 			//runs through each query
	{
		int numberofPasses = 0;

		Relation r = database.relation[parser.data.queries[i].NAME]; 		//references the relation which relates to the query i

		vector<int> indexes;
		vector<string> names;
		vector<RULE> qRules;
		vector<RULE> oldRules;
		vector<Node> nodes;
		vector<vector<Node>> backwards;
		vector<Node> rules;
		startPostOrder(parser, out, dependency, database, i, nodes, backwards, rules, qRules); 		//assesses the POA, reorders rules
		oldRules = parser.data.rules; 								//temporarily sets parser rules in a holding variable
		parser.data.rules = qRules; 								//prepares parser to analyze only rules of interest
		runRules(parser, database, out, numberofPasses, backwards);
		r = database.relation[parser.data.queries[i].NAME];
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
		parser.data.rules = oldRules;
		finishPostOrder(parser, out, dependency, numberofPasses, database, r, i, nodes, rules);
	}
}

//************************************************************************************

void graphString(Graph &dependency, ostream &out)
{
	out << "Dependency Graph";
	for(auto iterator = dependency.graph.begin(); iterator != dependency.graph.end(); ++iterator)
	{
		Node n = iterator->second;
		out << "\n  " << iterator->first << ":";
		for(auto iter = n.adj.begin(); iter != n.adj.end(); ++iter)
		{
			auto it = iter;
			++it;
			out << " " << *iter;
		}
	}
	out << endl;
}

//************************************************************************************

void graphRules(Parser &parser, Graph &dependency, vector<Node> &r)
{
	for(unsigned int i = 0; i < parser.data.rules.size(); i++)				//assigns names for all rules
	{
		Node n;
		n.ID = createName("R", i + 1);
		n.number = i;
		n.pred = parser.data.rules[i].head;
		for(unsigned int j = 0; j < parser.data.rules[i].body.size(); j++)	//assigns adjacent nodes for all rules
		{
			for(unsigned int k = 0; k < parser.data.rules.size(); k++)		//checks with each rule name
			{
				if(parser.data.rules[i].body[j].NAME == parser.data.rules[k].head.NAME)
				{
					string temp = createName("R", k + 1);
					n.adj.insert(temp);
				}
			}
		}
		dependency.graph.insert(pair<string, Node> (n.ID, n));
		r.push_back(n);
	}
}

//************************************************************************************

void createGraph(Parser &parser, Graph &dependency, ostream &out)
{
	vector<Node> q;
	vector<Node> r;
	for(unsigned int i = 0; i < parser.data.queries.size(); i++)			//assigns names for all queries
	{
		Node n;
		n.ID = createName("Q", i + 1);
		n.pred = parser.data.queries[i];
		q.push_back(n);
	}

	graphRules(parser, dependency, r);

	for(unsigned int i = 0; i < q.size(); i++)				//assigns adjacent nodes for all queries
	{
		for(unsigned int j = 0; j < r.size(); j++)
		{
			if(q[i].pred.NAME == r[j].pred.NAME)
			{
				q[i].adj.insert(r[j].ID);
			}
		}
		dependency.graph.insert(pair<string, Node> (q[i].ID, q[i]));
	}

	graphString(dependency, out);
}

//************************************************************************************


void startInterpreting(Parser parser, ostream &out)
{
	Database database;
	dealWithSchemes(parser, database, out);
	dealWithFacts(parser, database, out);

	Graph dependency;
	createGraph(parser, dependency, out);

	dealWithQueries(parser, database, out, dependency);
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
