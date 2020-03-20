#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "textbuffer.h"
typedef struct lineBuffer *LB;

struct textbuffer {
    LB first;
    LB last;
    int nLines;
};

struct lineBuffer {
    char *value;
    LB next;
    LB prev;
};



static LB newLine(char *text, LB first);
static char *richText(char *string, char *open, char *close, int start, int middle);
/**
 * Allocate a new textbuffer whose contents is initialised with the text
 * in the given string.
 */
TB newTB (char *text) {
    TB new = malloc(sizeof(struct textbuffer));
    
    new->first = NULL;
    new->last = NULL;
    new->nLines = 0;
    if (strcmp(text, "") != 0) {
        char *curr = malloc(strlen(text) + 1);
        char *originalCurr = curr;
        strcpy(curr, text);
	    char *next = strchr(curr, '\n');

        while (next != NULL) {
            next[0] = '\0';
            
            char *value = malloc(strlen(curr)+1);
            strcpy(value, curr);
            if (new->first == NULL) {
                new->first = newLine(value, new->first);
                new->last = new->first;
            } else {
                new->last->next = newLine(value, new->last);
                new->last = new->last->next;
            }
            new->nLines++;
            curr = &next[1];
            next = strchr(curr, '\n');
        }
        free(originalCurr);
        free(next);
        
    }
	return new;

}

/**
 * Free  the  memory occupied by the given textbuffer. It is an error to
 * access the buffer afterwards.
 */
void releaseTB(TB tb) {
	while (tb->first != NULL) {
	    LB curr = tb->first;
	    if (tb->first != NULL) tb->first = tb->first->next;
	    if (tb->first != NULL) tb->first->prev = NULL;
	    free(curr->value);
		free(curr);
	}
	free(tb);
}


/**
 * Allocate  and return a string containing all of the text in the given
 * textbuffer. If showLineNumbers is true, add a prefix corresponding to
 * the line number.
 */
char *dumpTB (TB tb, bool showLineNumbers) {
    char *dumpLines;
    if (tb->nLines == 0) {
        return calloc(1,1);
    } else {
        dumpLines = malloc(1);
        strcpy(dumpLines, "\0");
        LB curr = tb->first;
        int dumpSize = 1;
        for (int i = 1; curr != NULL; i++) {
            char *temp;
            int alloc;
            if (showLineNumbers) {
                int lineNum = 1 + (int) log10(i);
                alloc = lineNum + strlen(". \n") + strlen(curr->value) + 1;
                temp = calloc(alloc, sizeof(char));
                sprintf(temp, "%d. %s\n", i, curr->value);
            } else {
                alloc = strlen(curr->value) + 2;
                temp = calloc(alloc, sizeof(char));
                sprintf(temp, "%s\n", curr->value);
            }
            //printf("dumpSize %d, Alloc %d\n", dumpSize, alloc);
            dumpLines = realloc(dumpLines, dumpSize + alloc);
            dumpSize += alloc;
            strcat(dumpLines, temp);
            free(temp);
            curr = curr->next;
        }
        free(curr);
    }
    return dumpLines;
}

/**
 * Return the number of lines of the given textbuffer.
 */
int linesTB (TB tb) {
	return tb->nLines;
}

/**
 * Add a given prefix to all lines between 'from' and 'to', inclusive.
 * - The  program  should abort() wih an error message if 'from' or 'to'
 *   is out of range. The first line of a textbuffer is at position 1.
 */
void addPrefixTB (TB tb, int from, int to, char *prefix) {
    if (from < 1 || from > to) {
        printf("'from' is out of range\n");
        abort();
    } else if (to < from || to > tb->nLines) {
        printf("'to' is out of range\n");
        abort();
    }
    if (strcmp(prefix, "") != 0) {
        LB curr = tb->first;
        for (int i = 1; i < from; i++) {
            curr = curr->next;
        }
        for (int i = from; i <= to; i++) {
            int alloc = strlen(prefix) + strlen(curr->value);
            char *temp = calloc(alloc, sizeof(char));
            sprintf(temp, "%s%s", prefix, curr->value);    // stores "prefix value"
            curr->value = realloc(curr->value, alloc);
            strcpy(curr->value, temp);
            free(temp);
            curr = curr->next;
        }
    }
}

/**
 * Merge 'tb2' into 'tb1' at line 'pos'.
 * - After this operation:
 *   - What was at line 1 of 'tb2' will now be at line 'pos' of 'tb1'.
 *   - Line  'pos' of 'tb1' will moved to line ('pos' + linesTB('tb2')),
 *     after the merged-in lines from 'tb2'.
 *   - 'tb2' can't be used anymore (as if we had used releaseTB() on it)
 * - The program should abort() with an error message if 'pos' is out of
 *   range.
 */
void mergeTB (TB tb1, int pos, TB tb2) {
    //char *text1 = dumpTB(tb1, false);
    //char *text2 = dumpTB(tb2, false);
    //if (strcmp(text1, text2) != 0) {
        pasteTB(tb1, pos, tb2);
        releaseTB(tb2);
    //}
}

/**
 * Copy 'tb2' into 'tb1' at line 'pos'.
 * - After this operation:
 *   - What was at line 1 of 'tb2' will now be at line 'pos' of 'tb1'.
 *   - Line  'pos' of 'tb1' will moved to line ('pos' + linesTB('tb2')),
 *     after the pasted-in lines from 'tb2'.
 *   - 'tb2' is unmodified and remains usable independent of tb1.
 * - The program should abort() with an error message if 'pos' is out of
 *   range.
 */
void pasteTB (TB tb1, int pos, TB tb2) {
    if (pos > tb1->nLines+1) {
        printf("Pos is too large\n");
        abort();
    } else if (pos < 1) {
        printf("Pos is too small\n");
        abort();
    }
    
    LB curr = NULL;
    LB currtb1 = tb1->first;
    LB currtb2 = tb2->first;
    int lines = tb1->nLines + tb2->nLines;
    for (int i = 1; i <= lines; i++) {
        if (i >= pos && i < pos + tb2->nLines) {
            
            char *value = malloc(strlen(currtb2->value) + 1);
            strcpy(value, currtb2->value);
            LB newtb2 = newLine(value, curr);
            
            if (curr == NULL) {
                tb1->first = newtb2;
                curr = tb1->first;
            } else {
                
                curr->next = newtb2;
                curr = curr->next;
            }
            currtb2 = currtb2->next;
            tb1->nLines++;
        } else {
            if (i == 1) {
                curr = currtb1;
                currtb1 = currtb1->next;
            } else {
                curr->next = currtb1;
                curr->next->prev = curr;
                curr = curr->next;
                currtb1 = currtb1->next;
            }
        }
    }
}

/**
 * Cut  the lines between and including 'from' and 'to' out of the given
 * textbuffer 'tb' into a new textbuffer.
 * - The result is a new textbuffer (much as one created with newTB()).
 * - The cut lines will be deleted from 'tb'.
 * - The  program should abort() with an error message if 'from' or 'to'
 *   is out of range.
 */
TB cutTB (TB tb, int from, int to) {
    if (from < 1) {
        printf("'from' is out of range\n");
        abort();
    } else if (to > tb->nLines) {
        printf("'to' is out of range\n");
        abort();
    }
    if (to < from) return NULL;
    else {
        LB curr = tb->first;
        for (int i = 1; i < from; i++) {
            curr = curr->next;
        }
        TB new = malloc(sizeof(TB));
        new->first = NULL;
        new->last = NULL;
        new->nLines = 0;
        
        for (int i = from; i <= to; i++) {
            char *val = malloc(strlen(curr->value) + 1);
            strcpy(val, curr->value);
            if (new->first == NULL) {
                new->first = newLine(val, new->first);
                new->last = new->first;
            } else {
                new->last->next = newLine(val, new->last);
                new->last = new->last->next;
            }
            curr = curr->next;
            new->nLines++;
        }
        deleteTB(tb, from, to);
	    return new;
    }
}

/**
 * Return a linked list of match nodes containing the positions of all
 * of the matches of string 'search' in 'tb'.
 * - The textbuffer 'tb' should remain unmodified.
 * - The user is responsible for freeing the returned list.
 */
Match searchTB (TB tb, char *search) {
    Match head = NULL;
    if (strcmp(search, "") != 0) {
        LB currLB = tb->first;
        Match currMatch;
        for (int i = 1; currLB != NULL; i++) {
            char *text = currLB->value;
            while (strstr(text, search) != NULL) {
                if (head == NULL) {
                    currMatch = malloc(sizeof(Match));
                    head = currMatch;
                } else {
                    currMatch->next = malloc(sizeof(Match));
                    if (currMatch != NULL) {
                        currMatch = currMatch->next;
                        currMatch->next = NULL;
                    }
                }
                char *temp = strstr(text, search);
                // column = ... +1 since 1 indexed
                int column = strlen(currLB->value) - strlen(temp) + 1;
                currMatch->columnNumber = column;
                currMatch->lineNumber = i;
                text = &temp[strlen(search)];
            }
            currLB = currLB->next;
        }
    }
    return head;
}

/**
 * Remove  the  lines between 'from' and 'to' (inclusive) from the given
 * textbuffer 'tb'.
 * - The  program should abort() with an error message if 'from' or 'to'
 *   is out of range.
 */
void deleteTB (TB tb, int from, int to) {
    if (from < 1 || from > to) {
        printf("'from' is out of range\n");
        abort();
    } else if (to > tb->nLines) {
        printf("'to' is out of range\n");
        
        abort();
    }
    LB curr = tb->first;
    for (int i = 1; curr != NULL; i++) {
        if (i >= from && i <= to) {
            if (curr->prev == NULL) {
			    curr = curr->next;
			    if (curr != NULL) {
			        free(curr->prev->value);
			        free(curr->prev);
			        curr->prev = NULL;
			        tb->first = curr;
			    }
		    } else {
		        LB temp = curr->next;
			    curr->prev->next = curr->next;
			    if (curr->next == NULL) tb->last = curr->prev;
			    else curr->next->prev = curr->prev;
			    free(curr->value);
			    free(curr);
			    curr = temp;
		    }
            tb->nLines--;
        } else {
            curr = curr->next;
        }
    }
}

/**
 * Search  every  line of the given textbuffer for every occurrence of a
 * set of specified substitutions and alter them accordingly.
 * - Refer to the spec for delasts.
 */
void formRichText (TB tb) {
    LB curr = tb->first;
    while (curr != NULL) {
        int isHash = 0;
        int matchStar = -1;
        int matchUnderscore = -1;
        for (int i = 0; i < strlen(curr->value); i++) {
            
            char *originalVal = curr->value;
            // Searching for '#' at start
            if (curr->value[0] == '#' && i > 0) {
                curr->value = richText(curr->value, "<h1>", "</h1>", 0, strlen(curr->value));
                free(originalVal);
                isHash = 1;
            // Searching for '*'
            } else if (curr->value[i] == '*' && isHash == 0) {
                // The two '*'s are next to each other
                if (matchStar == -1 || (i-matchStar) == 1) {
                    matchStar = i;
                // Case of nested special character
                } else if (matchStar > matchUnderscore && matchUnderscore != -1) {
                    for (int n = i; n < strlen(curr->value); n++) {
                        if (curr->value[n] == '_') {
                            matchStar = -1;
                            break;
                        }
                    }
                    
                    if (matchStar != -1) {
                        curr->value = richText(curr->value, "<b>", "</b>", matchStar, i);
                        free(originalVal);
                        matchStar = -1;
                    }
                } else {
                    curr->value = richText(curr->value, "<b>", "</b>", matchStar, i);
                    free(originalVal);
                    matchStar = -1;
                }
            } else if (curr->value[i] == '_' && isHash == 0) {
                if (matchUnderscore == -1 || (i-matchUnderscore) == 1) {
                    matchUnderscore = i;
                } else if (matchUnderscore > matchStar && matchStar != -1) {
                    for (int n = i; n < strlen(curr->value); n++) {
                        if (curr->value[n] == '*') {
                            matchUnderscore = -1;
                            break;
                        }
                    }
                    
                    if (matchUnderscore != -1) {
                        curr->value = richText(curr->value, "<i>", "</i>", matchUnderscore, i);
                        free(originalVal);
                        matchUnderscore = -1;
                    }
                } else {
                    curr->value = richText(curr->value, "<i>", "</i>", matchUnderscore, i);
                    free(originalVal);
                    matchUnderscore = -1;
                }
            }
        }
        curr = curr->next;
    }
}


static LB newLine(char *text, LB prev) {
    LB new = malloc(sizeof(LB)); 
    if (new != NULL) {
	    new->value = &text[0];
	    new->next = NULL;
	    new->prev = prev;
    }
	return new;
}

// Returns altered Rich Text string
static char *richText(char *string, char *open, char *close, int start, int middle) {
    char *temp = calloc(strlen(string) + strlen(open) + strlen(close), sizeof(char));
    int end = strlen(string);
    strncat(temp, string, start);
    strcat(temp, open);
    string = &string[start+1];
    strncat(temp, string, middle-start-1);
    string = &string[middle-start];
    strcat(temp, close);
    if (end != middle) strncat(temp, string, end-middle-1);
    string = temp;
    return temp;
}