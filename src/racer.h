/**
 * @file: racer.h
 * @brief: Declarations for racer functions and data structures.
 * @author: Stephen Brennan
 */
#ifndef SMB_RACER_H_
#define SMB_RACER_H_

#include <ncurses.h>

/**
 * Holds all necessary state for the type racing game.
 * @track: ncurses window holding track with competitors
 * @prompt: ncurses window which displays the prompt
 * @entry: ncurses window which displays what has been typed so far
 * @name: player's screen name
 * @text: the prompt the player is supposed to be typing
 * @input: the visible text in the entry window
 * @text_lines: line wrap buffer for the text
 * @text_position: index of word the player is typing
 * @input_position: position in input buffer
 */
struct racer {
	WINDOW *track;
	WINDOW *prompt;
	WINDOW *entry;
	char *name;
	char *text;
	char *input;
	uint16_t *text_lines;
	int text_position;
	int input_position;
};

/*
 * utility macros
 */
#define UNUSED(x) (void) x
#define ALT_BACKSPACE 127

/*
 * ncurses related functions (window.c)
 */
void racer_init_ncurses(void);
void racer_deinit_ncurses(void);
void racer_recreate_windows(struct racer *game);
void racer_wrap_buffer(char *buf, int height, int width, uint16_t lines[]);
void racer_print_wrapped(WINDOW *win, char *buf, uint16_t lines[]);
void racer_wrap_print(WINDOW *win, char *buf, uint16_t lines[]);
void racer_reprint_word(WINDOW *win, char *buf, uint16_t lines[], int idx,
                        int attrs);

#endif
