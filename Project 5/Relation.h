#include "Tuple.h"
#include "Scheme.h"
#include <vector>
#include<algorithm>
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
//************************************************************************************

	bool dup(vector<Relation> &relations, Scheme &s, int temp, int temp2)
	{
		for(unsigned int i = 0; i < s.names.size(); i++)
		{
			if(relations[temp].scheme.names[temp2] == s.names[i])
			{
				return false;
			}
		}
		return true;
	}

//************************************************************************************

	Scheme schemeName(vector<Relation> &relations)
	{
		Scheme s;
		for(unsigned int i = 0; i < relations.size(); i++)
		{
			for(unsigned int j = 0; j < relations[i].scheme.names.size(); j++)
			{
				if(dup(relations, s, i, j))
				{
					s.names.push_back(relations[i].scheme.names[j]);
				}
			}
		}
		return s;
	}

//************************************************************************************

void makeScheme(vector<Relation> &relations, bool &check, Scheme &s, vector<int> &specialOne, vector<int> &specialTwo, int &trueCheck)
{
	for(unsigned int i = 0; i < relations[0].scheme.names.size(); i++)			//put first relation scheme names on a vector
	{
		s.names.push_back(relations[0].scheme.names[i]);
	}
	trueCheck = 0;
	for(unsigned int i = 0; i < relations[1].scheme.names.size(); i++)			//attempt to put second relations scheme names on the vector
	{
		check = false;
		for(unsigned int j = 0; j < s.names.size(); j++)
		{
			if(relations[1].scheme.names[i] != s.names[j])						//check each variable for a match
			{
				continue;
			}
			else				//found a matching variable
			{
				specialTwo.push_back(i);	//push matching variable index from second relation on a vector
				specialOne.push_back(j);	//push matching variable index from first relation on a vector
				check = true;
				break;
			}
		}
		if(!check)		//if we didn't have a matching variable
		{
			s.names.push_back(relations[1].scheme.names[i]);	//add new scheme name
			trueCheck++;
		}
	}
}

//************************************************************************************

void doJoin(vector<string> &v, vector<string> &v2, Relation &newR, vector<int> &specialTwo)
{
	Tuple t;
	for(unsigned int i = 0; i < v.size(); i++)		//push all tuple values onto new tuple
	{
		t.push_back(v[i]);
	}
	for(unsigned int j = 0; j < v2.size(); j++)		//attempt to add new tuple values
	{
		bool add = true;
		for(unsigned int k = 0; k < specialTwo.size(); k++)
		{
			if(j == specialTwo[k])
			{
				add = false;
			}
		}
		if(add)
		{
			t.push_back(v2[j]);
		}
	}
	newR.tuples.insert(t);
}

//************************************************************************************

void cartProduct(vector<string> &v, vector<string> &v2, Relation &newR)
{
	Tuple t;
	for(unsigned int i = 0; i < v.size(); i++)	//push all tuple values onto new tuple
	{
		t.push_back(v[i]);
	}
	for(unsigned int j = 0; j < v2.size(); j++)	//push all tuple values onto new tuple
	{
		t.push_back(v2[j]);
	}
	newR.tuples.insert(t);			//create the new tuple
}

//************************************************************************************

	bool canJoin(vector<string> &v, vector<string> &v2, vector<int> &specialOne, vector<int> &specialTwo)
	{
		for(unsigned int i = 0; i < specialOne.size(); i++)
		{

			if(v[specialOne[i]] == v2[specialTwo[i]])
			{
				continue;
			}
			else
			{
				return false;
			}
		}
		return true;
	}

//************************************************************************************

	Relation join(vector<Relation> &relations, string head)
	{
		Relation newR;
		newR.name = head;
		newR.scheme = schemeName(relations);
		bool cartesian = false;
		vector<int> specialOne;
		vector<int> specialTwo;
		specialOne.clear();
		specialTwo.clear();
		Scheme s;
		bool check = false;
		int trueCheck = 0;
		makeScheme(relations, check, s, specialOne, specialTwo, trueCheck);
		if(trueCheck == relations[1].scheme.names.size())		//checks if all names in second relation were added to scheme
		{
			cartesian = true;			//if so, we do a Cartesian product
		}

		set<Tuple>::iterator iter;
		for(iter = relations[0].tuples.begin(); iter != relations[0].tuples.end(); ++iter)
		{
			vector<string> v = *iter;
			set<Tuple>::iterator iterTwo;
			for(iterTwo = relations[1].tuples.begin(); iterTwo != relations[1].tuples.end(); ++iterTwo)
			{
				vector<string> v2 = *iterTwo;
				if(cartesian)		//the Cartesian product
				{
					cartProduct(v, v2, newR);
				}
				else if(canJoin(v, v2, specialOne, specialTwo))		//join around matching variables
				{
					doJoin(v, v2, newR, specialTwo);
				}
			}
		}
		newR.scheme = s;
		return newR;
	}

//************************************************************************************

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

//************************************************************************************

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

//************************************************************************************

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

//************************************************************************************

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
		for(auto iterator = tuples.begin(); iterator != tuples.end(); ++iterator)
		{
			//output += tuples[*iterator];
		}
		return output;
	}
};
