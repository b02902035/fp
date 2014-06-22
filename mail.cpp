#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <cctype>
#include <vector>
#include "mail.h"
extern "C" {
#include "avl.h"
}

using namespace std;

// Compares for avl_create
int SenderTree_Compare(const void *pa, const void *pb, void *param){
	Elem_Sender a = *(const Elem_Sender *) pa;
	Elem_Sender b = *(const Elem_Sender *) pb;
	return strcmp(a.sender, b.sender);
}

int DateTree_Compare(const void *pa, const void *pb, void *param){
	Elem_Date a = *(const Elem_Date *) pa;
	Elem_Date b = *(const Elem_Date *) pb;
	if(a.date > b.date)
		return 1;
	else if(a.date == b.date)
		return 0;
	else return -1;	
}

int RecipientTree_Compare(const void *pa, const void *pb, void *param){
	Elem_Recipient a = *(const Elem_Recipient *) pa;
	Elem_Recipient b = *(const Elem_Recipient *) pb;
	return strcmp(a.recipient, b.recipient);
}

int IDTree_Compare(const void *pa, const void *pb, void *param){
	Mail a = *(const Mail *) pa;
	Mail b = *(const Mail *) pb;
	if(a.ID > b.ID)
		return 1;
	else if(a.ID == b.ID)
		return 0;
	else return -1;	
}

int HashAVL_Compare(const void *pa, const void *pb, void *param){
	Elem_HashAVL a = *(const Elem_HashAVL *) pa;
	Elem_HashAVL b = *(const Elem_HashAVL *) pb;
	return strcmp(a.keyword, b.keyword);
}

int Month_StrToInt(char* m){

	if ( m[0] == 'A' ){
			if ( m[1] == 'p' )
				return 4;
			else
				return 8;
		}else if ( m[0] == 'F' ){
				return 2;
		}else if ( m[0] == 'J' ){
			if ( m[1] == 'a' )
				return 1;
			else if ( m[2] == 'n' )
				return 6;
			else
				return 7;		
		}else if ( m[0] == 'M' ){
			if ( m[2] == 'r' )
				return 3;
			else
				return 5;
		}else if ( m[0] == 'S' ){
				return 9;
		}else if ( m[0] == 'O' ){
				return 10;
		}else if ( m[0] == 'N' ){
				return 11;
		}else{
				return 12;
		}
}
//
//void probe(const Mail mail){
//
//}

Database::Database(){
	SenderTree = avl_create(SenderTree_Compare, NULL, NULL);
//	DateTree = avl_create(DateTree_Compare, NULL, NULL);
	RecipientTree = avl_create(RecipientTree_Compare, NULL, NULL);
	IDTree = avl_create(IDTree_Compare, NULL, NULL);
	AVL_Table* hash_elem;
	hash_elem = avl_create(HashAVL_Compare, NULL, NULL);
	for(int i = 0; i < TABLE_SIZE; i++)
		Keyword_HashTable.push_back(hash_elem);		//P_BUG
	MailCount = 0;
}

Mail::Mail(){
	date = 0;
}

bool Database::exist(int ID){
	Elem_ID* tmp;
	tmp->ID = ID;
	if(avl_find(IDTree, tmp) != NULL)
		return true;
	else return false;
}
//
//void Database::add_keyword(char* keyword){
//	this.
//}

void Database::add(FILE* file){
	Mail *mail = new Mail;
	char month_str[50], subject[50];
	int d, y, h, m;			// day, year, hour, minute
	char c;
	int i, hash;

	fscanf(file, "From: %s", mail->sender);
	fscanf(file, "Date: %d %s %d at %d:%d", &d, month_str, &y, &h, &m);

	//change the date information into a single long long int
	mail->date = (long long int) y * 100000000 + Month_StrToInt(month_str) * 1000000
								 + d * 10000 + h * 100 + m;

	fscanf(file, "Message-ID: %d", &(mail->ID));
	
	if((*this).exist(mail->ID)){		//return if the mail is already added
		delete mail;
		cout << "-\n" << endl;
		return;
	}

	//add Sender to SenderTree
	Elem_Sender *sd = new Elem_Sender;
	strcpy(sd->sender, mail->sender);
	(sd->ID_List).push_back(mail->ID);
	if(avl_insert(this->SenderTree, sd) != NULL){
		Elem_Sender d_sender;
		d_sender = *(Elem_Sender *)avl_insert(this->SenderTree, sd);
		(d_sender.ID_List).push_back(mail->ID);
	}

	//add Date to Date_Vector
	Elem_Date *dt = new Elem_Date;
	dt->date = mail->date;
	dt->ID = mail->ID;
	for(vector<Elem_Date>::iterator date_it = (this->Date_Vector).begin(); date_it != (this->Date_Vector).end(); date_it++){
		if(((*date_it).date == dt->date) && (*(date_it++)).ID > dt->ID){
			date_it = (this->Date_Vector).insert(date_it,*dt);
			break;
		}
		else if((*(date_it++)).date > dt->date){
			date_it = (this->Date_Vector).insert(date_it,*dt);
			break;
		}
		else
			continue;
	}


	
	//eat Subject
	fscanf(file, "Subject: ");

	char* tmp = (char* )malloc(sizeof(char) * 50 + 1);
	i = 0;
	while ( ( c = fgetc(file) ) != EOF ){
		
		if ( isalnum(c) ){
			tmp[i] = c;
				i++;
		}
		else{
			tmp[i] = '\0';
			Elem_Keyword *kw = new Elem_Keyword;
			strcpy(kw->keyword, tmp);
			
			// add keyword to keyword_list in Class mail (keyword_list vector sorted)
			vector<Elem_Keyword>::iterator it;
			it = mail->keywordlist.begin();
			for(it = mail->keywordlist.begin(); it != mail->keywordlist.end(); it++){
				if(strcmp(kw->keyword ,(*it).keyword) == 0)
					break;
				else if(strcmp(kw->keyword ,(*it).keyword) < 0){
					it = (mail->keywordlist).insert(it, *kw);
					break;
				}
				else{
					if((it++) == mail->keywordlist.end())
						(mail->keywordlist).push_back(*kw);
				}
			}

			//add keyword to the keyword hashtable (ID_List unsort)
			hash = hashfunc(kw->keyword);
			Elem_HashAVL *elem_hashAVL = new Elem_HashAVL;
			Elem_HashAVL *d_elem;
			strcpy(elem_hashAVL->keyword, tmp);
			(elem_hashAVL->ID_List).push_back(mail->ID);

			d_elem = (Elem_HashAVL* )avl_insert(this->Keyword_HashTable[hash], elem_hashAVL);	//P_BUG: insert() returns pointer?
			
			if(d_elem != NULL){		// if the keyword is already exist, append the ID
				(d_elem->ID_List).push_back(mail->ID);
				delete elem_hashAVL;
			}

			delete kw;
			free(tmp);
			char* tmp = (char* )malloc(sizeof(char) * 50 + 1);
			i = 0;
		}

		if(c == '\n')
			break;
	}
	// end add subject

	fscanf(file, "To: %s", mail->recipient);

	// add recipient
	Elem_Recipient *recp = new Elem_Recipient;
	Elem_Recipient *d_recp;
	strcpy(recp->recipient, mail->recipient);
	(recp->ID_List).push_back(mail->ID);
	d_recp = (Elem_Recipient* )avl_insert(this->RecipientTree, recp);
	if(d_recp != NULL){
		(d_recp->ID_List).push_back(mail->ID);
	}


	// eat content
	fscanf(file, "Content:\n");
	int line_num = 0;
	while ( ( c = fgetc(file) ) != EOF ){
		
		if ( isalnum(c) ){
			tmp[i] = c;
				i++;
		}
		else{
			tmp[i] = '\0';
			Elem_Keyword *kw = new Elem_Keyword;
			strcpy(kw->keyword, tmp);
			
			// add keyword to keyword_list in Class mail (keyword_list vector sorted)
			vector<Elem_Keyword>::iterator it;
			it = mail->keywordlist.begin();
			for(it = mail->keywordlist.begin(); it != mail->keywordlist.end(); it++){
				if(strcmp(kw->keyword ,(*it).keyword) == 0)
					break;
				else if(strcmp(kw->keyword ,(*it).keyword) < 0){
					it = (mail->keywordlist).insert(it, *kw);
					break;
				}
				else{
					if((it++) == mail->keywordlist.end())
						(mail->keywordlist).push_back(*kw);
				}
			}

			//add keyword to the keyword hashtable (ID_List unsort)
			hash = hashfunc(kw->keyword);
			Elem_HashAVL *elem_hashAVL = new Elem_HashAVL;
			Elem_HashAVL *d_elem;
			strcpy(elem_hashAVL->keyword, tmp);
			(elem_hashAVL->ID_List).push_back(mail->ID);

			d_elem = (Elem_HashAVL* )avl_insert(this->Keyword_HashTable[hash], elem_hashAVL);	//P_BUG: insert() returns pointer?
			
			if(d_elem != NULL){		// if the keyword is already exist, append the ID
				(d_elem->ID_List).push_back(mail->ID);
				delete elem_hashAVL;
			}

			delete kw;
			free(tmp);
			char* tmp = (char* )malloc(sizeof(char) * 50 + 1);
			i = 0;
		}

		if(c == '\n'){		//stop eating at the end of last line
			line_num++;
			if(line_num == 2)
				break;
		}
	}
	avl_probe(this->IDTree, mail);
	(this->MailCount)++;
}

