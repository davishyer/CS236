#pragma once
#include "Token.h"
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include <cstdlib>

using namespace std;

class Scanner
{
private:
	string Quote(ifstream &in)								//returns the finished string
	{
		char c = in.get();
		string output = "\"";
		if(c == '\n' || c == 3)
		{
			final = true;
			return output;
		}
		else
		{
			while(c != '\'')
			{
				if(c == '\n' || c == 3)
				{
					final = true;
					break;
				}
				string str(1, c);
				output += str;
				c = in.get();
			}
		}
		output += "\"";
		return output;
	}

	bool arrow(ifstream &in)								//returns true if : followed by -
	{
		char c = in.get();
		if(c == '-')
		{
			return true;
		}
		in.putback(c);
		return false;
	}

	void comment(ifstream &in)								//skips all characters until \n
	{
		char c = in.get();
		//if invalid
		while(c != '\n')
		{
			c = in.get();
		}
		number++;
		return;
	}

	string Id(ifstream &in, char c, string &output)										//returns the finished id or keyword
	{
		string str(1, c);
		output += str;
		c = in.get();
		if((c < 48) || ((c > 57) && (c < 65)) || ((c > 90) && (c < 97)) || (c > 122))
		{
			in.putback(c);
		}
		else
		{
			Id(in, c, output);
		}

		return output;
	}

	void check(string s)									//creates new keyword or id
	{
		if(s == "\"Schemes\"")									//SCHEMES
		{
			Token t(SCHEMES, s, number);
			tokens.push_back(t);
		}
		else if(s == "\"Facts\"")								//FACTS
		{
			Token t = Token(FACTS, s, number);
			tokens.push_back(t);
		}
		else if(s == "\"Rules\"")								//RULES
		{
			Token t = Token(RULES, s, number);
			tokens.push_back(t);
		}
		else if(s == "\"Queries\"")								//QUERIES
		{
			Token t = Token(QUERIES, s, number);
			tokens.push_back(t);
		}
		else												//ID
		{
			Token t = Token(ID, s, number);
			tokens.push_back(t);
		}
	}


public:

	vector<Token> tokens;
	int number;
	Token t;
	bool error;
	bool final;

	Scanner()
	{
		number = 1;
		error = false;
		final = false;
	}
	~Scanner()
	{

	}

	void scan(ifstream &in, ofstream &out)
	{
		char c = in.get();
		switch(c)
		{
			case '\n':													//NEWLINE
			{
					number++;
					break;
			}
			case '\t':													//USELESS WHITESPACE
			{
					break;
			}
			case ' ':
			{
					break;
			}
			case '\'':													//STRING
			{
					string quote = Quote(in);
					if(!final)
					{
						t = Token(STRING, quote, number);
						tokens.push_back(t);
					}
					break;
			}
			case '(':													//LEFT PARENTH
			{
					t = Token(LEFT_PAREN, "\"(\"", number);
					tokens.push_back(t);
					break;
			}
			case ')':													//RIGHT PARENTH
			{
					t = Token(RIGHT_PAREN, "\")\"", number);
					tokens.push_back(t);
					break;
			}
			case ',':													//COMMA
			{
					t = Token(COMMA, "\",\"", number);
					tokens.push_back(t);
					break;
			}
			case '.':													//PERIOD
			{
					t = Token(PERIOD, "\".\"", number);
					tokens.push_back(t);
					break;
			}
			case '?':													//Q MARK
			{
					t = Token(Q_MARK, "\"?\"", number);
					tokens.push_back(t);
					break;
			}
			case ':':
			{
					if(arrow(in))										//ARROW
					{
						Token t = Token(COLON_DASH, "\":-\"", number);
						tokens.push_back(t);
					}
					else												//COLON
					{
						Token t = Token(COLON, "\":\"", number);
						tokens.push_back(t);
					}
					break;
			}
			case '#':													//COMMENT
			{
					comment(in);
					break;
			}
			case -1:
			{
				final = true;
				break;
			}

			default:
			{
				error = true;
			}
		}

		if((isalpha(c)))			//ID or KEYWORD
		{
			string output = "\"";
			string id = Id(in, c, output);
			id += "\"";
			check(id);
			error = false;
		}
		else
		{
			if(error && c != -1)
			{
				final = true;
			}
		}

	}
};
