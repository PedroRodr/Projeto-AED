/*
Projeto Final AED
  93780 - João Marques
  93783 - José Oliveira
  94062 - Pedro Rodrigues
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define VALID_CHAR ((('0' <= letter) && (letter <= '9')) || (('a' <= letter) && (letter <= 'z')) || (('A' <= letter) && (letter <= 'Z')))
#define MAXC_SIZE 70
#define HASH_COMP 2*('z'-'0')
#define POSITIVE 1
#define NEGATIVE 0

typedef struct table		/*This structure will be used when there is a collision on the hash table*/
{
  struct table *prev;		/*these pointers are used to create a list*/
  struct table *next;
  char* word_one;		/*these two pointers contain the two words, in numerical order*/
  char* word_two;
  int counter;			/*counter cointains the number of times the pair is found on the file*/
}TABLE;


void set_hash(TABLE** hash);
void get_arguments(int *top, int *num_words, FILE* fp);
char *get_name(char *file_name);
void insertion(FILE *fp, TABLE** hash);
void insert_in_hash(char* p1,int size_a,char* p2,int size_b, TABLE** hash);
void insert_block_hash(TABLE** hash, int sum, char* p1, char* p2);
void bubble_sort(TABLE* insert, TABLE* top_n[], int tamanho);
void top_n_function(TABLE** hash, int top_n, FILE* fp_out);
TABLE* create_block(char *p1, char *p2);
void writes_in_file(TABLE** top_n_p, FILE* fp, int top_n);
void free_time(FILE* fp_in, FILE* fp_out, char*fp_name, TABLE** hash);

int main(int argc,char* argv[])
{
  FILE *fp_in, *fp_out;			/*fp_in is the file that is being read, while fp_out is the file where the results will be written*/
  char* name_output;			/*name_output is used to create the fp_out*/
  int top_n, num_diff_words;		/*top_n and num_diff_words will be extracted from the file*/
  TABLE* hash[MAXC_SIZE*HASH_COMP];	/*the hash table contains pointers to a structure*/

  set_hash(hash);

  if((fp_in = fopen(argv[1], "r")) == NULL)	/*the program ends if there isn't any file to read*/
  {
    printf("Nenhum ficheiro escolhido.\n");
    return 0;
  }

  name_output = get_name(argv[1]);		
  fp_out = fopen(name_output, "w");
  get_arguments(&top_n, &num_diff_words, fp_in);

  insertion(fp_in, hash);			/*this function is the fundamental part of the program*/
  top_n_function(hash, top_n, fp_out);		/*this will write the results on the new file*/

  fclose(fp_in);
  fclose(fp_out);

  free_time(fp_in, fp_out, name_output, hash);	/*frees all allocated memory*/

return 0;
}

void set_hash(TABLE** hash)			 /*sets all hash pointers to NULL*/
{
  int i;

  for(i = 0; i < MAXC_SIZE*HASH_COMP; i++)
    hash[i] = NULL;
return;
}


char *get_name(char *file_name){		/*get_name will utilize the name of the file being read to the outpute file, in which insead of 						ending with ".txt" it ends with .sol*/
  int i;
  int length = strlen(file_name);
  char *file_name_out;
  char *extension = ".sol";

  file_name_out = (char*) malloc((length+1)*sizeof(char)); 	/*copies the name of the file, excluding the ".txt" */
  for(i = 0; i < length - 4; i++){
  file_name_out[i] = file_name[i];
  }
  strncat(file_name_out, extension, 5);				/*adds the extension ".sol"*/

return file_name_out;
}

void get_arguments(int *top, int *num_words, FILE* fp) /*gets the first two numbers from the file*/
{
  fscanf(fp,"%d %d",top, num_words);

return;	
}

void insertion(FILE *fp, TABLE** hash)			/*this function gets all the pairs in the file and inserts them into the hash table*/
{
  char word_a[MAXC_SIZE], word_b[MAXC_SIZE], letter;			/*word_a and word_b is the pair that will be inserted*/
  int size_a = 0, size_b, i = 0;					/*contains the size of each word*/
  char *p1, *p2, *p3;							/*pointers that will be used to trade word_a and word_b*/

  p1 = word_a;
  p2 = word_b;

  letter = fgetc(fp);

  while(!VALID_CHAR || letter == '\n')					/*this cycle will get the first valid character*/
    letter = fgetc(fp);
  letter = tolower(letter);						/*all our word will have lowered characters*/
  p1[i] = letter;
  i++;
  size_a++;
  for( ; ((letter = fgetc(fp)) != EOF ) && VALID_CHAR; i++, size_a++) 	/*this cycle will get our first word*/
  {
    letter = tolower(letter);
    p1[i] = letter;
  }
  size_a++;
  p1[i] = '\0'; 							/*to this point we do have our first word*/

  while(letter != EOF)							/*this cycle will get all pairs of words*/
  {
    i = 0;
    size_b = 0;

    letter = fgetc(fp);
    while(!VALID_CHAR && letter != EOF)					/*this cycle gets our fisrt valid character of the second word*/
      letter = fgetc(fp);
    letter = tolower(letter);
    p2[i] = letter;
    i++;
    size_b++;
    for(; (letter = fgetc(fp)) != EOF && VALID_CHAR; i++, size_b++)	/*this cycle gets our second word*/
    {
      letter = tolower(letter);
      p2[i] = letter;      
    }
    size_b++;
    p2[i] = '\0';
    if(letter == EOF)							/*if the last character was EOF there isn't a pair*/
      return;
    insert_in_hash(p1, size_a, p2, size_b, hash);			/*the pair is inserted into the hash table*/
    p3 = p1;								/*this part makes the second word the first word and the cycle will get*/
    p1 = p2;								/*the new second word*/
    p2 = p3;
    size_a = size_b;
  }
return;
}

void insert_in_hash(char* p1, int size_a, char* p2, int size_b, TABLE** hash) 	/*this function will insert or increment a pair of words */
{										/*or create a new block of words on the hash table*/
  int sum = 0;
  int i;

  for(i = 0; i < size_a - 1; i++)		/*The hash key is the sum of the ASCII value of the characters of both words*/
    sum = sum + (int)p1[i] - 48;		/*it is subtracted by the ASCII of 0 because the that's the lowest value we'll use*/
  for(i = 0; i < size_b - 1; i++)
    sum = sum + (int)p2[i] - 48;

  insert_block_hash(hash, sum, p1, p2);		/*this increments the counter, or makes a new block*/
      
return;
}

void insert_block_hash(TABLE** hash, int sum, char* p1, char* p2)	/*this function decides what is done with the pairof words*/
{
  TABLE* p_block_reference = NULL;
  TABLE* p_block_supp;

  if(hash[sum] == NULL)				/*if the position is pointing to NULL, we create a new block containing the pair*/
  {
    p_block_reference = create_block(p1, p2);
    hash[sum] = p_block_reference;		/*now the position isn't poining to NULL*/
  }
  else						/*if there is already a pair of words on the hash table*/
  {
    for(p_block_supp = hash[sum]; ; p_block_supp = p_block_supp->next)	/*goes through all the list*/
    {
    	if((strcmp(p1,p_block_supp->word_one) == 0) && (strcmp(p2,p_block_supp->word_two) == 0))/*if this condition is true, we increment*/
    	{
    		p_block_supp->counter++;
    		break;
    	}
    	if(p_block_supp->next == NULL)		/*if the pair of words doesn't exist in the list, we create a block and insert it*/
    	{	
    		p_block_reference = create_block(p1, p2);
    		p_block_reference->prev = p_block_supp;
    		p_block_supp->next = p_block_reference;
    		break;
    	}
    }
  }
return;
}

TABLE* create_block(char *p1, char *p2)	/*this function creates a block which will be used in the hash table*/
{
  TABLE* table_p;
  int strlenp1 = strlen(p1);			/*size of the strings*/
  int strlenp2 = strlen(p2);

  table_p = (TABLE*)malloc(sizeof(TABLE));				/*allocates memory for the block*/
  table_p->word_one = (char*)malloc((strlenp1+1)*sizeof(char));		/*allocates memory and inserts strings*/
  strcpy(table_p->word_one, p1);					
  table_p->word_two = (char*)malloc((strlenp2+1)*sizeof(char));
  strcpy(table_p->word_two, p2);
  table_p->next = NULL;
  table_p->prev = NULL;
  table_p->counter = 1;					/*a block is created when the pair of words is detected once, so counter = 1*/

  return table_p;
}

void top_n_function(TABLE** hash, int top_n, FILE* fp_out)	/*this functions find the top n most frequent sequences*/
{
  TABLE** top_n_p;						/*array of blocks*/						
  TABLE* aux;
  int i;

  top_n_p = (TABLE**)calloc((top_n+1),sizeof(TABLE*));		/*allocates memory for an array with the most n frequent blocks*/

  for(i = 0; i < top_n; i++)					/*initalizes the array with NULL*/
  {
    top_n_p[i] = NULL;
  }
  for(i = 0; i < MAXC_SIZE*HASH_COMP; i++)			/*these two fors are used to go through every list of ever hash position*/
  {
    for (aux = hash[i]; aux != NULL ; aux = aux->next)
    {
      if((top_n_p[0] == NULL) || (aux->counter > top_n_p[0]->counter))	/*if the least frequent of the blocks is less frequent than the one we*/ 
	bubble_sort(aux, top_n_p, top_n);				/*found, we'll use bubble sort*/
    }
  }
  writes_in_file(top_n_p, fp_out, top_n);		/*here we'll write the results on the file*/

  free(top_n_p);					/*frees alloced memory*/

return;
}

void bubble_sort(TABLE* insert, TABLE* top_n[], int tamanho)		/*this function will use the sort the array*/
{									/*it is ordered from the last position to 0 in decrescent order*/
  int i;
  TABLE* auxiliar;

  top_n[0] = insert;
  for(i = 1; (top_n[i] == NULL) && (i < tamanho); i++)			/*this for will replace any position with NULL, and bubble sort*/
  { 
    auxiliar = top_n[i-1];
    top_n[i-1] = top_n[i];
    top_n[i] = auxiliar;
  }
  for(;(i < tamanho) && (top_n[i-1]->counter > top_n[i]->counter);i++)	/*this for is when we have to bubble sort positions diffrent than NULL*/
  {
    auxiliar = top_n[i-1];
    top_n[i-1] = top_n[i];
    top_n[i] = auxiliar;
  }
return;
}
void writes_in_file(TABLE** top_n_p, FILE* fp, int top_n) 	/*this function will write the results in the file*/
{
  int i;
 
  for(i = top_n - 1 ; i >= 0; i--)				/*the most frequent pair is on the last position, so the for is decrescent*/
    if(top_n_p[i] != NULL) fprintf(fp, "%d - %d %s %s\n", top_n - i,top_n_p[i]->counter, top_n_p[i]->word_one, top_n_p[i]->word_two);
return;
}

void free_time(FILE* fp_in, FILE* fp_out, char*fp_name, TABLE** hash) 	/*this function will free most of the allocated memory*/
{
  TABLE *free_p, *aux_p;
  int i;

  for(i = 0; i < MAXC_SIZE*HASH_COMP; i++)			/*this for will go through all the hash table positions*/
  {
    for(free_p = hash[i]; free_p != NULL; free_p = aux_p)	/*this for is used to free a list of a position of the hash table*/
    {
      aux_p = free_p->next;
      free(free_p->word_one);					/*frees the block and words*/
      free(free_p->word_two);
      free(free_p);
    }
  }

  free(fp_name);						/*frees fp_name*/
return;
}
