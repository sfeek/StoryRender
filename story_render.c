#include <stdio.h>
#include <stdlib.h>
#include "simplestring.h"
#include "tokenizer.h"
#include "iolibrary.h"

#define MAX_LINES 10000
#define MAX_TOKENS 2
#define MAX_QUESTIONS 50

/* Strip CRLF */
void strip_newline(char *s)
{
    while(*s != '\0')
    {
        if(*s == '\r' || *s == '\n')
        {
            *s = '\0';
        }
        s++;
    }
}

/* Search for Label */
int search_label(char *lines[], int lns, char *search)
{
	int i;

	/* Make sure there is somthing to seach for */
	if (search == NULL) return -1;
	if (search[0]=='\0') return -1;
	if (lines == NULL) return -1;
	if (lns == 0) return -1;

	for(i=0;i<lns;i++)
	{
		if (lines[i][0] == ':') /* Check if it is a label */
		{
			if (strstr(lines[i],search) != NULL) return i+1; /* Found a match, return the line next line */
		}
	}

	/* Did not find a match */
	return -1;
}

int main(int argc, char *argv[])
{
	char *lines[MAX_LINES];
	char *t[MAX_TOKENS];
	char *labels[MAX_QUESTIONS];
	char *line = NULL;
	size_t len = 0;
	size_t read;
	int w,i,l,x,q,tc,nextspace,token_flag,input;
	FILE *fp;
	
	if (argc != 2) return 1; /* Make sure we have the right number of arguments */

	/* Open the file */
	fp = fopen(argv[1],"r");
	if (fp == NULL) return 1; /* Make sure that file was opened properly */

	/* Read in all the lines */
	for(i = 0;i < MAX_LINES; i++)
	{
		read = getline(&line,&len,fp);
		if (read == -1) break; /* Break out when reaching the end of the file */
				
		strip_newline(line); /* Strip new line and carriage returns */

		if (strlen(line) < 1) strcpy(line," "); /* Make a single space for empty lines */

		lines[i] = NULL;
		if (safe_copy(&lines[i],line)) /* Keep the good lines */
		{
			printf("Copy error occurred!\n");
			return 1;
		}

		nextspace = 0;
		for(w=1;w<strlen(lines[i]);w++) /* Make nice Line Wraps to 80 columns-ish */
		{
			if (w%80 == 0) nextspace = 1;
			if (lines[i][w] == ' ')
			{
				if (nextspace == 1)
				{
					lines[i][w] = '\n';
					nextspace = 0;
				}
			}
		}
	}

	fclose(fp); /* Close the file */
	free(line); /* Free the line buffer */

	/* Process the story */
	x=0;
	while(x<i)
	{
		if (lines[x][0] == ':') {x++; continue;} /* Skip labels */

		if (lines[x][0] == '#') /* Process a goto */
		{
			x = search_label(lines,i,lines[x]+1); /* Search for the label, set x to next line */
			
			if (x<0)
			{
				for(x=0;x<i;x++) free(lines[x]); /* Free the lines*/	
				printf("\nError in story logic - could not find label!\n");
				return 1;
			}
			continue;
		}

		if (lines[x][0] == '?') /* Look for questions */
		{
			q = 0;
			while(lines[x][0] != ':') /* Skip until we find another label */
			{
				if (x==i-1) /* Did we hit the end of the file?*/
				{
					for(w=0;w<i;w++) free(lines[w]);/* Free up the lines*/	
					for(l=1;l<=q;l++) free(labels[l]); /* Free the labels */
					printf("\nStory Logic Error, missing questions!\n"); 
					return 1; 				
				}

				if (lines[x][0] == '?') /* Look for more questions */
				{
					q++;
					
					if (q >= MAX_QUESTIONS) /* Don't exceed maximum number of questions */
					{
						for(x=0;x<i;x++) free(lines[x]);/* Free up the lines*/	
						for(l=1;l<q;l++) free(labels[l]); /* Free the labels */
						printf("\nStory Logic Error, too many questions!\n"); 
						return 1; 
					}

					token_flag = tokenize(t,lines[x]+1,MAX_TOKENS,":"); /* Separate question and label */

					if (token_flag == 2) /* Successfully tokenized */
					{
						printf("%d. %s\n",q,t[0]); /* Show the questions */
						
						labels[q]=NULL;
						safe_copy(&labels[q],t[1]); /* Keep the label */
						for(tc=0;tc<token_flag;tc++) free(t[tc]); /* Free the tokens */
					}
					else
					{
						for(tc=0;tc<token_flag;tc++) free(t[tc]); /* Free the tokens */
						for(x=0;x<i;x++) free(lines[x]); /* Free the lines*/	
						printf("\nError in story logic - missing label!\n");
						return 1;
					}
				}
				x++; /* Next line */
			}

			input = 0;
			while (input<1 || input>q) input = getint(""); /* Get the answer */
						
			x = search_label(lines,i,labels[input]); /* Search for the label, set x to next line */
			
			if (x<0)
			{
				for(x=0;x<i;x++) free(lines[x]); /* Free the lines*/	
				for(l=1;l<=q;l++) free(labels[l]); /* Free the labels */
				printf("\nError in story logic - could not find label!\n");
				return 1;
			}

			printf("\n\n"); /* Give some space! */

			for(l=1;l<=q;l++) free(labels[l]); /* Free the labels */
		}
		else /* If not a question, send text to the screen */
		{
			printf("%s\n",lines[x]);
			x++;
		}
	}	 
	
	for(x=0;x<i;x++) free(lines[x]); /* Free the lines*/	
	
	return 0; /* Beavis has left the building! */
}
