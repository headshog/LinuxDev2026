#!/bin/sh
# Compare rhasher output with md5sum, sha1sum and published TTH values
set -eu
LC_ALL=C
export LC_ALL

rhasher=${RHASHER:-$(pwd -P)/rhasher}
test_dir=$(mktemp -d "${TMPDIR:-/tmp}/rhasher-test.XXXXXX")
trap 'rm -rf "$test_dir"' 0
trap 'exit 1' HUP INT TERM
cd "$test_dir"
passed=0
failed=0

check()
{
    if [ "$2" = "$3" ]; then
        passed=$((passed + 1))
    else
        failed=$((failed + 1))
        printf 'FAIL: %s\n  expected: %s\n  actual:   %s\n' "$1" "$2" "$3" >&2
    fi
}

run()
{
    printf '%s\n' "$1" | "$rhasher"
}

to_hex()
{
    od -An -v -tx1 | tr -d ' \n'
}

base64_hex()
{
    printf '%s' "$1" | base64 -d | to_hex
}

base32_hex()
{
    printf '%s' "$1" | base32 -d | to_hex
}

# check_digest ALGORITHM UTILITY ARGUMENT < DATA
check_digest()
{
    expected=$("$2" | cut -d' ' -f1)
    check "$1 $3" "$expected" "$(run "$1 $3")"
    lower=$(printf '%s' "$1" | tr '[:upper:]' '[:lower:]')
    check "$lower $3" "$expected" "$(base64_hex "$(run "$lower $3")")"
}

# Strings and files compared with coreutils
long=$(printf '%01000d' 0)
for string in '' abc 'Hello,world!' "$long"; do
    printf '%s' "$string" > string
    check_digest MD5 md5sum "\"$string" < string
    check_digest SHA1 sha1sum "\"$string" < string
done

: > empty
printf 'First line\nSecond line\n' > text
head -c 300000 /dev/urandom > random
for file in empty text random "$rhasher"; do
    check_digest MD5 md5sum "$file" < "$file"
    check_digest SHA1 sha1sum "$file" < "$file"
done

# Base64 output in its exact form
check 'md5 "' '1B2M2Y8AsgTpgAmY7PhCfg==' "$(run 'md5 "')"
check 'sha1 "' '2jmj7l5rSw0yVb/vlWAYkK/YBwk=' "$(run 'sha1 "')"

# TTH: published base32 values, file and string, optional rhash utility
tth_empty=$(base32_hex LWPNACQDBZRYXW3VHJVCJ64QBZNGHOHHHZWCLNQ=)
tth_abc=$(base32_hex ASD4UJSEH5M47PDYB46KBTSQTSGDKLBHYXOMUIA=)
printf abc > abc
check 'TTH "' "$tth_empty" "$(run 'TTH "')"
check 'TTH "abc' "$tth_abc" "$(run 'TTH "abc')"
check 'tth "abc' "$tth_abc" "$(base64_hex "$(run 'tth "abc')")"
check 'TTH empty' "$tth_empty" "$(run 'TTH empty')"
check 'TTH abc' "$tth_abc" "$(run 'TTH abc')"
if command -v rhash > /dev/null 2>&1; then
    for file in text random; do
        check "TTH $file" "$(rhash --printf='%x{tth}' "$file")" \
            "$(run "TTH $file")"
    done
fi

# Command line syntax
abc_md5=$(printf abc | md5sum | cut -d' ' -f1)
abc_sha1=$(printf abc | sha1sum | cut -d' ' -f1)
check 'closing quote' "$abc_md5" "$(run 'MD5 "abc"')"
check 'extra words' "$abc_sha1" "$(run '  SHA1   "abc  ignored words')"
check 'tabs' "$abc_sha1" "$(run "$(printf 'SHA1\t"abc')")"
check 'Md5 is hex' "$abc_md5" "$(run 'Md5 "abc')"
check 'mD5 is base64' "$abc_md5" "$(base64_hex "$(run 'mD5 "abc')")"

# Errors go to stderr and do not stop the session
mkdir directory
printf '%s\n' 'MD5 "abc' 'nosuchhash "abc' 'MD5 no-such-file' 'MD5' '' \
    'MD5 directory' 'SHA1 "abc' > session
status=0
"$rhasher" < session > session.out 2> session.err || status=$?
check 'session status' 0 "$status"
check 'session stdout' "$(printf '%s\n' "$abc_md5" "$abc_sha1")" \
    "$(cat session.out)"
check 'session stderr lines' 4 "$(wc -l < session.err | tr -d ' ')"

# With a terminal the prompt is printed (readline or getline)
if command -v script > /dev/null 2>&1 &&
   script -qec true /dev/null < /dev/null > /dev/null 2>&1; then
    printf 'SHA1 "abc\n' | timeout 10 script -qec "$rhasher" /dev/null \
        > tty.out 2>&1 || :
    check 'tty prompt' yes "$(grep -q '> ' tty.out && echo yes || echo no)"
    check 'tty hash' yes "$(grep -q "$abc_sha1" tty.out && echo yes || echo no)"
else
    echo 'SKIP: no pseudo-terminal for interactive test'
fi

printf '%s passed, %s failed\n' "$passed" "$failed"
[ "$failed" -eq 0 ]
