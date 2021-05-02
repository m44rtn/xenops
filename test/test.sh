#!/bin/sh

TEST_FILE=test/test.h
SPACER='=============\n\n'

# navigate to root dir 
cd ..
printf 'this script will execute all xenops commands (short, long)\n\n'

# testing...
printf 'testing version\n'
./xenops -v
./xenops --version
printf $SPACER

printf 'testing quiet\n'
./xenops -q -f $TEST_FILE
./xenops --quiet --file $TEST_FILE
printf $SPACER

printf 'testing help\n'
./xenops -h
./xenops --help
printf $SPACER

printf 'testing build\n'
./xenops -b -f $TEST_FILE
./xenops --build --file $TEST_FILE
printf $SPACER

printf 'testing minor\n'
./xenops -mi -f $TEST_FILE
./xenops --minor --file $TEST_FILE
printf $SPACER

printf 'testing major\n'
./xenops -ma -f $TEST_FILE
./xenops --major --file $TEST_FILE
printf $SPACER

printf 'testing limit and overflow on build only\n'
./xenops -o b -l b=1 -f $TEST_FILE

# update build again to test the long command
./xenops -q -f $TEST_FILE

./xenops --overflow b --limit 1 --file $TEST_FILE
printf $SPACER

printf 'testing limit globally\n'
./xenops -o b, mi -l 1 -f $TEST_FILE
./xenops --overflow b, mi --limit 1 --file $TEST_FILE
printf $SPACER

printf 'testing limit and overflow on build only but with overflow also enabled for minor\n'
printf '(should not update MAJOR)\n'
# first update build to one so that we can test overflow
./xenops -q -f $TEST_FILE

./xenops -o b, mi -l b=1 -f $TEST_FILE

# update build for long command
./xenops -q -f $TEST_FILE

./xenops --overflow b --limit 1 --file $TEST_FILE
printf $SPACER

printf 'testing prefix\n'
./xenops -p TEST_ -f $TEST_FILE
./xenops --prefix TEST_ --file $TEST_FILE
printf $SPACER

