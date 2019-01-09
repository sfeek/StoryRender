#include <stdio.h>
#include <stdlib.h>
#include "simplestring.h"
#include "tokenizer.h"
#include "iolibrary.h"

#define MAX_LINES 10000
#define MAX_TOKENS 2
#define MAX_QUESTIONS 50

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

/* Strip CRLF */
void strip_newline (char *s)
{
	while (*s != '\0')
	{
		if (*s == '\r' || *s == '\n')
		{
			*s = '\0';
		}

		s++;
	}
}

/* Search for Label */
int search_label (char *lines[], int lns, char *search)
{
	int i;

	/* Make sure there is somthing to seach for */
	if (search == NULL)
		return -1;

	if (search[0] == '\0')
		return -1;

	if (lines == NULL)
		return -1;

	if (lns == 0)
		return -1;

	for (i = 0; i < lns; i++)
	{
		if (lines[i][0] == ':') /* Check if it is a label */
		{
			if (strstr (lines[i], search) != NULL)
				return i + 1; /* Found a match, return the line next line */
		}
	}

	/* Did not find a match */
	return -1;
}

/* Program Main Loop */
int main (int argc, char *argv[])
{
	char *lines[MAX_LINES];
	char *t[MAX_TOKENS];
	char *labels[MAX_QUESTIONS];
	char *line = NULL;
	size_t len = 0;
	size_t read;
	int w, i, l, x, q, nextspace, token_flag, input;
	FILE *fp;

	/* Start with NULL buffers */
	for (l = 0; l < MAX_LINES; l++)
		lines[l] = NULL;

	for (l = 0; l < MAX_TOKENS; l++)
		t[l] = NULL;

	for (l = 0; l < MAX_QUESTIONS; l++)
		labels[l] = NULL;

	if (argc != 2)
		return 0; /* Make sure we have the right number of arguments */

	/* Open the file */
	fp = fopen (argv[1], "r");

	if (fp == NULL)
		return 0; /* Make sure that file was opened properly */

	/* Read in all the lines */
	for (i = 0; i < MAX_LINES; i++)
	{
		read = getline (&line, &len, fp);

		if (read == -1)
			break; /* Break out when reaching the end of the file */

		strip_newline (line); /* Strip new line and carriage returns */

		if (strlen (line) < 1)
			strcpy (line, " "); /* Make a single space for empty lines */

		if (safe_copy (&lines[i], line)) /* Keep the good lines */
		{
			printf ("String copy error occurred!\n");
			goto exit_abort;
		}

		nextspace = 0;
		l = strlen (lines[i]);

		for (w = 1; w < l; w++) /* Make nice Line Wraps to 80 columns-ish */
		{
			if (w % 80 == 0)
				nextspace = 1;

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

	/* Process the story */
	x = 0;

	while (x < i)
	{
		if (lines[x][0] == ':')
		{
			x++;    /* Skip labels */
			continue;
		}

		if (lines[x][0] == '#') /* Process a goto */
		{
			x = search_label (lines, i, lines[x] + 1); /* Search for the label, set x to next line */

			if (x < 0)
			{
				printf ("\nStory Logic Error, missing label!\n");
				goto exit_abort;
			}

			continue;
		}

		if (lines[x][0] == '?') /* Look for questions */
		{
			q = 0;

			while (lines[x][0] != ':') /* Skip until we find another label */
			{
				if (x == i - 1) /* Did we hit the end of the file?*/
				{
					printf ("\nStory Logic Error, missing questions!\n");
					goto exit_abort;
				}

				if (lines[x][0] == '?') /* Look for more questions */
				{
					if (q >= MAX_QUESTIONS) /* Don't exceed maximum number of questions */
					{
						printf ("\nStory Logic Error, too many questions!\n");
						goto exit_abort;
					}

					token_flag = tokenize (t, lines[x] + 1, MAX_TOKENS, ":"); /* Separate question and label */

					if (token_flag == 2) /* Successfully tokenized */
					{
						if (q == 0)
							printf ("\n"); /* Blank line before first answer */

						printf ("%s%d. %s%s%s\n", KYEL, q + 1, KCYN, t[0], KNRM); /* Show the questions */
						safe_copy (&labels[q], t[1]); /* Keep the label */

						for (l = 0; l < token_flag; l++)
						{
							free (t[l]);    /* Free the tokens */
							t[l] = NULL;
						}
					}
					else
					{
						printf ("\nStory Logic Error, missing label!\n");
						goto exit_abort;
					}

					q++; /* Next question */
				}

				x++; /* Next line */
			}

			input = 0;

			while (input < 1 || input > q)
				input = getint (""); /* Get the answer */

			x = search_label (lines, i, labels[input - 1]); /* Search for the label, set x to next line */

			if (x < 0)
			{
				printf ("\nStory Logic Error, missing label!\n");
				goto exit_abort;
			}

			printf ("\n\n"); /* Give some space! */

			for (l = 0; l < q; l++)
			{
				free (labels[l]);    /* Free the labels */
				labels[l] = NULL;
			}
		}
		else /* If not a question, send text to the screen */
		{
			printf ("%s%s%s\n", KGRN, lines[x], KNRM);
			x++;
		}
	}

exit_abort:

	/* Clean up all dynamically allocated memory */
	for (l = 0; lines[l]; l++)
	{
		free (lines[l]);
		lines[l] = NULL;
	}

	for (l = 0; labels[l]; l++)
	{
		free (labels[l]);
		labels[l] = NULL;
	}

	for (l = 0; t[l]; l++)
	{
		free (t[l]);
		t[l] = NULL;
	}

	free (line);
	line = NULL;

	if (fp)
		fclose (fp); /* Close the file */

	return 0;
}
