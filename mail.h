#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>
extern "C" {
#include "avl.h"
}
#define TABLE_SIZE 1000

using namespace std;

int hashfunc(char* string){
	unsigned long hash = 5381;
	int c;
	while(c = *string++)
		hash = ((hash << 5) + hash) + c;
	return int(hash % TABLE_SIZE);
}

typedef struct avl_node AVL_Node;
typedef struct avl_table AVL_Table;

typedef struct elem_keyword{
	char keyword[50];
}Elem_Keyword;

class Mail{
public:
	char sender[100];
	long long int date;
	int ID;
	char recipient[100];
	vector<Elem_Keyword> keywordlist;
	Mail();
//	AVL_Node SenderNode;
//	AVL_Node DateNode;
//	AVL_Node Recipient;
};

/*
typedef struct keywordnode{
	Mail *mail;
	string keyword;
	struct keywordNode *next;
}KeywordNode;
*/

typedef struct elem_sender{	//the element of Sender_AVL_Tree
	char sender[100];
	vector<int> ID_List;
}Elem_Sender;
/*
typedef struct elem_date{	//the element of Date_AVL_Tree
	long long int date;
	vector<int ID_List;
}Elem_Date;
*/
typedef struct elem_date{	//the element of DateVector
	long long int date;
	int ID;
}Elem_Date;

typedef struct elem_recipient{	//the element of Recipient_AVL_Tree
	char recipient[50];
	vector<int> ID_List;
}Elem_Recipient;

typedef struct elem_ID{		//the element of ID_AVL_Tree
	int ID;
	vector<char*> keywordlist;
	char sender[100];
	char recipient[100];
	long long int date;
}Elem_ID;

typedef struct elem_hashavl{	//the element of HashTable's AVLTree_Element's Element
	char keyword[50];
	vector<int> ID_List;
}Elem_HashAVL;

class Database{
public:
	vector<AVL_Table* > Keyword_HashTable;
//	AVL_Table *Keyword_Table[TABLE_SIZE];
	AVL_Table *SenderTree;
//	AVL_Table *DateTree;
	vector<Elem_Date> Date_Vector;
	AVL_Table *RecipientTree;
	AVL_Table *IDTree;
	int MailCount;

	Database();
	bool exist(int ID);
	void add(FILE* file);
	int remove(int ID);
	void add_keyword(char* keyword);
};