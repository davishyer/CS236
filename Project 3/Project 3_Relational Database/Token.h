#pragma once
#include "Scanner.h"
#include <string>
#include <iostream>

using namespace std;

enum tokenType
{
	COMMA, PERIOD, Q_MARK, LEFT_PAREN, RIGHT_PAREN, COLON,
	COLON_DASH, SCHEMES, FACTS, RULES, QUERIES, ID, STRING, ENDFILE
};

class Token
{
private:
	tokenType kind;
	string value;
public:
	int line;
	Token()
	{
		line = 0;
	}
	Token(tokenType k, string v, int l)
	{
		kind = k;
		value = v;
		line = l;
	}
	~Token()
	{

	}

	string getKind()
	{
		switch(kind)
		{
		case 0:
			return "COMMA";
			break;

		case 1:
			return "PERIOD";
			break;

		case 2:
			return "Q_MARK";
			break;

		case 3:
			return "LEFT_PAREN";
			break;

		case 4:
			return "RIGHT_PAREN";
			break;

		case 5:
			return "COLON";
			break;

		case 6:
			return "COLON_DASH";
			break;

		case 7:
			return "SCHEMES";
			break;

		case 8:
			return "FACTS";
			break;

		case 9:
			return "RULES";
			break;

		case 10:
			return "QUERIES";
			break;

		case 11:
			return "ID";
			break;

		case 12:
			return "STRING";
			break;
		case 13:
			return "ENDFILE";
			break;
		}
		return "";
	}

	string getValue()
	{
		return value;
	}

	int getLine()
	{
		return line;
	}

	void print()
	{
		cout << "(" << getKind() << "," << value << "," << line << ")\n";
	}
};
