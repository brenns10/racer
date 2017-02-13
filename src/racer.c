/**
 * @file: racer.c
 * @brief: Main entry point for racer game.
 * @author: Stephen Brennan
 */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "racer.h"

char *test = "the quick brown fox jumps over the lazy hare " \
	"the quick brown fox jumps over the lazy hare " \
	"the quick brown fox jumps over the lazy hare " \
	"the quick brown fox jumps over the lazy hare " \
	"the quick brown fox jumps over the lazy hare " \
	"the quick brown fox jumps over the lazy hare " \
	"the quick brown fox jumps over the lazy hare " \
	"the quick brown fox jumps over the lazy hare " \
	"the quick brown fox jumps over the lazy hare " \
	"the quick brown fox jumps over the lazy hare " \
	"the quick brown fox jumps over the lazy hare " \
	"the quick brown fox jumps over the lazy hare ";

void racer_init(struct racer *game)
{
	racer_init_ncurses();
	racer_recreate_windows(game);
	game->text = test;
	game->text_lines = malloc(256 * sizeof(uint16_t));
	game->text_position = 0;
	game->input_position = 0;
	game->input = malloc(256);
	racer_wrap_print(game->prompt, game->text, game->text_lines);
	racer_reprint_word(game->prompt, game->text, game->text_lines, 0,
	                   A_STANDOUT);
}

bool racer_is_allowed(int c) {
	if ('A' <= c && c <= 'Z')
		return true;
	else if ('a' <= c && c <= 'z')
		return true;
	switch (c) {
	case '\'':
	case '"':
	case '.':
	case ',':
	case '-':
	case '!':
	case '?':
	case ' ':
		return true;
	default:
		return false;
	}
}

void racer_addch(struct racer *game, char c)
{
	int pair;
	if (c == game->text[game->text_position + game->input_position]) {
		pair = 2;
	} else {
		pair = 1;
	}
	wattron(game->entry, COLOR_PAIR(pair));
	waddch(game->entry, c);
	wattroff(game->entry, COLOR_PAIR(pair));
	game->input[game->input_position++] = c;
	wnoutrefresh(game->entry);
}

void racer_backspace(struct racer *game)
{
	mvwdelch(game->entry, 0, game->input_position - 1);
	game->input_position--;
	wnoutrefresh(game->entry);
}

bool racer_input_matches(struct racer *game)
{
	return 0 == strncmp(game->text + game->text_position, game->input,
	                    game->input_position);
}

void racer_advance_word(struct racer *game)
{
	wclear(game->entry);
	wmove(game->entry, 0, 0);
	racer_reprint_word(game->prompt, game->text, game->text_lines,
	                   game->text_position, A_NORMAL);
	while (game->text[game->text_position] != ' ') {
		game->text_position++;
	}
	game->text_position++;
	racer_reprint_word(game->prompt, game->text, game->text_lines,
	                   game->text_position, A_STANDOUT);
	game->input_position = 0;
}

void racer_handle_input(struct racer *game, int input)
{
	if (racer_is_allowed(input)) {
		racer_addch(game, input);
	} else if (input == KEY_BACKSPACE || input == ALT_BACKSPACE) {
		racer_backspace(game);
	}
	wnoutrefresh(game->track);
	if (input == ' ' && racer_input_matches(game)) {
		racer_advance_word(game);
	}
}

void racer_loop(struct racer *game)
{
	int input;
	bool exit = false;
	while (!exit) {
		input = wgetch(game->entry);
		racer_handle_input(game, input);
		// reset cursor to correct place
		wmove(game->entry, 0, game->input_position);
		doupdate();
	}
}

int main(int argc, char *argv[])
{
	struct racer game = {
		.track = NULL,
		.prompt = NULL,
		.entry = NULL,
		.name = NULL,
	};
	UNUSED(argc);
	UNUSED(argv);

	racer_init(&game);
	wrefresh(game.prompt);
	racer_loop(&game);
	racer_deinit_ncurses();
}
