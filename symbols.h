#ifndef SYMBOLS_H_
#define SYMBOLS_H_

#include <stdio.h>
#include <stdlib.h>

/*
 * The complete set of states for the TM.
 * STATE_ERROR = 0, so all uninitialized transitions default to it.
 */
typedef enum State {
	STATE_ERROR = 0,
	STATE_HALT,

	/* 1. Main 8-State Logic */
	STATE_QUERY_UP,
	STATE_QUERY_RIGHT,
	STATE_QUERY_DOWN,
	STATE_QUERY_LEFT,
	STATE_RESPOND_UP,
	STATE_RESPOND_RIGHT,
	STATE_RESPOND_DOWN,
	STATE_RESPOND_LEFT,

	/* 2. Subroutine: GO_UP (pos - NCOLS) */
	STATE_GOTO_HOME_UP,
	STATE_GOTO_TALLY_END_UP,
	/* CHECK_TALLY_UP is now unused, but left for alignment */
	STATE_CHECK_TALLY_UP, 
	STATE_RETURN_TO_GRID_UP,
	STATE_MOVE_LEFT_AND_MARK,
	STATE_GOTO_HOME_FOR_RESET_UP,
	STATE_RESET_TALLY_UP,
	STATE_GOTO_FINAL_POS_UP,

	/* 3. Subroutine: GO_DOWN (pos + NCOLS) */
	STATE_GOTO_HOME_DOWN,
	STATE_GOTO_TALLY_END_DOWN,
	STATE_CHECK_TALLY_DOWN,
	STATE_RETURN_TO_GRID_DOWN,
	STATE_MOVE_RIGHT_AND_MARK,
	STATE_GOTO_HOME_FOR_RESET_DOWN,
	STATE_RESET_TALLY_DOWN,
	STATE_GOTO_FINAL_POS_DOWN,
	
	/* 4. Subroutine: GO_UP_RETURN (pos - NCOLS, then halt) */
	STATE_GOTO_HOME_UP_RETURN,
	STATE_GOTO_TALLY_END_UP_RETURN,
	STATE_CHECK_TALLY_UP_RETURN,
	STATE_RETURN_TO_GRID_UP_RETURN,
	STATE_MOVE_LEFT_AND_MARK_RETURN,
	STATE_GOTO_HOME_FOR_RESET_UP_RETURN,
	STATE_RESET_TALLY_UP_RETURN,
	STATE_GOTO_FINAL_POS_UP_RETURN,

	/* 5. Subroutine: GO_DOWN_RETURN (pos + NCOLS, then halt) */
	STATE_GOTO_HOME_DOWN_RETURN,
	STATE_GOTO_TALLY_END_DOWN_RETURN,
	STATE_CHECK_TALLY_DOWN_RETURN,
	STATE_RETURN_TO_GRID_DOWN_RETURN,
	STATE_MOVE_RIGHT_AND_MARK_RETURN,
	STATE_GOTO_HOME_FOR_RESET_DOWN_RETURN,
	STATE_RESET_TALLY_DOWN_RETURN,
	STATE_GOTO_FINAL_POS_DOWN_RETURN,
	
	NUM_STATES /* Utility for array sizing */
} State;


/*
 * The complete "alphabet" of symbols the TM can read/write.
 */
typedef enum {
	SYM_PERIOD = 0, /* . (Empty space) */
	SYM_HASH,       /* # (Obstacle) */
	SYM_X,          /* X (Visited) */
	SYM_R,          /* R (Boundary) */
	SYM_AT,         /* @ (Home beacon) */
	SYM_T,          /* T (Tally mark) */
	SYM_E,          /* E (Tally end) */
	SYM_SH_PERIOD,  /* % (Shadow for .) */
	SYM_SH_HASH,    /* $ (Shadow for #) */
	SYM_SH_X,       /* & (Shadow for X) */
	SYM_SH_R,       /* r (Shadow for R) */
	NUM_SYMBOLS     /* Utility for array sizing */
} Symbol;

/*
 * A single TM rule.
 * (currentState, readSymbol) -> (writeSymbol, move, nextState)
 */
typedef struct {
	Symbol write_symbol; /* The symbol to write */
	int    move;         /* -1 (L), 0 (Stay), +1 (R) */
	State  next_state;   /* The state to transition to */
} Transition;

/*
 * Helper to map a tape 'char' to its corresponding 'Symbol' enum.
 */
static Symbol char_to_symbol(char c) {
	switch (c) {
		case '.': return SYM_PERIOD;
		case '#': return SYM_HASH;
		case 'X': return SYM_X;
		case 'R': return SYM_R;
		case '@': return SYM_AT;
		case 'T': return SYM_T;
		case 'E': return SYM_E;
		case '%': return SYM_SH_PERIOD;
		case '$': return SYM_SH_HASH;
		case '&': return SYM_SH_X;
		case 'r': return SYM_SH_R;
		default:
			fprintf(stderr, "Warning: Unknown tape symbol '%c'. Triggering ERROR.\n", c);
			/* * We can't return an "error" symbol, so we return
			 * a symbol that all states should treat as an error (e.g., '@').
			 * The transition table's default ERROR state will catch this.
			 */
			return SYM_AT;
	}
}

/*
 * Helper to map a 'Symbol' enum back to its 'char' representation.
 */
static char symbol_to_char(Symbol s) {
	switch (s) {
		case SYM_PERIOD:    return '.';
		case SYM_HASH:      return '#';
		case SYM_X:         return 'X';
		case SYM_R:         return 'R';
		case SYM_AT:        return '@';
		case SYM_T:         return 'T';
		case SYM_E:         return 'E';
		case SYM_SH_PERIOD: return '%';
		case SYM_SH_HASH:   return '$';
		case SYM_SH_X:      return '&';
		case SYM_SH_R:      return 'r';
		/* NUM_SYMBOLS is not a real symbol */
		case NUM_SYMBOLS:
		default:
			fprintf(stderr, "Error: Unknown symbol enum %d\n", s);
			exit(EXIT_FAILURE);
	}
}

#endif /* SYMBOLS_H_ */

