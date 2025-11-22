Simulation of a faithful 40-state Turing machine that traverses an m-by-n grid.
Solves part 1 of day 06 from [Advent of Code 2024](https://adventofcode.com/2024/day/6).
Created using Gemini 2.5 Pro (enhanced interrogation techniques required).
See `transcript.html` for the full chatlog.

The machine uses 11 symbols and 40 logical states (not including error and halt states).
The machine strictly follows a Read/Write/Move/Transition cycle, where movement is limited to one cell left or right.
(Compare to the simplified machine in `gallivant.c`, which allows arbitrary movement.)

The tape is simulated as a char array of two logical segments.
The first segment of the tape is reserved for internal use.
The second segment represents the state of the grid, where rows are padded by boundary symbols.
The machine uses a tally-counter mechanism in the first segment to move between (arbitrary-length) rows.
Duplicate symbols are used to preserve the grid while commuting between tape segments.

Typical invocation:
`./compile-run.sh < example.txt 2> output.txt`

Halts on `input.txt` with the correct answer after ~6.2 billion steps.
Highly recommended to turn off debug logging for large grids.

Machines 1 and 3 not implemented.

