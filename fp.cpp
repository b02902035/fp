#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <vector>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include "mail.h"


using namespace std;

int main(void) {
	char operation[10];
	Database database;
	int i;
	while (scanf("%s", operation) != EOF ){
		switch(operation[0]){
			/*add <file>: add a mail into database, where <file> is the path of the file that stores a mail*/
			case 'a':
				char filepath[1000];
				scanf("%s", filepath);
//				ifstream file;
//				file.open(filepath);
				FILE* file;
				file = fopen(filepath, "r");
				database.add(file);
				cout << database.MailCount << endl;
				break;
				
			/*remove <id>: remove mail from database, where <id> is the unique positive Message-ID of a mail*/
			case 'r':
				//int ID;
				//scanf("%d", &ID);	
				//database.remove(ID);
				break;
				
			/*query [-f"<from>"] [-t"<to>"] [-d<date1>~<date2>] <keyword-expression>:return all the Message-ID of mails that fits the query description*/
			case 'q':
				break;
				
			default:
				break;
		}	
	}	
	return 0;
}