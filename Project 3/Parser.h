#pragma once
#include "Scanner.h"
#include "Datalog.h"

using namespace std;



class Parser
{
private:
	Token current;
	Scanner scanner;

public:
	string errorMessage;
	Datalog data;
	PARAMETER paramIn;
	PREDICATE predIn;
	RULE ruleIn;

	Parser(Scanner scan)
	{
		scanner = scan;
		int temp = scanner.tokens[scanner.tokens.size() - 1].line;
		//temp++;
		Token end = Token(ENDFILE, "endfile", temp);
		scanner.tokens.push_back(end);
	}
	~Parser()
	{

	}

	void error()										//error statement
	{
		errorMessage = "Failure!\n  (";
		errorMessage += current.getKind();
		errorMessage += ",\"";
		string temp = current.getValue();
		if(temp != "endfile")
		{
			int l = temp.length();
			temp = temp.substr(1, l-2);
		}
		errorMessage += temp;
		errorMessage += "\",";
		stringstream ss;
		ss << current.line;
		temp = ss.str();
		errorMessage += temp;
		errorMessage += ")\n";
		throw errorMessage;
	}

	void match(string t)								//tests for a matching kind
	{
		if(current.getKind() == t)						//if a match is found, move to the next token
		{
			current = getToken();
		}
		else
		{
			error();
		}
	}

	Token getToken()									//sets the current token to be the next in line
	{
		scanner.tokens.erase(scanner.tokens.begin());
		return scanner.tokens[0];
	}

	void datalogProgram()								//starts from the first token
	{
		current = scanner.tokens[0];
		for(unsigned i = 0; i < scanner.tokens.size(); i++)
		{
			if(scanner.tokens[i].getKind() == "STRING")
			{
				string temp = scanner.tokens[i].getValue();
				int l = temp.length();
				temp = temp.substr(1, l-2);
				string thisone = temp;
				data.domain.insert(thisone);
			}
		}
		current = scanner.tokens[0];
		match("SCHEMES");
		match("COLON");
		schemeList();

		match("FACTS");
		match("COLON");
		factList();

		match("RULES");
		match("COLON");
		ruleList();

		match("QUERIES");
		match("COLON");
		queryList();

		match("ENDFILE");
	}

	void schemeList()									//follows grammar for scheme list
	{
		scheme();
		if(current.getKind() == "ID")
		{
			schemeList();
		}
	}

	void scheme()										//follows grammar for scheme
	{
		predicate();
		data.schemes.push_back(predIn);
		predIn.parameters.clear();
	}

	void factList()										//follows grammar for fact list
	{
		if(current.getKind() == "ID")
		{
			fact();
			factList();
		}
	}

	void fact()											//follows grammar for fact
	{
		predicate();
		data.facts.push_back(predIn);
		predIn.parameters.clear();
		match("PERIOD");
	}

	void ruleList()										//follows grammar for rule list
	{
		if(current.getKind() == "ID")
		{
			rule();
			data.rules.push_back(ruleIn);
			ruleList();
		}
	}

	void rule()											//follows grammar for rule
	{
		predicate();
		ruleIn.head = predIn;
		match("COLON_DASH");
		ruleIn.body.clear();
		predicateList();
		match("PERIOD");
	}

	void queryList()									//follows grammar for query list
	{
		query();
		data.queries.push_back(predIn);
		predIn.parameters.clear();
		if(current.getKind() == "ID")
		{
			queryList();
		}
	}

	void query()										//follows grammar for query
	{
		predicate();
		match("Q_MARK");
	}

	void predicateList()								//follows grammar for predicate list
	{
		predIn.parameters.clear();
		predicate();
		ruleIn.body.push_back(predIn);
		predIn.parameters.clear();
		if(current.getKind() == "COMMA")
		{
			match("COMMA");
			predicateList();
		}
	}

	void predicate()									//follows grammar for predicate
	{
		string temp = current.getValue();
		int l = temp.length();
		temp = temp.substr(1, l-2);
		predIn.NAME = temp;
		match("ID");
		match("LEFT_PAREN");
		parameterList();
		match("RIGHT_PAREN");
	}

	void parameterList()								//follows grammar for parameter list
	{
		parameter();
		paramIn.hasName = false;
		paramIn.hasValue = false;
		if(current.getKind() == "COMMA")
		{
			match("COMMA");
			parameterList();
		}
	}

	void parameter()									//follows grammar for parameter
	{

		if(current.getKind() == "STRING")
		{
			paramIn.hasName = true;
			string temp = current.getValue();
			int l = temp.length();
			temp = temp.substr(1, l-2);
			paramIn.name = temp;
			predIn.parameters.push_back(paramIn);
			paramIn.name = "";
			match("STRING");
			paramIn.hasName = true;
		}
		else if(current.getKind() == "ID")
		{
			paramIn.hasValue = true;
			string temp = current.getValue();
			int l = temp.length();
			temp = temp.substr(1, l-2);
			paramIn.value = temp;
			predIn.parameters.push_back(paramIn);
			paramIn.value = "";
			match("ID");
		}
		else
		{
			error();
		}
	}
};
