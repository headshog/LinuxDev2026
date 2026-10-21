#!/bin/sh
set -eu
LC_ALL=C
export LC_ALL

build_dir=$(pwd -P)
move=$build_dir/move
library=$build_dir/libprotect.so
safemove=$build_dir/safemove
test_dir=$(mktemp -d "${TMPDIR:-/tmp}/tracing-test.XXXXXX")
trap 'rm -rf "$test_dir"' 0
trap 'exit 1' HUP INT TERM
cd "$test_dir"
input=$test_dir/input
output=$test_dir/output
printf 'First line\nSecond line\n\000Binary data\377\n' > expected

check_status()
{
    expected_status=$1
    shift
    status=0
    "$@" > command.out 2> command.err || status=$?
    if [ "$status" -ne "$expected_status" ]; then
        cat command.err >&2
        printf 'FAIL: expected status %s, got %s: %s\n' \
            "$expected_status" "$status" "$*" >&2
        exit 1
    fi
}

prepare()
{
    rm -f "$input" "$output"
    cp expected "$input"
}

check_rollback()
{
    cmp expected "$input"
    test ! -e "$output"
}

fault()
{
    code=$1
    syscall=$2
    target=$3
    injected_error=$4
    prepare
    check_status "$code" strace -qq -o trace.log -P "$target" \
        -e "inject=$syscall:error=$injected_error" "$move" "$input" "$output"
    grep -q '(INJECTED)' trace.log
    check_rollback
    printf 'OK: %s -> %s\n' "$syscall/$injected_error" "$code"
}

# A real run also discovers which stat syscall this libc/architecture uses.
prepare
check_status 0 strace -qq -o trace.log -P "$input" -P "$output" \
    "$move" "$input" "$output"
test ! -e "$input"
cmp expected "$output"
stat_call=$(sed -n 's/^\(fstat\|fstat64\|newfstatat\|statx\)(.*/\1/p' trace.log | head -n 1)
test -n "$stat_call"

prepare
printf 'Old output must be replaced completely\n' > "$output"
check_status 0 "$move" "$input" "$output"
test ! -e "$input"
cmp expected "$output"
prepare
: > "$input"
check_status 0 "$move" "$input" "$output"
test ! -e "$input"
test ! -s "$output"
prepare
dd if=/dev/zero of=large bs=8192 count=3 2>/dev/null
cp large "$input"
check_status 0 "$move" "$input" "$output"
test ! -e "$input"
cmp large "$output"
printf 'OK: ordinary, overwrite, empty and multi-buffer copies\n'

check_status 1 "$move"
check_status 1 "$move" a b c
prepare
check_status 2 "$move" "$test_dir/missing" "$output"
check_rollback
check_status 4 "$move" "$test_dir" "$output"
check_rollback
check_status 8 "$move" "$input" "$input"
check_rollback
ln "$input" "$output"
check_status 8 "$move" "$input" "$output"
cmp expected "$input"
cmp expected "$output"
prepare
ln -s "$input" "$output"
check_status 5 "$move" "$input" "$output"
cmp expected "$input"
test -L "$output"
rm "$output"
ln -s "$input" "$test_dir/input-link"
check_status 2 "$move" "$test_dir/input-link" "$output"
check_rollback
check_status 5 "$move" "$input" "$test_dir/missing/output"
check_rollback
check_status 7 "$move" "$input" /dev/null
check_rollback
printf 'OK: invalid arguments, missing files, file types and aliases\n'

fault 2 openat "$input" EACCES
fault 3 "$stat_call" "$input" EIO
fault 5 openat "$output" EACCES
fault 6 "$stat_call" "$output" EIO
fault 9 ftruncate "$output" EIO
fault 10 read "$input" EIO
fault 11 write "$output" ENOSPC
fault 11 write "$output" EDQUOT
fault 12 fsync "$output" EIO
fault 13 close "$output" EIO
fault 14 close "$input" EIO
fault 15 unlinkat "$input" EPERM

prepare
check_status 11 strace -qq -o trace.log -P "$output" \
    -e inject=write:retval=0:when=1 "$move" "$input" "$output"
grep -q '(INJECTED)' trace.log
check_rollback
prepare
check_status 0 strace -qq -o trace.log -P "$input" \
    -e inject=read:error=EINTR:when=1 "$move" "$input" "$output"
grep -q '(INJECTED)' trace.log
test ! -e "$input"
cmp expected "$output"
printf 'OK: zero-length write and interrupted read\n'

prepare
check_status 16 strace -qq -o trace.log -P "$output" \
    -e inject=fsync:error=EIO -e inject=close:error=EIO \
    "$move" "$input" "$output"
grep -q '(INJECTED)' trace.log
check_rollback
prepare
check_status 17 strace -qq -o trace.log -P "$input" \
    -e "inject=$stat_call:error=EIO" -e inject=close:error=EIO \
    "$move" "$input" "$output"
grep -q '(INJECTED)' trace.log
check_rollback
prepare
check_status 18 strace -qq -o trace.log -P "$input" -P "$output" \
    -e inject=read:error=EIO -e inject=unlinkat:error=EPERM \
    "$move" "$input" "$output"
grep -q '(INJECTED)' trace.log
cmp expected "$input"
test -f "$output"
printf 'OK: errors while cleaning up\n'

prepare
check_status 0 env LD_PRELOAD="$library" "$move" "$input" "$output"
test ! -e "$input"
cmp expected "$output"
cp expected "$test_dir/input-PROTECT"
check_status 15 env LD_PRELOAD="$library" "$move" \
    "$test_dir/input-PROTECT" "$test_dir/protected-output"
cmp expected "$test_dir/input-PROTECT"
test ! -e "$test_dir/protected-output"
check_status 15 "$safemove" "$test_dir/input-PROTECT" "$test_dir/protected-output"
cmp expected "$test_dir/input-PROTECT"
test ! -e "$test_dir/protected-output"
mkdir "$test_dir/PROTECT-directory"
cp expected "$test_dir/PROTECT-directory/ordinary"
check_status 0 "$safemove" "$test_dir/PROTECT-directory/ordinary" "$output"
test ! -e "$test_dir/PROTECT-directory/ordinary"
cmp expected "$output"
printf 'OK: LD_PRELOAD, safemove and basename-only protection\n'
printf 'All tests passed\n'
