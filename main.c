/*
 * Define _GNU_SOURCE to enable POSIX/GNU extensions (getline, strdup)
 * while compiling with -std=c99.
 */
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "machine2.h" /* This includes symbols.h for us */

/*
 * Define __DEBUG to enable step-by-step logging to stderr.
 */
#define __DEBUG 1

/*
 * These are now "base" constants.
 * The actual grid size will be computed from stdin.
 */
#define RROWS (2*1)
#define RCOLS 1

/*
 * The TM "head" and "control unit".
 */
typedef struct {
	State curr_state;
	size_t curr_pos;
} Machine;


#ifdef __DEBUG
/*
 * Helper for logging: Converts a State enum to a string.
 */
static const char* state_to_str(State s) {
	switch (s) {
		case STATE_ERROR: return "STATE_ERROR"; 
		case STATE_HALT: return "STATE_HALT";
		case STATE_QUERY_UP: return "STATE_QUERY_UP";
		case STATE_QUERY_RIGHT: return "STATE_QUERY_RIGHT";
		case STATE_QUERY_DOWN: return "STATE_QUERY_DOWN";
		case STATE_QUERY_LEFT: return "STATE_QUERY_LEFT";
		case STATE_RESPOND_UP: return "STATE_RESPOND_UP";
		case STATE_RESPOND_RIGHT: return "STATE_RESPOND_RIGHT";
		case STATE_RESPOND_DOWN: return "STATE_RESPOND_DOWN";
		case STATE_RESPOND_LEFT: return "STATE_RESPOND_LEFT";
		case STATE_GOTO_HOME_UP: return "STATE_GOTO_HOME_UP";
		case STATE_GOTO_TALLY_END_UP: return "STATE_GOTO_TALLY_END_UP";
		case STATE_CHECK_TALLY_UP: return "STATE_CHECK_TALLY_UP";
		case STATE_RETURN_TO_GRID_UP: return "STATE_RETURN_TO_GRID_UP";
		case STATE_MOVE_LEFT_AND_MARK: return "STATE_MOVE_LEFT_AND_MARK";
		case STATE_GOTO_HOME_FOR_RESET_UP: return "STATE_GOTO_HOME_FOR_RESET_UP";
		case STATE_RESET_TALLY_UP: return "STATE_RESET_TALLY_UP";
		case STATE_GOTO_FINAL_POS_UP: return "STATE_GOTO_FINAL_POS_UP";
		case STATE_GOTO_HOME_DOWN: return "STATE_GOTO_HOME_DOWN";
		case STATE_GOTO_TALLY_END_DOWN: return "STATE_GOTO_TALLY_END_DOWN";
		case STATE_CHECK_TALLY_DOWN: return "STATE_CHECK_TALLY_DOWN";
		case STATE_RETURN_TO_GRID_DOWN: return "STATE_RETURN_TO_GRID_DOWN";
		case STATE_MOVE_RIGHT_AND_MARK: return "STATE_MOVE_RIGHT_AND_MARK";
		case STATE_GOTO_HOME_FOR_RESET_DOWN: return "STATE_GOTO_HOME_FOR_RESET_DOWN";
		case STATE_RESET_TALLY_DOWN: return "STATE_RESET_TALLY_DOWN";
		case STATE_GOTO_FINAL_POS_DOWN: return "STATE_GOTO_FINAL_POS_DOWN";
		case STATE_GOTO_HOME_UP_RETURN: return "STATE_GOTO_HOME_UP_RETURN";
		case STATE_GOTO_TALLY_END_UP_RETURN: return "STATE_GOTO_TALLY_END_UP_RETURN";
		case STATE_CHECK_TALLY_UP_RETURN: return "STATE_CHECK_TALLY_UP_RETURN";
		case STATE_RETURN_TO_GRID_UP_RETURN: return "STATE_RETURN_TO_GRID_UP_RETURN";
		case STATE_MOVE_LEFT_AND_MARK_RETURN: return "STATE_MOVE_LEFT_AND_MARK_RETURN";
		case STATE_GOTO_HOME_FOR_RESET_UP_RETURN: return "STATE_GOTO_HOME_FOR_RESET_UP_RETURN";
		case STATE_RESET_TALLY_UP_RETURN: return "STATE_RESET_TALLY_UP_RETURN";
		case STATE_GOTO_FINAL_POS_UP_RETURN: return "STATE_GOTO_FINAL_POS_UP_RETURN";
		case STATE_GOTO_HOME_DOWN_RETURN: return "STATE_GOTO_HOME_DOWN_RETURN";
		case STATE_GOTO_TALLY_END_DOWN_RETURN: return "STATE_GOTO_TALLY_END_DOWN_RETURN";
		case STATE_CHECK_TALLY_DOWN_RETURN: return "STATE_CHECK_TALLY_DOWN_RETURN";
		case STATE_RETURN_TO_GRID_DOWN_RETURN: return "STATE_RETURN_TO_GRID_DOWN_RETURN";
		case STATE_MOVE_RIGHT_AND_MARK_RETURN: return "STATE_MOVE_RIGHT_AND_MARK_RETURN";
		case STATE_GOTO_HOME_FOR_RESET_DOWN_RETURN: return "STATE_GOTO_HOME_FOR_RESET_DOWN_RETURN";
		case STATE_RESET_TALLY_DOWN_RETURN: return "STATE_RESET_TALLY_DOWN_RETURN";
		case STATE_GOTO_FINAL_POS_DOWN_RETURN: return "STATE_GOTO_FINAL_POS_DOWN_RETURN";
		default: return "??? UNKNOWN STATE ???";
	}
}

/*
 * Helper for logging: Converts a Symbol enum to its char.
 */
static char symbol_to_str(Symbol s) {
	/* We can just re-use the main helper */
	return symbol_to_char(s);
}
#endif


int main(void) {
	/* --- Part 1: Tape Initialization (Dynamic) --- */
	
	/* 1. Read first line to get puzzle width */
	char* line_buf = NULL;
	size_t line_buf_cap = 0;
	ssize_t line_len = getline(&line_buf, &line_buf_cap, stdin);
	
	if (line_len <= 0) {
		fprintf(stderr, "Error: Could not read first line from stdin.\n");
		return EXIT_FAILURE;
	}
	
	/* Remove trailing newline */
	if (line_buf[line_len - 1] == '\n') {
		line_buf[line_len - 1] = '\0';
		line_len--;
	}
	
	size_t PROWS = 0;
	const size_t PCOLS = line_len;
	const size_t NCOLS = PCOLS + RCOLS;
	
	/* 2. Store grid lines temporarily */
	char** grid_lines = NULL;
	
	/* Store the first line */
	grid_lines = malloc(sizeof(char*));
	grid_lines[0] = strdup(line_buf);
	PROWS++;
	
	/* Read remaining lines */
	while ((line_len = getline(&line_buf, &line_buf_cap, stdin)) > 0) {
		if (line_len == 0 || line_buf[0] == '\n') break; /* Stop on empty line */
		
		/* Remove trailing newline */
		if (line_buf[line_len - 1] == '\n') {
			line_buf[line_len - 1] = '\0';
			line_len--;
		}
		
		/* Check for consistent width */
		if ((size_t)line_len != PCOLS) {
			fprintf(stderr, "Error: Inconsistent line length. Saw %zu, expected %zu\n",
				(size_t)line_len, PCOLS);
			/* ... (free memory) ... */
			return EXIT_FAILURE;
		}
		
		PROWS++;
		grid_lines = realloc(grid_lines, PROWS * sizeof(char*));
		grid_lines[PROWS - 1] = strdup(line_buf);
	}
	free(line_buf); /* Done with getline buffer */
	
	
	/* 3. Calculate all tape dimensions */
	const size_t NROWS = PROWS + RROWS;
	const size_t TALLY_SIZE = NCOLS;
	const size_t TAPE_SCRATCH_SIZE = TALLY_SIZE + 2; /* @ + Tally + E */
	const size_t TAPE_GRID_OFFSET = TAPE_SCRATCH_SIZE;
	const size_t TAPE_GRID_SIZE = NROWS * NCOLS;
	const size_t TAPE_TOTAL_SIZE = TAPE_SCRATCH_SIZE + TAPE_GRID_SIZE + 1;

	/* 4. Allocate and build the final tape */
	char* tape = malloc(TAPE_TOTAL_SIZE);
	if (tape == NULL) {
		fprintf(stderr, "Error: Failed to allocate tape memory.\n");
		return EXIT_FAILURE;
	}
	
	/* Init scratchpad */
	memset(tape, '.', TAPE_SCRATCH_SIZE);
	tape[0] = '@';
	tape[TAPE_SCRATCH_SIZE - 1] = 'E'; /* Tally End marker */

	/* Init grid boundaries (Top and Bottom) */
	memset(tape + TAPE_GRID_OFFSET, 'R', NCOLS);
	memset(tape + TAPE_GRID_OFFSET + (NROWS - 1) * NCOLS, 'R', NCOLS);

	/* Copy puzzle grid lines into the tape */
	for (size_t i = 0; i < PROWS; i++) {
		size_t tape_row_start = TAPE_GRID_OFFSET + (i + 1) * NCOLS;
		/* *** BUG FIX: Only add the LEFT boundary *** */
		tape[tape_row_start] = 'R'; /* Left boundary */
		strncpy(tape + tape_row_start + 1, grid_lines[i], PCOLS);
		
		free(grid_lines[i]); /* Free the temp line */
	}
	free(grid_lines); /* Free the temp array */
	
	tape[TAPE_TOTAL_SIZE - 1] = '\0'; /* Null-terminate */

	
	/* 5. Find guard and init machine */
	Machine machine;
	char* guard_char = strpbrk(tape, "^>v<");
	if (guard_char == NULL) {
		fprintf(stderr, "Error: No guard (^, >, v, <) found on map.\n");
		free(tape);
		return EXIT_FAILURE;
	}
	
	machine.curr_pos = guard_char - tape;
	
	switch (*guard_char) {
		case '^': machine.curr_state = STATE_QUERY_UP; break;
		case '>': machine.curr_state = STATE_QUERY_RIGHT; break;
		case 'v': machine.curr_state = STATE_QUERY_DOWN; break;
		case '<': machine.curr_state = STATE_QUERY_LEFT; break;
	}
	
	/* Per puzzle rules, mark start position as 'X' */
	tape[machine.curr_pos] = 'X';

	
	fprintf(stderr, "Starting simulation (NCOLS=%zu) at pos %zu, state %s...\n",
		NCOLS,
		machine.curr_pos,
		#ifdef __DEBUG
			state_to_str(machine.curr_state)
		#else
			"..."
		#endif
	);

	long long step_count = 0;
	
	/*
	 * This is the TM "Engine".
	 */
	while (machine.curr_state != STATE_HALT && machine.curr_state != STATE_ERROR) {
		/* 1. Read */
		char tape_char = tape[machine.curr_pos];
		Symbol current_symbol = char_to_symbol(tape_char);
		
		/* 2. Lookup Rule */
		const Transition* rule = &g_transition_table[machine.curr_state][current_symbol];
		
		/* --- DEBUG LOGGING --- */
		#ifdef __DEBUG
		fprintf(stderr,
			"[Step %-5lld] pos=%-4zu state=%-30s read='%c' | write='%c' move=%-2d next=%s\n",
			step_count,
			machine.curr_pos,
			state_to_str(machine.curr_state),
			tape_char,
			symbol_to_str(rule->write_symbol),
			rule->move,
			state_to_str(rule->next_state)
		);
		#endif
		/* --- END LOGGING --- */

		/* 3. Write */
		tape[machine.curr_pos] = symbol_to_char(rule->write_symbol);
		
		/* 4. Move */
		machine.curr_pos += rule->move;
		
		/* 5. Transition */
		machine.curr_state = rule->next_state;
		
		step_count++;
	}
	
	if (machine.curr_state == STATE_ERROR) {
		fprintf(stderr, "Simulation Halted on ERROR after %lld steps.\n\n", step_count);
	} else {
		fprintf(stderr, "Simulation Halted after %lld steps.\n\n", step_count);
	}

	/* --- Part 2 Output: Print the final grid --- */
	printf("--- Final Grid State ---\n");
	for (size_t i = 0; i < NROWS; i++) {
		/* Print from the start of the grid, not the scratchpad */
		printf("%.*s\n", (int)NCOLS, tape + TAPE_GRID_OFFSET + i*NCOLS);
	}

	/* --- Part 3: Count the 'X's --- */
	size_t visited_count = 0;
	size_t grid_end = TAPE_GRID_OFFSET + TAPE_GRID_SIZE;
	for (size_t i = TAPE_GRID_OFFSET; i < grid_end; i++) {
		if (tape[i] == 'X') {
			visited_count++;
		}
	}
	
	printf("\n--- Part 3 Result ---\n");
	printf("Total distinct positions visited: %zu\n", visited_count);
	
	free(tape); /* Clean up dynamic memory */
	return EXIT_SUCCESS;
}

