#include <set>
#include <string>

using namespace std;

class Node
{
private:
public:
	bool mark;
	int postOrder;
	string ID;
	set<string> adj;
	PREDICATE pred;
	int number;

	Node()
	{
		mark = false;
		postOrder = -1;
		number = -1;
	}
	~Node()
	{

	}

	bool operator < (const Node &n) const
	{
		return n.ID < ID;
	}
};
