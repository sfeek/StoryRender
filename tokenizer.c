#include "tokenizer.h"

/* Function to split a string into an array of strings by a delimiter
	Input:
		char *[]: to a preallocated array of character pointers
		char *: to buffer containing string to be tokenized
		int: max number of available elements in the token array
		char *: delimiter string

	Output:
		char *[] filled with tokens from the string and a return count of the number of elements filled on success
		returns MAX_TOKENS_REACHED if there are more tokens available than space in the token array to put them
		returns MEMORY_ERROR if there was a problem allocating space for the token array
		if there was an error, memory is automatically freed before returning error flag
		if there is a NULL buffer, NULL_BUFFER is returned and nothing is processed
*/

int tokenize (char *single[], char *input, int max_tokens, char *delimiter)
{
	char *token;
	int index = 0;
	int i;
	int token_flag;
	char *buffer;

	if (single == NULL || input == NULL || delimiter == NULL)
		return NULL_BUFFER;/* Handle NULL parameters */

	if (max_tokens < 1)
		return MAX_TOKENS_REACHED;/* Make sure that we have at least one token */

	max_tokens++;
	buffer = strdup (input); /* Make a copy of the string to be tokenized so that the original is not destroyed */
	token = strtok (buffer, delimiter); /* Get the first token */
	index = 0; /* Reset index counter */
	token_flag = OK; /* Reset token flag */

	while (token != NULL) /* Cycle through until the end of the buffer is reached */
	{
		single[index] = malloc (strlen (token) + 1); /* Make room for a copy of the token, including null terminator */

		if (single[index] == NULL) /* Check if memory allocation failed, if so flag and leave the loop  */
		{
			token_flag = MEMORY_ERROR;
			break;
		}

		strcpy (single[index], token); /* Make copy of the token */
		token = strtok (NULL, delimiter); /* Get the next token */
		index++; /* Increment the index */

		if (index == max_tokens) /* Make sure that we do not exceed token array count, if so flag and leave the loop */
		{
			token_flag = MAX_TOKENS_REACHED;
			break;
		}
	}

	/* Check token result flags, cleanup and return if flags tripped */
	free (buffer); /* Free the copy string */

	if (token_flag == MAX_TOKENS_REACHED)
	{
		for (i = 0; i < index; i++)
			free (single[i]);

		return MAX_TOKENS_REACHED;
	}

	if (token_flag == MEMORY_ERROR)
	{
		for (i = 0; i < index; i++)
			free (single[i]);

		return MEMORY_ERROR;
	}

	return index; /* Return number of strings created if successful */
}
