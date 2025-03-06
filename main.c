#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "brainfuck.h"

interpreter_t interp;
int
init_interpreter(char *filename, size_t tape_len, size_t loop_limit)
{
	interp = (interpreter_t)
	{
		.instr_ptr = 0,

		.tape_len = tape_len,
		.tape_i = 0,

		.lstack_len = loop_limit,
		.lstack_i = 0,

		.loop_depth = 0,
		.copying = false,
		.n_copied = 0
	};

	/* * * * * * * * * * * * * *
	 * Initialize tape memory  *
	 * * * * * * * * * * * * * */
	interp.tape = malloc(sizeof(TAPE_TYPE) * interp.tape_len);
	if (interp.tape == NULL)
		return -1;
	memset(interp.tape, 0, sizeof(TAPE_TYPE) * interp.tape_len);

	/* * * * * * * * * * * * *
	 * Initialize loop stack *
	 * * * * * * * * * * * * */
	interp.loop_stack = malloc(sizeof(size_t) * interp.lstack_len);
	if (interp.loop_stack == NULL)
		return -1;

	/* * * * * * * * * * * * * *
	 * Initialize instructions *
	 * * * * * * * * * * * * * */
	FILE *fp = fopen(filename, "r");
	if (fp == NULL)
		return -1;

	// Calculate file size
	fseek(fp, 0, SEEK_END);
	interp.instr_len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	interp.instr = malloc(interp.instr_len);
	if (interp.instr == NULL)
		return -1;

	int bytes_read =
		fread(interp.instr, 1, interp.instr_len, fp);
	if (bytes_read != interp.instr_len)
		return -1;

	fclose(fp);
	return 0;
}
void
destroy_interpreter()
{
	free(interp.instr);
	interp.instr = NULL;
	free(interp.tape);
	interp.tape = NULL;
	free(interp.loop_stack);
	interp.loop_stack = NULL;
}
int
step()
{
	char ch = interp.instr[interp.instr_ptr];

	if (interp.copying)
	{
		// Stop copying
		if (ch == '"')
		{
			interp.copying = false;
			interp.tape[interp.tape_i] = interp.n_copied;
			interp.n_copied = 0;
			goto end_step;
		}

		// Copy current character
		interp.tape[interp.tape_i] = ch;
		interp.tape_i++;
		interp.n_copied++;
		goto end_step;
	}

	if (interp.loop_depth > 0)
	{
		if (ch == '[')
			interp.loop_depth++;
		if (ch == ']')
			interp.loop_depth--;
		goto end_step;
	}

	switch (ch)
	{
		case '>':
			interp.tape_i++;
			break;
		case '<':
			interp.tape_i--;
			break;
		case '+':
			interp.tape[interp.tape_i]++;
			break;
		case '-':
			interp.tape[interp.tape_i]--;
			break;
		case ',':
			printf("(%ld)>", interp.tape_i);

			interp.tape[interp.tape_i] = getchar();
			getchar(); // get rid of \n
			break;
		case '.':;
			putchar(interp.tape[interp.tape_i]);
			break;
		case '[':
			if (interp.tape[interp.tape_i] == 0)
			{
				interp.loop_depth++;
				break;
			}

			interp.loop_stack[interp.lstack_i] = interp.instr_ptr-1;
			interp.lstack_i++;
			break;
		case ']':
			interp.lstack_i--;
			interp.instr_ptr = interp.loop_stack[interp.lstack_i];
			break;
		case '#':
			show_memory();
			break;
		case '"':
			interp.copying = true;
			break;
	}

end_step:
	interp.instr_ptr++;
	return interp.instr_ptr >= interp.instr_len;
}
void
run()
{
	while ( !step() ) {};
}
void show_memory()
{
	for (int i = 0; i < interp.tape_len; i++)
	{
		printf("%02x", interp.tape[i]);

		// Show current pointer position
		if (i == interp.tape_i)
			putchar('<');
		else
			putchar(' ');

		// Wrap every 16 characters
		if ((i+1) % 16 == 0)
			putchar('\n');
	}
	putchar('\n');
}

int
main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Usage: %s <file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int success =
		init_interpreter(argv[1], 256, 16);
	if (success != 0)
	{
		perror(argv[0]);
		exit(EXIT_FAILURE);
	}

	run();

	destroy_interpreter();
	exit(EXIT_SUCCESS);
}
