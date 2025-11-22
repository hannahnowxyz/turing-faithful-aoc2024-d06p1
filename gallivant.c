#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* For strchr */

#define PROWS 10
#define PCOLS 10
#define RROWS (2*1)
#define RCOLS 1
#define NROWS (PROWS + RROWS)
#define NCOLS (PCOLS + RCOLS)

/*
 * State transition macros:
 * TO_RESPONSE: (Query state) -> (Corresponding Response state)
 * TO_NEXT_QUERY: (Response state) -> (Next Query state after turning)
 */
#define TO_RESPONSE(s) (s + 4)
#define TO_NEXT_QUERY(s) ((s - 4 + 1) % 4)


static char tape[NROWS*NCOLS + 1] =
	"RRRRRRRRRRR"
	"R....#....."
	"R.........#"
	"R.........."
	"R..#......."
	"R.......#.."
	"R.........."
	"R.#..X....." /* Use 'X' for start, per suggestion */
	"R........#."
	"R#........."
	"R......#..."
	"RRRRRRRRRRR"
	"\0"
;

typedef enum {
	/* Query states: Head is ON the guard's position */
	STATE_QUERY_UP = 0,
	STATE_QUERY_RIGHT = 1,
	STATE_QUERY_DOWN = 2,
	STATE_QUERY_LEFT = 3,

	/* Response states: Head is IN FRONT OF the guard's position */
	STATE_RESPOND_UP = 4,
	STATE_RESPOND_RIGHT = 5,
	STATE_RESPOND_DOWN = 6,
	STATE_RESPOND_LEFT = 7,

	STATE_HALT
} State;

typedef struct {
	State curr_state;
	size_t curr_pos;
} Machine;


int main(void) {
	/* Find the starting 'X' on the tape */
	char* start_char = strchr(tape, 'X');
	if (start_char == NULL) {
		fprintf(stderr, "Error: No start 'X' found on tape.\n");
		return EXIT_FAILURE;
	}

	Machine machine = {
		/* Start in a QUERY state, at the 'X' */
		.curr_state = STATE_QUERY_UP,
		.curr_pos = start_char - tape
	};

	while (machine.curr_state != STATE_HALT) {
		/*
		 * Faithful TM Step 1: Read the symbol at the current position.
		 */
		char current_sym = tape[machine.curr_pos];

		/*
		 * Faithful TM Step 2: Decide (Write, Move, Transition)
		 * based *only* on (curr_state, current_sym).
		 */
		switch (machine.curr_state) {

			/* --- QUERY STATES --- */
			/* Job: Mark current square, move 1 step, transition to RESPOND */
			case STATE_QUERY_UP:
				tape[machine.curr_pos] = 'X';    /* Write */
				machine.curr_pos -= NCOLS;       /* Move Up */
				machine.curr_state = TO_RESPONSE(machine.curr_state); /* Transition */
				break;
			case STATE_QUERY_RIGHT:
				tape[machine.curr_pos] = 'X';    /* Write */
				machine.curr_pos += 1;           /* Move Right */
				machine.curr_state = TO_RESPONSE(machine.curr_state); /* Transition */
				break;
			case STATE_QUERY_DOWN:
				tape[machine.curr_pos] = 'X';    /* Write */
				machine.curr_pos += NCOLS;       /* Move Down */
				machine.curr_state = TO_RESPONSE(machine.curr_state); /* Transition */
				break;
			case STATE_QUERY_LEFT:
				tape[machine.curr_pos] = 'X';    /* Write */
				machine.curr_pos -= 1;           /* Move Left */
				machine.curr_state = TO_RESPONSE(machine.curr_state); /* Transition */
				break;

			/* --- RESPONSE STATES --- */
			/* Job: Read symbol, then either (step) or (turn) */
			case STATE_RESPOND_UP:
				switch (current_sym) {
					case '.':
					case 'X': /* Path clear: Step forward */
						tape[machine.curr_pos] = 'X';        /* Write */
						/* No move (head is now on new pos) */
						machine.curr_state = STATE_QUERY_UP; /* Transition */
						break;
					case '#': /* Obstacle: Turn right */
						/* No write */
						machine.curr_pos += NCOLS;           /* Move Down (back) */
						machine.curr_state = TO_NEXT_QUERY(machine.curr_state); /* Transition */
						break;
					case 'R': /* Boundary: Halt */
						machine.curr_state = STATE_HALT;     /* Transition */
						break;
				}
				break;
			case STATE_RESPOND_RIGHT:
				switch (current_sym) {
					case '.':
					case 'X': /* Path clear: Step forward */
						tape[machine.curr_pos] = 'X';
						machine.curr_state = STATE_QUERY_RIGHT;
						break;
					case '#': /* Obstacle: Turn right */
						machine.curr_pos -= 1; /* Move Left (back) */
						machine.curr_state = TO_NEXT_QUERY(machine.curr_state);
						break;
					case 'R': /* Boundary: Halt */
						machine.curr_state = STATE_HALT;
						break;
				}
				break;
			case STATE_RESPOND_DOWN:
				switch (current_sym) {
					case '.':
					case 'X': /* Path clear: Step forward */
						tape[machine.curr_pos] = 'X';
						machine.curr_state = STATE_QUERY_DOWN;
						break;
					case '#': /* Obstacle: Turn right */
						machine.curr_pos -= NCOLS; /* Move Up (back) */
						machine.curr_state = TO_NEXT_QUERY(machine.curr_state);
						break;
					case 'R': /* Boundary: Halt */
						machine.curr_state = STATE_HALT;
						break;
				}
				break;
			case STATE_RESPOND_LEFT:
				switch (current_sym) {
					case '.':
					case 'X': /* Path clear: Step forward */
						tape[machine.curr_pos] = 'X';
						machine.curr_state = STATE_QUERY_LEFT;
						break;
					case '#': /* Obstacle: Turn right */
						machine.curr_pos += 1; /* Move Right (back) */
						machine.curr_state = TO_NEXT_QUERY(machine.curr_state);
						break;
					case 'R': /* Boundary: Halt */
						machine.curr_state = STATE_HALT;
						break;
				}
				break;
			
			/* Unreachable, but good practice */
			case STATE_HALT:
				break;
		}
	}

	/* Print the final state of the tape */
	for (size_t i = 0; i < NROWS; i++) {
		printf("%.*s\n", NCOLS, tape + i*NCOLS);
	}

	return EXIT_SUCCESS;
}

