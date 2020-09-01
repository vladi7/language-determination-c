//language_recognition.c
//Author: Aaron Hagy & Vladislav Dubrovenski

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "utf8.h"
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
//structure for language
//count: how many times this language has been found in the text file
//name: stores the name of language
typedef struct language
	{
		unsigned int count;
		char name[50];
	}language;
//cell for the linked list holding the language names
//name: the name of the language
//next: the next cell in the linked list
typedef struct cell
	{
			char * name;
			struct cell * next;
	}typeCell;

//node for the tree structure holding the words
//value: the word
//firstCell: the first cell in the list
//lastCell: the last cell in the list
//leftChild: the left child node
//rightChild: the right child node
typedef struct node
	{
			char *value;
			typeCell *firstCell;
			struct node *leftChild;
			struct node *rightChild;
	}typeNode;


//Array of language structures for counting occurances
struct language languageLibrary[20];

//the number of languages in the library (used as length of library)
int numberOfLanguages = 0;

//a comparison type, comparing two char*
typedef int (*Compare)(const char *, const char*);

//searches linked list of current node, to check if the language is already in the list
//currCell: the current cell to compare
//fileName: the name of the language to compare
int contains(typeCell* currCell, char* fileName)
{
	
	if(currCell==NULL)
	{
		return 1;
	}
	
	else if(strcmp(currCell->name, fileName)==0)
	{
		return 0;
	}
	return contains(currCell->next, fileName);
	
}

//recursively calls printCells, and increments the respective language library count, until it hits a cell=NULL
//thisCell: the current cell to be printed
void printCells(typeCell* thisCell)
{
	int i;
	if(thisCell==NULL)
	{
		return;
	}
	for(i = 0; i< numberOfLanguages; i++)
	{
		if(strcmp(thisCell->name, languageLibrary[i].name)==0)
		{
			languageLibrary[i].count++;
		}
	}
	printCells(thisCell->next);
}

//inserts cell at the end of the linked list
//currCell: the current cell
//fileName: the name of the file
typeCell* insertCell(typeCell* currCell,  char* fileName)
{
	typeCell *thisCell = NULL;
	
	thisCell = (typeCell*) malloc(sizeof(typeCell));
	thisCell->name = strdup(fileName);
	
	thisCell->next=currCell;
	
	return thisCell;
}
//recursively searches the tree for a user input word
//key: the word to search
//leaf: the current node of the tree
//cmp: the comparison variable
void searchTree(char* key, typeNode* leaf)
{
	int i = 0;
	if(leaf==NULL)
	{
		return;
	}	
	int res;
	res = strcmp(key, leaf->value);
        if( res < 0)
		{
			searchTree(key, leaf->leftChild);
		}
        else if( res > 0)
		{
			searchTree(key, leaf->rightChild);
		}
        else     // key found
		{
			printCells(leaf->firstCell);
		}
}

//recursively searches the tree for a user input word, if found then adds the filename to the linked list of that node, if not found then adds a new node with the filename in the linked list
//key: the word to search
//leaf: the current node of the tree
//cmp: the comparison variable
//fileName: the name of the file that contains the key
void insertNode(char* key, typeNode** leaf, char* fileName)
{
	int res;
    if( *leaf == NULL ) 
	{
        (*leaf) = (struct node*) malloc( sizeof( struct node ) );
        (*leaf)->value = malloc( strlen (key) +1 );     // memory for key
        
        strcpy ((*leaf)->value, key);		// copy the key
		(*leaf)->firstCell = (struct cell*) malloc (sizeof(struct cell));
		((*leaf)->firstCell)->name = strdup(fileName);
        (*leaf)->leftChild = NULL;
        (*leaf)->rightChild = NULL;
    } 
	else 
	{
        res = strcmp(key, (*leaf)->value);
        if( res < 0)
		{
			insertNode(key, &(*leaf)->leftChild, fileName);
		}
        else if( res > 0)
		{
			insertNode(key, &(*leaf)->rightChild, fileName);
		}
        else     // key already exists
		{
			if(contains((*leaf)->firstCell, fileName)==1)
			{
				(*leaf)->firstCell = insertCell((*leaf)->firstCell, fileName);
			}
		}			
    }
}



//opens a directory, reads each file in the directory, and, line-by-line, adds each word in each file to the tree structure, 
//adding the name of the file to the linked list each time that word is found in a different file.
//adds the name of the file to the language library, and initializes the counts to 0
//dir: the name of the directory
static void open_dir(const char* dir, typeNode** root)
{
	struct dirent *entry;
	DIR *d = opendir(dir);
	FILE *fp;
	int index = 0;
	
	char currentLine[50];
	if(d ==0)
	{
		perror("opendir");
		return;
	}
	char* token;

	while ((entry = readdir(d)) != 0)
	{
		chdir(dir);
		if ((fp=fopen(entry->d_name, "r"))!=NULL)
			{
				entry->d_name[0]=toupper(entry->d_name[0]);
				token = strtok(entry->d_name, ".");
				if(entry->d_name[0]!= '.')
				{
					strncpy(languageLibrary[numberOfLanguages].name, entry->d_name, 49);
					languageLibrary[numberOfLanguages].count = 0;
					numberOfLanguages++;
			    }
				while(fgets(currentLine, 49, fp))
				{ 
							char x[50];
							sscanf(currentLine, "%s", x);
							insertNode(x, root, token);
							index++;
				}
				fclose(fp);

			}
		
	}
	closedir(d);
}

int main(int argc, char* argv[])
{
	typeNode *root = NULL;
	int i;
	int highestOccurance = 0;
	char result[50];
	char file[300];
	char* token;
	open_dir(argv[1], &root);
	

	while(fgets(file,299,stdin))
	{
		token = strtok (file," ,.-");
			  while (token != NULL)
			  {
				searchTree(token, root);
				
				token = strtok (NULL, " ,.-");
			  }
	}
	for(i = 0; i < numberOfLanguages; i++)
	{
		if(languageLibrary[i].count > highestOccurance)
		{
			highestOccurance = languageLibrary[i].count;
			strncpy(result, languageLibrary[i].name, 49);
		}
	}
	printf("%s\n", result);
	
return 0;
}
