/* Command table manipulation functions and vectors.

   Copyright (C) 1993-1998 Sebastiano Vigna
   Copyright (C) 1999-2016 Todd M. Lewis and Sebastiano Vigna

   This file is part of ne, the nice editor.

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or (at your
   option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
   for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.  */


#include "ne.h"
#include "help.h"
#include "hash.h"

#undef TABSIZE

/* The standard macro descriptor allocation dimension. */

#define STD_MACRO_DESC_SIZE   1024


/* This structure represents a command. It includes a long and a short name,
a NULL-terminated vector of help strings (of specified length) and some flags
which are related to the syntax and the semantics of the arguments. */

typedef struct {
	const char *name, *short_name;
	const char * const *help;
	int help_len;
	int flags;
} command;


#define NO_ARGS               (1<<1)         /* This command must be called without argument. */
#define ARG_IS_STRING         (1<<2)         /* The argument is a string (default is a number). */
#define IS_OPTION             (1<<3)         /* The command controls an option,
                                                and can be played while exec_only_options is true. */
#define DO_NOT_RECORD         (1<<4)         /* Never record this command. */
#define EMPTY_STRING_OK       (1<<5)         /* This command can accept an empty string ("") as an argument. */


/* These macros makes the following vector more readable. */

#define HELP_LEN(x)  (sizeof(x ## _HELP) / sizeof(char *) - 1)
#define NAHL(x) x ## _NAME, x ##_ABBREV, x ## _HELP, HELP_LEN(x)

/* This is the command vector. Note that the command names come from names.h,
   and the help names come from help.h. This must be kept sorted. */

static const command commands[ACTION_COUNT] = {
	{ NAHL(ABOUT         ), NO_ARGS                                                               },
	{ NAHL(ADJUSTVIEW    ),           ARG_IS_STRING                                               },
	{ NAHL(ALERT         ), NO_ARGS                                                               },
	{ NAHL(ATOMICUNDO    ),           ARG_IS_STRING |                             EMPTY_STRING_OK },
	{ NAHL(AUTOCOMPLETE  ),           ARG_IS_STRING |                             EMPTY_STRING_OK },
	{ NAHL(AUTOINDENT    ),                           IS_OPTION                                   },
	{ NAHL(AUTOMATCHBRACKET),                         IS_OPTION                                   },
	{ NAHL(AUTOPREFS     ),                           IS_OPTION                                   },
	{ NAHL(BACKSPACE     ),0                                                                      },
	{ NAHL(BEEP          ), NO_ARGS                                                               },
	{ NAHL(BINARY        ),                           IS_OPTION                                   },
	{ NAHL(CAPITALIZE    ),0                                                                      },
	{ NAHL(CASESEARCH    ),                           IS_OPTION                                   },
	{ NAHL(CENTER        ),0                                                                      },
	{ NAHL(CLEAR         ), NO_ARGS                                                               },
	{ NAHL(CLIPNUMBER    ),                           IS_OPTION                                   },
	{ NAHL(CLOSEDOC      ), NO_ARGS                                                               },
	{ NAHL(COPY          ),0                                                                      },
	{ NAHL(CRLF          ),                           IS_OPTION                                   },
	{ NAHL(CUT           ),0                                                                      },
	{ NAHL(DELETECHAR    ),0                                                                      },
	{ NAHL(DELETEEOL     ), NO_ARGS                                                               },
	{ NAHL(DELETELINE    ),0                                                                      },
	{ NAHL(DELETENEXTWORD),0                                                                      },
	{ NAHL(DELETEPREVWORD),0                                                                      },
	{ NAHL(DELTABS       ),                           IS_OPTION                                   },
	{ NAHL(DOUNDO        ),                           IS_OPTION                                   },
	{ NAHL(ERASE         ),0                                                                      },
	{ NAHL(ESCAPE        ),                                       DO_NOT_RECORD                   },
	{ NAHL(ESCAPETIME    ),                           IS_OPTION                                   },
	{ NAHL(EXEC          ),           ARG_IS_STRING |             DO_NOT_RECORD                   },
	{ NAHL(EXIT          ), NO_ARGS                                                               },
	{ NAHL(FASTGUI       ),                           IS_OPTION                                   },
	{ NAHL(FIND          ),           ARG_IS_STRING                                               },
	{ NAHL(FINDREGEXP    ),           ARG_IS_STRING                                               },
	{ NAHL(FLAGS         ), NO_ARGS |                             DO_NOT_RECORD                   },
	{ NAHL(FLASH         ), NO_ARGS                                                               },
	{ NAHL(FREEFORM      ),                           IS_OPTION                                   },
	{ NAHL(GOTOBOOKMARK  ),           ARG_IS_STRING |                             EMPTY_STRING_OK },
	{ NAHL(GOTOCOLUMN    ),0                                                                      },
	{ NAHL(GOTOLINE      ),0                                                                      },
	{ NAHL(GOTOMARK      ), NO_ARGS                                                               },
	{ NAHL(HELP          ),           ARG_IS_STRING |             DO_NOT_RECORD                   },
	{ NAHL(HEXCODE       ),                           IS_OPTION                                   },
	{ NAHL(INSERT        ),                           IS_OPTION                                   },
	{ NAHL(INSERTCHAR    ),0                                                                      },
	{ NAHL(INSERTLINE    ),0                                                                      },
	{ NAHL(INSERTSTRING  ),           ARG_IS_STRING                                               },
	{ NAHL(INSERTTAB     ),0                                                                      },
	{ NAHL(KEYCODE       ),                                       DO_NOT_RECORD                   },
	{ NAHL(LINEDOWN      ),0                                                                      },
	{ NAHL(LINEUP        ),0                                                                      },
	{ NAHL(LOADAUTOPREFS ), NO_ARGS                                                               },
	{ NAHL(LOADPREFS     ),           ARG_IS_STRING                                               },
	{ NAHL(MACRO         ),           ARG_IS_STRING |             DO_NOT_RECORD                   },
	{ NAHL(MARK          ),                           IS_OPTION                                   },
	{ NAHL(MARKVERT      ),                           IS_OPTION                                   },
	{ NAHL(MATCHBRACKET  ), NO_ARGS                                                               },
	{ NAHL(MODIFIED      ),                           IS_OPTION                                   },
	{ NAHL(MOVEBOS       ), NO_ARGS                                                               },
	{ NAHL(MOVEEOF       ), NO_ARGS                                                               },
	{ NAHL(MOVEEOL       ), NO_ARGS                                                               },
	{ NAHL(MOVEEOW       ),0                                                                      },
	{ NAHL(MOVEINCDOWN   ), NO_ARGS                                                               },
	{ NAHL(MOVEINCUP     ), NO_ARGS                                                               },
	{ NAHL(MOVELEFT      ),0                                                                      },
	{ NAHL(MOVERIGHT     ),0                                                                      },
	{ NAHL(MOVESOF       ), NO_ARGS                                                               },
	{ NAHL(MOVESOL       ), NO_ARGS                                                               },
	{ NAHL(MOVETOS       ), NO_ARGS                                                               },
	{ NAHL(NEWDOC        ), NO_ARGS                                                               },
	{ NAHL(NEXTDOC       ),0                                                                      },
	{ NAHL(NEXTPAGE      ),0                                                                      },
	{ NAHL(NEXTWORD      ),0                                                                      },
	{ NAHL(NOFILEREQ     ),                           IS_OPTION                                   },
	{ NAHL(NOP           ), NO_ARGS                                                               },
	{ NAHL(OPEN          ),           ARG_IS_STRING                                               },
	{ NAHL(OPENCLIP      ),           ARG_IS_STRING                                               },
	{ NAHL(OPENMACRO     ),           ARG_IS_STRING                                               },
	{ NAHL(OPENNEW       ),           ARG_IS_STRING                                               },
	{ NAHL(PAGEDOWN      ),0                                                                      },
	{ NAHL(PAGEUP        ),0                                                                      },
	{ NAHL(PARAGRAPH     ),0                                                                      },
	{ NAHL(PASTE         ),0                                                                      },
	{ NAHL(PASTEVERT     ),0                                                                      },
	{ NAHL(PLAY          ),0                                                                      },
	{ NAHL(POPPREFS      ),0                                                                      },
	{ NAHL(PRESERVECR    ),                           IS_OPTION                                   },
	{ NAHL(PREVDOC       ),0                                                                      },
	{ NAHL(PREVPAGE      ),0                                                                      },
	{ NAHL(PREVWORD      ),0                                                                      },
	{ NAHL(PUSHPREFS     ),                           IS_OPTION                                   },
	{ NAHL(QUIT          ), NO_ARGS                                                               },
	{ NAHL(READONLY      ),                           IS_OPTION                                   },
	{ NAHL(RECORD        ),                           IS_OPTION | DO_NOT_RECORD                   },
	{ NAHL(REDO          ),0                                                                      },
	{ NAHL(REFRESH       ), NO_ARGS                                                               },
	{ NAHL(REPEATLAST    ),0                                                                      },
	{ NAHL(REPLACE       ),           ARG_IS_STRING |                             EMPTY_STRING_OK },
	{ NAHL(REPLACEALL    ),           ARG_IS_STRING |                             EMPTY_STRING_OK },
	{ NAHL(REPLACEONCE   ),           ARG_IS_STRING |                             EMPTY_STRING_OK },
	{ NAHL(REQUESTORDER  ),                           IS_OPTION                                   },
	{ NAHL(RIGHTMARGIN   ),                           IS_OPTION                                   },
	{ NAHL(SAVE          ), NO_ARGS                                                               },
	{ NAHL(SAVEALL       ), NO_ARGS                                                               },
	{ NAHL(SAVEAS        ),           ARG_IS_STRING                                               },
	{ NAHL(SAVEAUTOPREFS ), NO_ARGS                                                               },
	{ NAHL(SAVECLIP      ),           ARG_IS_STRING                                               },
	{ NAHL(SAVEDEFPREFS  ), NO_ARGS                                                               },
	{ NAHL(SAVEMACRO     ),           ARG_IS_STRING                                               },
	{ NAHL(SAVEPREFS     ),           ARG_IS_STRING                                               },
	{ NAHL(SEARCHBACK    ),                           IS_OPTION                                   },
	{ NAHL(SELECTDOC     ),0                                                                      },
	{ NAHL(SETBOOKMARK   ),           ARG_IS_STRING |                             EMPTY_STRING_OK },
	{ NAHL(SHIFT         ),           ARG_IS_STRING |                             EMPTY_STRING_OK },
	{ NAHL(SHIFTTABS     ),                           IS_OPTION                                   },
	{ NAHL(STATUSBAR     ),                           IS_OPTION                                   },
	{ NAHL(SUSPEND       ), NO_ARGS                                                               },
	{ NAHL(SYNTAX        ),           ARG_IS_STRING | IS_OPTION                                   },
	{ NAHL(SYSTEM        ),           ARG_IS_STRING                                               },
	{ NAHL(TABS          ),                           IS_OPTION                                   },
	{ NAHL(TABSIZE       ),                           IS_OPTION                                   },
	{ NAHL(THROUGH       ),           ARG_IS_STRING                                               },
	{ NAHL(TOGGLESEOF    ), NO_ARGS                                                               },
	{ NAHL(TOGGLESEOL    ), NO_ARGS                                                               },
	{ NAHL(TOLOWER       ),0                                                                      },
	{ NAHL(TOUPPER       ),0                                                                      },
	{ NAHL(TURBO         ),                           IS_OPTION                                   },
	{ NAHL(UNDELLINE     ),0                                                                      },
	{ NAHL(UNDO          ),0                                                                      },
	{ NAHL(UNLOADMACROS  ), NO_ARGS                                                               },
	{ NAHL(UNSETBOOKMARK ),           ARG_IS_STRING |                             EMPTY_STRING_OK },
	{ NAHL(UTF8          ),                           IS_OPTION                                   },
	{ NAHL(UTF8AUTO      ),                           IS_OPTION                                   },
	{ NAHL(UTF8IO        ),                           IS_OPTION                                   },
	{ NAHL(VERBOSEMACROS ),                           IS_OPTION                                   },
	{ NAHL(VISUALBELL    ),                           IS_OPTION                                   },
	{ NAHL(WORDWRAP      ),                           IS_OPTION                                   },
};


/* Checks whether the command line m starts with the command c. Return 0 on
   success, non-zero on failure. */

int cmdcmp(const char *c, const char *m) {
	assert(c != NULL);
	assert(m != NULL);

	while (*c && ascii_up_case[*(unsigned char *)c] == ascii_up_case[*(unsigned char *)m]) {
		c++;
		m++;
	}

	return *c || *m && !isasciispace(*m) ;
}





/* This table *can* have conflicts, so that we can keep its size much
   smaller. */

static macro_desc *macro_hash_table[MACRO_HASH_TABLE_SIZE];


/* This is the command name hashing function. We consider only the 5 least
   significant bits because they are the bits which distinguish characters,
   independently of their case. We are not interested in strings which contain
   non-alphabetical characters, because they will certainly generate an error
   (the only exception notably being "R1"). We should subtract 1 to s[i], but
   this doesn't seem to produce any improvement. hash_macro() act as hash(),
   but uses MACRO_HASH_TABLE_SIZE for its modulo. */


static int hash_cmd(const char * const s, int len) {
	int h = -1;
	while(len-- != 0) h = (h * 31 + ascii_up_case[(unsigned char)s[len]]) % HASH_TABLE_SIZE;
	return (h + HASH_TABLE_SIZE) % HASH_TABLE_SIZE;
}


static int hash_macro(const char * const s, int len) {
	int h = -1;
	while(len-- != 0) h = (h * 31 + ascii_up_case[(unsigned char)s[len]]) % MACRO_HASH_TABLE_SIZE;
	return (h + MACRO_HASH_TABLE_SIZE) % MACRO_HASH_TABLE_SIZE;
}



/* Parses a command line. This function has an interface which is slightly
   varied with respect to the other functions of ne. In case of a parsing
   error, an error index *with sign inverted* is passed back. In case parsing
   succeeds, an (greater or equal to zero) action is returned, and the
   numerical or string argument is passed in the variables pointed to by
   num_arg or string_arg, respectively, if they are non-NULL. Otherwise, the
   argument is not passed back. The string argument is free()able. -1 and NULL
   denote the lack of an optional numerical or string argument, respectively.
   NOP is returned on a NOP command, or on a comment line (any line whose first
   non-space character is a non alphabetic character). Note that the various
   syntax flags are used here. */

int parse_command_line(const char * command_line, int64_t * const num_arg, char ** const string_arg, const bool exec_only_options) {
	if (num_arg) *num_arg = -1;
	if (string_arg) *string_arg = NULL;
	if (!command_line || !*command_line) return NOP_A;

	while(isasciispace(*command_line)) command_line++;
	const char *p = command_line;

	if (!isalpha((unsigned char)*p)) { /* Comment, treated as NOP. */
		const int len = strlen(p);
		if (!(*string_arg = malloc(len + 1))) return -OUT_OF_MEMORY;
		memcpy(*string_arg, p, len);
		(*string_arg)[len] = 0;
		return NOP_A;
	}

	while(*p && !isasciispace(*p)) p++;

	const int h = hash_cmd(command_line, p - command_line);
	action a;
	if ((a = hash_table[h]) && !cmdcmp(commands[--a].name, command_line)
		|| (a = short_hash_table[h]) && !cmdcmp(commands[--a].short_name, command_line)) {

		while(isasciispace(*p)) p++;

		if (!(*p && (commands[a].flags & NO_ARGS))) {
			if (!*p || (commands[a].flags & ARG_IS_STRING) || isxdigit((unsigned char)*p) || *p == 'x' || *p =='X') {
				if ((commands[a].flags & IS_OPTION) || !exec_only_options) {
					if (*p) {
						if ((commands[a].flags & ARG_IS_STRING) && string_arg) {
							int len = strlen(p);

							if (len > 1 && *p == '"' && p[len - 1] == '"') {
								p++;
								len -= 2;
							}

							if (len == 0 && !(commands[a].flags & EMPTY_STRING_OK)) return -STRING_IS_EMPTY;

							if (!(*string_arg = malloc(len + 1))) return -OUT_OF_MEMORY;
							memcpy(*string_arg, p, len);
							(*string_arg)[len] = 0;
						}
						else if (num_arg) {
							char *q;
							*num_arg = strtoll(p, &q, 0);
							if (*q && !isasciispace(*q)) return -NOT_A_NUMBER;
						}
					}
					return a;
				}
				D(fprintf(stderr,"parse_command error: Can execute only options.\n");)
				return -CAN_EXECUTE_ONLY_OPTIONS;
			}
			D(fprintf(stderr,"parse_command error: Has numeric argument.\n");)
			return -HAS_NUMERIC_ARGUMENT;
		}
		D(fprintf(stderr,"parse_command error: Has no argument.\n");)
		return -HAS_NO_ARGUMENT;
	}
	D(fprintf(stderr,"parse_command error: No such command.\n");)
	return -NO_SUCH_COMMAND;
}


/* Parses and executes a command line. Standard error codes are returned. If
   the search for a standard command fails, we try to execute a macro in ~/.ne
   with the same name. */

int execute_command_line(buffer *b, const char *command_line) {
	encoding_type encoding = detect_encoding(command_line, strlen(command_line));
	if (b->encoding != ENC_ASCII && encoding != ENC_ASCII && b->encoding != encoding) return INCOMPATIBLE_COMMAND_ENCODING;

	int64_t n;
	int a;
	char *p;
	if ((a = parse_command_line(command_line, &n, &p, b->exec_only_options)) >= 0) return do_action(b, a, n, p);
	a = -a;

	if ((a == NO_SUCH_COMMAND) && (a = execute_macro(b, command_line)) == CANT_OPEN_MACRO) a = NO_SUCH_COMMAND;

	return a;
}



/* Allocates a macro descriptor. It does not allocate the internal character
   stream, which has to be allocated and stuffed in separately. */

macro_desc *alloc_macro_desc(void) {
	return calloc(1, sizeof(macro_desc));
}



/* Frees a macro descriptors. */

void free_macro_desc(macro_desc *md) {

	if (!md) return;

	assert_macro_desc(md);

	free(md->name);
	free_char_stream(md->cs);
	free(md);
}



/* Here we record an action in a character stream. The action name is expanded
   in a short or long name, depending on the value of the verbose parameter.  A
   numerical or string argument are expanded and copied, too. If the command
   should not be recorded (for instance, ESCAPE_A) we return. */

void record_action(char_stream *cs, action a, int64_t c, const char *p, bool verbose) {
	if (commands[a].flags & DO_NOT_RECORD) return;

	char t[MAX_INT_LEN + 2];

	/* NOP_A is special; it may actually be a comment.
	   Blank lines and real NOPs are recorded as blank lines. */
	if (a == NOP_A) {
		if (p && *p) add_to_stream(cs, p, strlen(p) + 1);
		else add_to_stream(cs, "", 1);
		return;
	}

	if (verbose) add_to_stream(cs, commands[a].name, strlen(commands[a].name));
	else add_to_stream(cs, commands[a].short_name, strlen(commands[a].short_name));

	if (c >= 0) {
		sprintf(t, " %" PRId64, c);
		add_to_stream(cs, t, strlen(t));
	}
	else if (p) {
		add_to_stream(cs, " ", 1);
		if (!*p || isasciispace(*p)) add_to_stream(cs, "\"", 1);
		add_to_stream(cs, p, strlen(p));
		if (!*p || isasciispace(*p)) add_to_stream(cs, "\"", 1);
	}

	add_to_stream(cs, "", 1);

}


/* A support function for optimize_macro(). It examines a string to see if it
   is a valid "InsertChar ##" command. If it is, then insertchar_val() returns
   the character code, otherwise it returns 0. */

static int insertchar_val(const char *p) {
	if ( !p || !*p) return 0;
	while(isasciispace(*p)) p++;
	const char * const cmd = p;

	if (!isalpha((unsigned char)*p)) return 0;

	while(*p && !isasciispace(*p)) p++;

	int h = hash_cmd(cmd, p - cmd);

	action a;
	if (((a = hash_table[h]) && !cmdcmp(commands[--a].name, cmd)
		|| (a = short_hash_table[h]) && !cmdcmp(commands[--a].short_name, cmd)) && a == INSERTCHAR_A) {

		while(isasciispace(*p)) p++;
		h = strtol(p, (char **)&cmd, 0);
		return *cmd || h < 0 ? 0 : h;
	}
	return 0;
}

/* Optimizing macros is not safe if there are any subsequent undo commands
   calls to macros (which may themselves contain undo commands). This function
   looks through a stream for undo or non-built in commands, and returns false
   if any are found; returns true otherwise. */

bool vet_optimize_macro_stream(char_stream * const cs, int64_t pos) {
	int64_t n;
	int a;
	char *p;

	while (pos < cs->len ) {
		if ((a = parse_command_line(&cs->stream[pos], &n, &p, 0)) >= 0) {
			if (p) free(p);
			if (a == UNDO_A) return false; /* optimization is not safe */
		} else {
			a = -a;
			if (a == NO_SUCH_COMMAND) return false; /* possibly a macro invocation */
		}
		pos += strlen(&cs->stream[pos]) + 1;
	}
	return true;
}

/* Looks through the macro stream for consecutive runs of InsertChar commands
   and replaces them with appropriate InsertString commands. This makes macros
   much easier to read if and when they have to be edited. Note that if the
   character inserted by InsertChar is not an ASCII character, then we should
   leave it as an InsertChar command to maximize portability of the macros. */

void optimize_macro(char_stream *cs, bool verbose) {
	if (!cs || !cs->len) return;

	int building = 0;
	bool safe_to_optimize = false;

	for (int64_t pos = 0; pos < cs->len; pos += strlen(&cs->stream[pos]) + 1) {
		char * const cmd = &cs->stream[pos];
		const int chr = insertchar_val(cmd);
		if (chr < 0x80 && isprint(chr) && (safe_to_optimize = vet_optimize_macro_stream(cs,pos))) {
			delete_from_stream(cs, pos, strlen(cmd) + 1);
			const char two[2] = { chr };
			if (building) {
				building++;
				insert_in_stream(cs, two, building, 1);
			}
			else {
				const char * const insert = verbose ? INSERTSTRING_NAME : INSERTSTRING_ABBREV;
				const int64_t len  = strlen(insert);
				insert_in_stream(cs, "\"",   pos, 2);   /* Closing quote */
				insert_in_stream(cs, two,    pos, 1);   /* The character itself */
				insert_in_stream(cs, " \"",  pos, 2);   /* space and openning quote */
				insert_in_stream(cs, insert, pos, len); /* The command itself */
				building = pos + len + 2;               /* This is where the char is now */
			}
		}
		else building = 0;
	}
}


/* This function is the ultimate goal of this file. It plays a character
   stream, considering each line as a command line. It polls the global stop
   variable in order to check for the user interrupting. Note that the macro is
   duplicated before execution: this is absolutely necessary, for otherwise a
   call to CloseDoc, Record or UnloadMacros could free() the block of memory
   which we are executing. */

int play_macro(buffer *b, char_stream *cs) {
	if (!cs) return ERROR;

	/* If len is 0 or 1, the character stream does not contain anything. */
	const int64_t len = cs->len;
	if (len < 2) return OK;

	char * const stream = malloc(len);
	if (!stream) return OUT_OF_MEMORY;
	char * p = stream;

	memcpy(stream, cs->stream, len);

	stop = false;

	b->executing_macro = 1;
	int error = OK;
	while(!stop && p - stream < len) {
#ifdef NE_TEST
		fprintf(stderr, "%s\n", p); /* During tests, we output to stderr the current command. */
#endif

		if (error = execute_command_line(b, p))
#ifndef NE_TEST
			break /* During tests, we never interrupt a macro. */
#endif
				;

#ifdef NE_TEST
		refresh_window(cur_buffer);
		draw_status_bar();
#endif
		p += strlen(p) + 1;
	}

	free(stream);

	return stop ? STOPPED : error;
}


/* Loads a macro, and puts it in the global macro hash table.  file_part is
   applied to the name argument before storing it and hashing it.  Note that if
   the macro can't be opened, we retry prefixing its name with the preferences
   directory name (~/.ne/). Thus, for instance, all autopreferences file whose
   name does not conflict with internal commands can be executed transparently
   just by typing their name. */

macro_desc *load_macro(const char *name) {

	assert(name != NULL);

	macro_desc * const md = alloc_macro_desc();
	if (!md) return NULL;

	char_stream * cs = load_stream(md->cs, name, false, false);

	char *macro_dir, *prefs_dir;
	if (!cs && (prefs_dir = exists_prefs_dir()) && (macro_dir = malloc(strlen(prefs_dir) + 2 + strlen(name)))) {
		strcat(strcpy(macro_dir, prefs_dir), name);
		cs = load_stream(md->cs, macro_dir, false, false);
		free(macro_dir);
	}

	if (!cs && (prefs_dir = exists_gprefs_dir()) && (macro_dir = malloc(strlen(prefs_dir) + 2 + strlen(name) + 7))) {
		strcat(strcat(strcpy(macro_dir, prefs_dir), "macros/"), name);
		cs = load_stream(md->cs, macro_dir, false, false);
		free(macro_dir);
	}

	if (cs) {
		/* the last line may not be null-terminated, so... */
		add_to_stream(cs, "", 1);

		md->cs = cs;
		md->name = str_dup(file_part(name));

		const int h = hash_macro(md->name, strlen(md->name));

		macro_desc **m = &macro_hash_table[h];

		while(*m) m = &((*m)->next);

		*m = md;

		return md;
	}

	free_macro_desc(md);
	return NULL;
}



/* Executes a named macro. If the macro is not in the global
macro list, it is loaded. A standard error code is returned. */

int execute_macro(buffer *b, const char *name) {
	static int call_depth = 0;

	if (++call_depth > 32) {
		--call_depth;
		return MAX_MACRO_DEPTH_EXCEEDED;
	}

	const char * const p = file_part(name);
	int h = hash_macro(p, strlen(p));

	macro_desc *md;
	for(md = macro_hash_table[h]; md && cmdcmp(md->name, p); md = md->next );
	if (!md) md = load_macro(name);

	assert_macro_desc(md);

	if (md) {
		if (b->recording) {
			add_to_stream(b->cur_macro, "# include macro ", 16);
			add_to_stream(b->cur_macro, md->name, strlen(md->name)+1);
		}
		h = play_macro(b, md->cs);
		if (b->recording) {
			add_to_stream(b->cur_macro, "# conclude macro ", 17);
			add_to_stream(b->cur_macro, md->name, strlen(md->name)+1);
		}
		--call_depth;
		return h;
	}

	--call_depth;
	return CANT_OPEN_MACRO;
}


/* Clears up the macro table. */

void unload_macros(void) {
	for(int i = 0; i < MACRO_HASH_TABLE_SIZE; i++) {
		macro_desc *m = macro_hash_table[i];
		macro_hash_table[i] = NULL;
		while(m) {
			macro_desc * const n = m->next;
			free_macro_desc(m);
			m = n;
		}
	}
}

/* Find any key strokes that currently map to commands[i].name or commands[i].short_name.
   Returns either NULL or a char string that must be freed by the caller. */
char *find_key_strokes(int c) {
	char *str=NULL, *p;

	for(int i = 0; i < NUM_KEYS; i++) {
		if (key_binding[i]) {
			if (((!strncasecmp(commands[c].short_name,key_binding[i],strlen(commands[c].short_name))) &&
				  ((!key_binding[i][strlen(commands[c].short_name)]      ) ||
				   (key_binding[i][strlen(commands[c].short_name)] == ' ')
				  )
				 ) ||
				 ((!strncasecmp(commands[c].name,key_binding[i],strlen(commands[c].name))) &&
				  ((!key_binding[i][strlen(commands[c].name)]      ) ||
				   (key_binding[i][strlen(commands[c].name)] == ' ')
				  )
				 )
				) {
				if (!str) {
					if (!(str = malloc(14))) return NULL;
					strcpy(str,"Bound key(s):");
				}
				if (p = realloc(str,strlen(str) + strlen(key_stroke[i]) + 2)) {
					str = strcat(strcat(p," "),key_stroke[i]);
				} else {
					free(str);
					return NULL;
				}
			}
		}
	}
	return str;
}

static int help_cmd(int cmd) {
	req_list rl = { .ignore_tab=false };
	int s;
	print_message("Help: press Enter or Tab, or F1 or Escape or Escape-Escape");
	char *key_strokes, **tmphelp;
	if ((key_strokes = find_key_strokes(cmd)) && (tmphelp = calloc(commands[cmd].help_len+1, sizeof(char *)))) {
		tmphelp[0] = (char *)commands[cmd].help[0];
		tmphelp[1] = (char *)commands[cmd].help[1];
		tmphelp[2] = key_strokes;
		memcpy(&tmphelp[3], &commands[cmd].help[2], sizeof(char *) * (commands[cmd].help_len-2));
		rl.cur_entries = commands[cmd].help_len+1;
		rl.alloc_entries = 0;
		rl.max_entry_len = ne_columns;
		rl.entries = tmphelp;
		s = request_strings(&rl, 0);
		free(tmphelp);
	} else {
		rl.cur_entries = commands[cmd].help_len;
		rl.alloc_entries = 0;
		rl.max_entry_len = ne_columns;
		rl.entries = (char **)commands[cmd].help;
		s = request_strings(&rl, 0);
	}
	if (key_strokes) free(key_strokes);

	return s;
}

/* This function helps. The help text relative to the command name pointed to
   by p is displayed (p can also contain arguments). The string *p is not
   free'd by help(). If p is NULL, the
   alphabetically ordered list of commands is displayed with the string
   requester. The help finishes when the user escapes. */

int help(const char *p) {
	req_list rl = { .ignore_tab=true };

	D(fprintf(stderr,"Help Called with parm %p.\n",p);)
	int r = 0, last_selected = 0;
	do {
		print_message("Help: select Command and press Enter, or F1 or Escape or Escape-Escape");
		rl.cur_entries = ACTION_COUNT;
		rl.alloc_entries = 0;
		rl.max_entry_len = MAX_COMMAND_WIDTH;
		rl.entries = (char **)command_names;
		if (p || (r = request_strings(&rl, r)) >= 0) {
			D(fprintf(stderr,"Help check #2: p=%p, r=%d\n",p,r);)
			if (p) {
				for(r = 0; r < strlen(p); r++) if (isasciispace((unsigned char)p[r])) break;

				r = hash_cmd(p, r);
				D(fprintf(stderr,"Help check #3: p=%p, *p=%s, r=%d\n",p,p,r);)

				action a;
				if ((a = hash_table[r]) && !cmdcmp(commands[--a].name, p)
				|| (a = short_hash_table[r]) && !cmdcmp(commands[--a].short_name, p)) r = a;
				else r = -1;
				D(fprintf(stderr,"Help check #4: r=%d\n",r);)

				p = NULL;
			}
			else {
				D(fprintf(stderr,"Gonna parse_command_line(\"%s\",NULL,NULL,false);\n",command_names[r]);)
				r = parse_command_line(command_names[r], NULL, NULL, false);
				D(fprintf(stderr,"...and got r=%d\n",r);)
			}

			if (r < 0) {
				r = 0;
				continue;
			}

			assert(r >= 0 && r < ACTION_COUNT);
			last_selected = r;

			int s = help_cmd(r);
			if (s == ERROR) r = s;
		}
	} while(r >= 0);
	draw_status_bar();
	return last_selected;
}

/* Present a requester of the internal commands, selecting the best match for
   given prefix. Oddly, it only shows long command names, even if the best
   match is an abbreviated command name. The assumption is that this would be
   used primarily by users who are still learning their way through ne's
   commands. They probably are iffy on the command<->abbreviated command name
   mapping, too. So though it presents long command names, it returns a
   pointer to the selected abbreviated command. This helps users learn the
   short names for commands. */

char *request_command(const char * const prefix, bool use_prefix) {
	/* Take some short-cuts in setting up the req_list. We can because
	   command_names is a perfectly cromulent list for such purposes. */
	req_list rl = { .ignore_tab=false };
	rl.cur_entries = ACTION_COUNT;
	rl.alloc_entries = 0;
	rl.max_entry_len = MAX_COMMAND_WIDTH;
	rl.entries = (char **)command_names;

	/* Iterate through the commands, both long and short names, looking for the best match for the prefix. */
	int best_match_len = -1;
	int best_match = 0;
	for (int iteration=0; iteration<2; iteration++) {
		for (int r=0; r<ACTION_COUNT; r++) {
			const char *c = iteration ? commands[r].short_name : commands[r].name;
			const char *m = prefix;
			int len = 0;
			while (*c && *m && ascii_up_case[*(unsigned char *)c] == ascii_up_case[*(unsigned char *)m]) {
				c++;
				m++;
				len++;
			}
			if (len > best_match_len) {
				best_match_len = len;
				best_match = r;
			}
		}
	}
	rl.fuzz_len = best_match_len;
	while (true ) {
		print_message("Command: select Command and press Enter, Tab for help, or F1 or Escape or Escape-Escape");
		best_match = request_strings(&rl, best_match);
		if (best_match >= 0) {
			char *result = malloc(strlen(commands[best_match].short_name) + 2);
			if (!result) {
				alert(); /* OUT_OF_MEMORY */
				return NULL;
			}
			strcpy(result,commands[best_match].short_name);
			strcat(result," ");
			draw_status_bar();
			return result;
		} else if ( best_match == ERROR ) {
			draw_status_bar();
			return NULL;
		} else {
			best_match = -best_match - 2;
			help_cmd( best_match ); /* user selected via TAB, so pull up help. */
		}
	}
}
