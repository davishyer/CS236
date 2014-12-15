#include "Scanner.h"
#include "Parser.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <istream>

using namespace std;


void startParse(Scanner scan, ostream &out)
{
	Parser parse(scan);

	parse.datalogProgram();

	out << parse.data.toString();
}

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


	if(scanner.final == true)
	{
		//out << "Input Error on line " << scanner.number << "\n";
	}
	else
	{
		//out << "Total Tokens = " << scanner.tokens.size() << endl;
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

