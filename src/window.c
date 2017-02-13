/**
 * @file: window.c
 * @brief: Functions that relate to ncurses.
 * @author: Stephen Brennan
 */
#include <string.h>

#include "racer.h"

#define renewwin(win, lines, cols, y, x) \
	if (!win) { \
		win = newwin(lines, cols, y, x); \
	} else { \
		mvwin(win, y, x); \
		wresize(win, lines, cols); \
	}

/**
 * @brief: Initialize ncurses library and screen.
 */
void racer_init_ncurses(void)
{
	initscr();   // must be called first
	cbreak();    // no line buffering
	noecho();    // don't echo typed characters
	nonl();      // don't do CR/LF operations on return key
	//timeout(0);  // non-blocking getch()
	//curs_set(0); // set the cursor to be invisible
	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
}

/**
 * @brief: Perform tear-down of ncurses before exiting.
 */
void racer_deinit_ncurses(void)
{
	endwin();
}

/**
 * @brief: Put a box around the windows and wait for input.
 *
 * This is mostly for diagnostics, so that I can see what the windows look like.
 */
static void racer_box_windows(struct racer *game)
{
	box(game->track, 0, 0);
	box(game->prompt, 0, 0);
	box(game->entry, 0, 0);
	wnoutrefresh(game->track);
	wnoutrefresh(game->prompt);
	wnoutrefresh(game->entry);
	doupdate();
	wgetch(game->track);
}

/**
 * @brief: Create (or recreate) the ncurses windows of the game.
 *
 * This ought to be called once at the beginning of the game, in order to
 * initially create the windows. If the terminal is resized, this should be
 * called again to recreate the windows at the new size.
 *
 * @param game: The game instance.
 */
void racer_recreate_windows(struct racer *game)
{
	int maxy, maxx, q, y2, y3;
	getmaxyx(stdscr, maxy, maxx);
	q = maxy / 3;
	y2 = q;
	y3 = y2 + q;
	renewwin(game->track, y2, maxx, 0, 0);
	renewwin(game->prompt, y3 - y2, maxx, y2, 0);
	renewwin(game->entry, maxy - y3, maxx, y3, 0);
}

/**
 * @brief: Compute the word wrapping of a string onto a window.
 *
 * This does intelligent word-wrapping rather than breaking lines in the middle
 * of words. Its writes as output the index that each line starts at. It does
 * not actually do the printing of the buffer.
 *
 * @param buf: buffer to print
 * @param height: height of window this will be printed to
 * @param width: width of window this will be printed to
 * @param[out] lines: array to write the index that each line starts at
 */
void racer_wrap_buffer(char *buf, int height, int width, uint16_t lines[])
{
	int buflen = strlen(buf);
	int offset = 0;
	int lineno = 0;
	int last_in_line;

	while (buflen - offset >= width && lineno <= height) {
		/* rule: last character in the line must be space */
		last_in_line = offset + width - 1;
		while (buf[last_in_line] != ' ') {
			last_in_line--;
		}
		lines[lineno++] = offset;
		offset = last_in_line + 1;
	}
	lines[lineno] = offset;
	lines[lineno + 1] = buflen;
}

/**
 * @brief: Print a wrapped buffer onto a window.
 * @param win: window to print onto
 * @param buf: text to print
 * @param lines: line wrap buffer (from racer_wrap_buffer)
 */
void racer_print_wrapped(WINDOW *win, char *buf, uint16_t lines[])
{
	int buflen = strlen(buf);
	for (int lineno = 0; lines[lineno] != buflen; lineno++) {
		wmove(win, lineno, 0);
		waddnstr(win, buf + lines[lineno],
		         lines[lineno+1] - lines[lineno]);
	}
}

/**
 * @brief: Wrap and print a buffer. A helper function.
 * @param win: window to print onto
 * @param buf: text to print
 * @param lines: line wrap
 */
void racer_wrap_print(WINDOW *win, char *buf, uint16_t lines[])
{
	int height, width;
	getmaxyx(win, height, width);
	racer_wrap_buffer(buf, height, width, lines);
	racer_print_wrapped(win, buf, lines);
	wnoutrefresh(win);
}

/**
 * @brief: Reprint a word with an attribute.
 * @param win: window to print into
 * @param buf: text to print
 * @param lines: line wrap array
 * @param idx: index of word
 * @param attrs: attributes to print with
 */
void racer_reprint_word(WINDOW *win, char *buf, uint16_t lines[], int idx,
                        int attrs)
{
	int line = 0;
	int col = 0;
	int wordlen = 0;

	/* find the row and column */
	while (lines[line] <= idx) {
		line++;
	}
	line--;
	col = idx - lines[line];

	/* find the length of the word */
	while (buf[idx + wordlen] != ' ' && buf[idx + wordlen] != '\0') {
		wordlen++;
	}

	wmove(win, line, col);
	wattron(win, attrs);
	waddnstr(win, buf + idx, wordlen);
	wattroff(win, attrs);
	wnoutrefresh(win);
}
