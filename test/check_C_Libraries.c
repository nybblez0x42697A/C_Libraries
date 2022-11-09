#include <check.h>
#include <stdlib.h>
#include <stdio.h>


#include "../include/C_Libraries.h"

START_TEST(test_split_notes)
{

}
END_TEST


Suite *check_C_Libraries_suite(void)
{
	Suite *suite;
	TCase *tc_core;


	// Creates the initial suite, because you kind of just need it
	//
	suite = suite_create("C_Libraries\_test");

	// Confirms successful creation/deletion of structure before continuing
	// to ensure proper handling prior to next Test Cases
	//
	tc_core = tcase_create("Core");

	tcase_add_test(tc_core, test_split_notes);

	suite_add_tcase(suite, tc_core);

	return suite;
}

int main(void)
{

	// Instantiate and initialize the Suite Runner
	//
	// If additional Test Suites are desired, add with srunner_add_suite()
	// Generally speaking:
	// - Suite Runner is responsibile testing entire program
	// - 1x Test Suite per library
	// - 1+ Test Cases per logical section
	// - Multiple Unit Tests per function
	//
	// The Test Cases and Unit Tests are logical groupings of things, so you
	// could say This Test Case handles file open and validation functions
	//
	// Other developers prefer to chunk their thoughts out differently,
	// This Test Cases are responsible for testing ALL my functions on
	// invalid input
	//
	Suite *suite = check_C_Libraries_suite();
	SRunner *runner = srunner_create(suite);

	// Prevents valigrind errors from appearing that could be mistakenly be
	// considered the programmers fault. Will run slower! For details, see:
	//
	// https://libcheck.github.io/check/doc/check_html/check_4.html#Finding-Memory-Leaks
	//
	srunner_set_fork_status(runner, CK_NOFORK);

	// Actual Suite Runner command of execution with desired verbosity level
	//
	srunner_run_all(runner, CK_VERBOSE);

	// Extract results for exit code handling
	//
	int no_failed = srunner_ntests_failed(runner);

	// free() all resources used for execution of Suite Runner.
	// Documentation mentions it also cleans up for Test Cases, but that
	// appears to be just THEIR portion, you are still responsible for
	// cleaning up after yourself
	//
	srunner_free(runner);

	return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

