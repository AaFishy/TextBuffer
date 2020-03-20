// Test for textbuffer 

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "textbuffer.h"

static void testNewTB(void);
static void testPrefix(void);
static void testPaste(void);
static void testMerge(void);
static void testDelete(void);
static void testCut(void);
static void testSearch(void);
static void printMatch(Match node);
static void freeMatch(Match node);

static void testRich(void);
static void quickRich(char *rich);
static void testRich(void);
int main(void) {
	testNewTB();
	testPrefix();
	testPaste();
	testMerge();
	testDelete();
	testCut();
	testSearch();
	testRich();
}

static void quickRich(char *rich) {
	TB tb1 = newTB(rich);
	formRichText(tb1);
	char *text1 = dumpTB(tb1, false);
	printf("%s%s\n", rich,text1);
	free(text1);
	
	releaseTB(tb1);
}
static void testRich(void) {
	printf("===Testing formRichText===\n");
	quickRich("*some string\n");
	quickRich("*some string*lol*\n");
	quickRich("* *\n");
	quickRich("*some_string*again_\n");
	quickRich("*some* _string_\n");
	quickRich("some *string_again_\n");
	quickRich("some#string*once_again*\n");
	quickRich("#string_stuff_\n");
	quickRich("#\n");
	quickRich("###\n");	
}
static void freeMatch(Match node) {
	Match cur = node;
	while (node != NULL) {
		node = node->next;
		free(cur);
		cur = node;
	}
}
static void printMatch(Match node) {
	Match cur = node;
	for (; cur != NULL; cur = cur->next) {
		printf("LINE: %d COL: %d\n", cur->lineNumber, cur->columnNumber);
	}
}
static void testSearch(void) {
	printf("===Testing searchTB===\n");
	printf("Making new TB\n");
	TB tb1 = newTB("Hello World My\nname is jarred lovegood\nand i love carley ray jepson\n");
	char *text1 = dumpTB(tb1, false);
	printf("%s", text1);
	free(text1);
	Match found = searchTB(tb1, "love");
	printMatch(found);
	freeMatch(found);
	releaseTB(tb1);
}
static void testCut(void) {
	printf("===Testing cutTB===\n");
	printf("Making new TB\n");
	TB tb1 = newTB("one\ntwo\nthree\nfour\nfive\nsix\n");
	char *text1 = dumpTB(tb1,false);
	printf("%s", text1);
	free(text1);
	printf("Cutting TB\n");
	TB tb2 = cutTB(tb1, 1,6);
	text1 = dumpTB(tb2, false);
	printf("%s", text1);
	free(text1);
	printf("Old TB:\n");
	text1 = dumpTB(tb1,false);
	printf("%s", text1);
	free(text1);

	releaseTB(tb1);
	releaseTB(tb2);
}
static void testDelete(void) {
	printf("===Testing deleteTB===\n"	);
	printf("Creating new TB\n");
	TB tb1 = newTB("line1\nline2\nline3\nline4\n");
	printf("NUMBER OF LINES should be 4: %d \n", linesTB(tb1));
	printf("Printing TB\n");
	char *text1 = dumpTB(tb1, false);
	printf("%s", text1);
	free(text1);
	deleteTB(tb1, 1,4);
	printf("NUMBER OF LINES should be 1: %d \n", linesTB(tb1));
	text1 = dumpTB(tb1, false);
	printf("%s", text1);
	free(text1);
	printf("Freeing TB\n");
	releaseTB(tb1);
	printf("Success\n");
}
static void testMerge(void) {
	printf("===Testing MergeTB===\n");
	printf("Creating new TB\n");
	TB tb1 = newTB("line1\nline2\nline3\nline4\n");
	char *text1 = dumpTB(tb1, false);
	printf("%s", text1);

	printf("Creating 2nd new TB\n");
	TB tb2 = newTB("1line\n2line\n3line\n4line\n");
	char *text2 = dumpTB(tb2, false);
	printf("%s", text2);

	mergeTB(tb1, 2, tb2);

	printf("printing merged\n");
	char *text3 = dumpTB(tb1, false);
	printf("%s", text3);

	releaseTB(tb1);
	free(text1);
	free(text2);
	free(text3);
}
static void testPaste(void) {
	printf("===Testing pasteTB===\n");
	printf("Creating new TB\n");
	TB tb1 = newTB("line1\nline2\nline3\nline4\n");
	char *text1 = dumpTB(tb1, false);
	printf("%s", text1);

	printf("Creating 2nd new TB\n");
	TB tb2 = newTB("1line\n2line\n3line\n4line\n");
	char *text2 = dumpTB(tb2, false);
	printf("%s", text2);

	pasteTB(tb1, 2, tb2);

	printf("printing combined\n");
	char *text3 = dumpTB(tb1, false);
	printf("%s", text3);

	releaseTB(tb1);
	releaseTB(tb2);
	free(text1);
	free(text2);
	free(text3);
}
static void testNewTB(void) {
	printf("===Testing newTB; releaseTB; dumpTB===\n"	);
	printf("Creating new TB\n");
	TB tb1 = newTB("line1\nline2\nline3\nline4\n");
	printf("NUMBER OF LINES should be 4: %d \n", linesTB(tb1));
	printf("Printing TB w/o numbers\n");
	char *text1 = dumpTB(tb1, false);
	printf("%s", text1);
	free(text1);
	printf("Printing TB w/ numbers\n");
	text1 = dumpTB(tb1, true);
	printf("%s", text1);
	printf("Freeing TB\n");
	releaseTB(tb1);
	printf("Success\n");
	free(text1);		
}
static void testPrefix(void) {
	printf("===Testing addPrefixtb===\n"	);
	printf("Creating new TB\n");
	TB tb1 = newTB("line1\nline2\nline3\nline4\n");
	printf("ADDING PREFIX\n");
	addPrefixTB(tb1, 1, 4, "PREFIX");
	printf("Printing TB w/o numbers\n");
	char *text1 = dumpTB(tb1, false);
	printf("%s", text1);
	free(text1);
	printf("Printing TB w/ numbers\n");
	text1 = dumpTB(tb1, true);
	printf("%s", text1);
	printf("Freeing TB\n");
	releaseTB(tb1);
	printf("Success\n");
	free(text1);
}




