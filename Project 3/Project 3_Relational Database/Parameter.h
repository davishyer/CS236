#pragma once
#include <vector>
#include <string>

using namespace std;

class PARAMETER
{
private:

public:
	string value;
	string name;
	bool hasName = false;
	bool hasValue = false;
	PARAMETER()
	{

	}
	PARAMETER(string nameIn, int valueIn)
	{
		if(hasName)
		{
			name = nameIn;
		}
		else if(hasValue)
		{
			value = valueIn;
		}
	}
	~PARAMETER()
	{

	}

};
