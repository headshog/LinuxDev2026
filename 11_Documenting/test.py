import os
import re
import subprocess


binary = os.environ.get("GUESS_NUMBER", "./guess-number")
russian = os.environ.get("TEST_LANGUAGE") == "ru"
env = dict(os.environ, LC_ALL="ru_RU.UTF-8" if russian else "C",
           LANGUAGE="ru" if russian else "C")
yes, no = ("Да", "Нет") if russian else ("Yes", "No")
intro = "Загадайте целое число от %s до %s.\n" if russian else "Think of an integer between %s and %s.\n"
question = r"Ваше число больше ([0-9IVXLCDM]+)\?" if russian else r"Is your number greater than ([0-9IVXLCDM]+)\?"
result = "Ваше число: %s.\n" if russian else "Your number is %s.\n"
invalid = (f'Введите в точности "{yes}" или "{no}".\n' if russian
           else f'Please answer exactly "{yes}" or "{no}".\n')
closed = "Ошибка: стандартный ввод закрыт.\n" if russian else "Error: standard input was closed.\n"


def roman(value):
    digits = [
        ["", "M", "MM", "MMM"],
        ["", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"],
        ["", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"],
        ["", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"],
    ]
    return "".join(table[value // divisor % 10]
                   for table, divisor in zip(digits, [1000, 100, 10, 1]))


def run(args=(), text=""):
    return subprocess.run([binary, *args], input=text, capture_output=True,
                          text=True, encoding="utf-8", timeout=5, env=env)


def answers(number):
    low, high = 1, 3999
    replies, middles = [], []
    while low < high:
        middle = (low + high) // 2
        middles.append(middle)
        replies.append(yes if number > middle else no)
        if number > middle:
            low = middle + 1
        else:
            high = middle
    return replies, middles


numbers = sorted(set(range(1, 101)) | set(range(123, 4000, 137)) | {3888, 3998, 3999})
for use_roman in [False, True]:
    args = ["-r"] if use_roman else []
    display = roman if use_roman else str
    for number in numbers:
        replies, middles = answers(number)
        process = run(args, "\n".join(replies) + "\n")
        assert process.returncode == 0 and not process.stderr, process
        assert process.stdout.startswith(intro % (display(1), display(3999))), process.stdout
        assert process.stdout.endswith(result % display(number)), process.stdout
        assert re.findall(question, process.stdout) == list(map(display, middles))
        assert len(middles) <= 12 and f"({yes}/{no})" in process.stdout

    replies, middles = answers(1984)
    for wrong in ["", "maybe", "yes", "YES", yes + " ", "x" * 10000, yes + "\0junk"]:
        process = run(args, wrong + "\n" + "\n".join(replies) + "\n")
        assert process.returncode == 0 and process.stderr == invalid, process
        assert process.stdout.endswith(result % display(1984)), process.stdout
        assert re.findall(question, process.stdout) == list(map(display, [2000] + middles))
    for text in ["", yes + "\n", "maybe\n"]:
        process = run(args, text)
        assert process.returncode != 0 and process.stderr.endswith(closed), process

help_result = run(["--help"])
assert help_result.returncode == 0 and not help_result.stderr
assert ("Использование:" if russian else "Usage:") in help_result.stdout
assert yes in help_result.stdout and no in help_result.stdout
for option in ["--roman", "--help", "--version", "--help-md"]:
    assert option in help_result.stdout
version = run(["--version"])
assert version.returncode == 0 and version.stdout == "guess-number 0.2.0\n"
markdown = run(["--help-md"])
assert markdown.returncode == 0 and not markdown.stderr
assert "@mainpage guess-number\n@verbatim\nUsage:" in markdown.stdout
assert "@endverbatim" in markdown.stdout and "Answer exactly" in markdown.stdout
for args in [["--bad"], ["-z"], ["extra"], ["-r", "extra"], ["--roman=bad"]]:
    process = run(args)
    assert process.returncode != 0 and not process.stdout and process.stderr, process

print(f"{'Russian' if russian else 'English'}: decimal/Roman games, help, errors and EOF: OK")
