#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef uint8_t TAPE_TYPE;
typedef struct
{
	char *instr;
	size_t instr_len;
	size_t instr_ptr;

	TAPE_TYPE *tape;
	size_t tape_len;
	size_t tape_i;

	size_t *loop_stack;
	size_t lstack_len;
	size_t lstack_i;

	size_t loop_depth; // TODO: find a more suitable name
	bool copying;
	size_t n_copied;
} interpreter_t;
extern interpreter_t interp;

int
init_interpreter(char *filename, size_t tape_len, size_t loop_limit);
void
destroy_interpreter();

int
step();
void
run();

void
show_memory();
