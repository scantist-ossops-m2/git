#!/bin/sh

test_description='verify safe.directory checks'

. ./test-lib.sh

GIT_TEST_ASSUME_DIFFERENT_OWNER=1
export GIT_TEST_ASSUME_DIFFERENT_OWNER

expect_rejected_dir () {
	test_must_fail git status 2>err &&
	grep "safe.directory" err
}

test_expect_success 'safe.directory is not set' '
	expect_rejected_dir
'

test_expect_success 'safe.directory does not match' '
	git config --global safe.directory bogus &&
	expect_rejected_dir
'

test_expect_success 'path exist as different key' '
	git config --global foo.bar "$(pwd)" &&
	expect_rejected_dir
'

test_expect_success 'safe.directory matches' '
	git config --global --add safe.directory "$(pwd)" &&
	git status
'

test_expect_success 'safe.directory matches, but is reset' '
	git config --global --add safe.directory "" &&
	expect_rejected_dir
'

test_expect_success 'safe.directory=*' '
	git config --global --add safe.directory "*" &&
	git status
'

test_expect_success 'safe.directory=*, but is reset' '
	git config --global --add safe.directory "" &&
	expect_rejected_dir
'

test_expect_success 'local clone of unowned repo refused in unsafe directory' '
	test_when_finished "rm -rf source" &&
	git init source &&
	(
		sane_unset GIT_TEST_ASSUME_DIFFERENT_OWNER &&
		test_commit -C source initial
	) &&
	test_must_fail git clone --local source target &&
	test_path_is_missing target
'

test_expect_success 'local clone of unowned repo accepted in safe directory' '
	test_when_finished "rm -rf source" &&
	git init source &&
	(
		sane_unset GIT_TEST_ASSUME_DIFFERENT_OWNER &&
		test_commit -C source initial
	) &&
	test_must_fail git clone --local source target &&
	git config --global --add safe.directory "$(pwd)/source/.git" &&
	git clone --local source target &&
	test_path_is_dir target
'

test_done
