#include "Tuple.h"
#include "Scheme.h"
#include <set>

using namespace std;

class Relation
{
private:

public:
	Relation()
	{

	}

	~Relation()
	{

	}

	set<Tuple> tuples;
	Scheme scheme;
	string name;

	Relation select(int index, string value, Relation r)
	{
		Relation newR;
		newR.name = r.name;
		newR.scheme = r.scheme;

		set<Tuple>::iterator iter;
		for(iter = r.tuples.begin(); iter != r.tuples.end(); ++iter)
		{
			vector<string> v = *iter;

			if(v[index] == value)
			{
				newR.tuples.insert(*iter);
			}
		}

		return newR;
	}

	Relation project(vector<int> indexes, Relation r)
	{
		Relation newR;
		newR.name = r.name;
		newR.scheme = r.scheme;

		set<Tuple>::iterator iter;
		for(iter = r.tuples.begin(); iter != r.tuples.end(); ++iter)
		{
			Tuple t;

			Tuple v = *iter;
			for(unsigned int i = 0; i < indexes.size(); i++)
			{
				t.push_back(v[indexes[i]]);
			}
			newR.tuples.insert(t);
		}
		return newR;
	}

	Relation rename(vector<string> names, Relation r)
	{
		Relation newR;
		newR.name = r.name;

		for(unsigned int i = 0; i < names.size(); i++)
		{
			newR.scheme.names.push_back(names[i]);
		}
		newR.tuples = r.tuples;
		return newR;
	}

	Relation doubleSelect(int index, int index2, Relation r)
	{
		Relation newR;
		newR.name = r.name;
		newR.scheme = r.scheme;

		set<Tuple>::iterator iter;
		for(iter = r.tuples.begin(); iter != r.tuples.end(); ++iter)
		{
			vector<string> v = *iter;

			if(v[index] == v[index2])
			{
				newR.tuples.insert(*iter);
			}
		}
		/*Relation newestR;
		newestR.name = newR.name;
		newestR.scheme = newR.scheme;
		set<Tuple>::iterator it;
		for(it = newR.tuples.begin(); it != newR.tuples.end(); ++it)
		{
			Tuple t;
			Tuple v = *it;
			for(unsigned int i = 0; i < newR.tuples.size(); i++)
			{
				if(i != index)
				{
					t.push_back(v[i]);
				}
			}
			newestR.tuples.insert(t);
		}
		return newestR;*/
		return newR;
	}
















//****************************************************************
	string toString()
	{
		string output = "\nNAME: ";
		output += name;
		output += "\nSCHEMES: ";
		for(unsigned int i = 0; i < scheme.names.size(); i++)
		{
			output += scheme.names[i];
			if(i != scheme.names.size() - 1)
			{
				output += ", ";
			}
		}
		output += "\n";
		for(unsigned int i = 0; i < tuples.size(); i++)
		{
			//output += tuples[i].values;
			if(i != tuples.size() - 1)
			{
				output += ", ";
			}
		}
		return output;
	}
};
