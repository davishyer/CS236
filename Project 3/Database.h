#include <map>
#include "Relation.h"

using namespace std;

class Database
{
private:

public:
	Database()
	{

	}

	~Database()
	{

	}
	map<string, Relation> relation;
	//key: name of relation
	//value: relation
};
