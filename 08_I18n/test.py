import os
import re
import subprocess


binary = os.environ.get("GUESS_NUMBER", "./guess-number")
russian = os.environ.get("TEST_LANGUAGE") == "ru"
env = dict(os.environ)
env["LC_ALL"] = "ru_RU.UTF-8" if russian else "C"
env["LANGUAGE"] = "ru" if russian else "C"
yes, no = ("Да", "Нет") if russian else ("Yes", "No")
intro = "Загадайте целое число от 1 до 100." if russian else "Think of an integer between 1 and 100."
question = r"Ваше число больше (\d+)\?" if russian else r"Is your number greater than (\d+)\?"
result = "Ваше число: %d.\n" if russian else "Your number is %d.\n"
invalid = (f'Введите в точности "{yes}" или "{no}".\n' if russian
           else f'Please answer exactly "{yes}" or "{no}".\n')
closed = "Ошибка: стандартный ввод закрыт.\n" if russian else "Error: standard input was closed.\n"


def run(text):
    return subprocess.run([binary], input=text, capture_output=True, text=True,
                          encoding="utf-8", timeout=5, env=env)


def answers(number):
    low, high = 1, 100
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


for number in range(1, 101):
    replies, middles = answers(number)
    process = run("\n".join(replies) + "\n")
    assert process.returncode == 0, process
    assert process.stderr == "", process.stderr
    assert process.stdout.startswith(intro + "\n"), process.stdout
    assert process.stdout.endswith(result % number), process.stdout
    assert list(map(int, re.findall(question, process.stdout))) == middles, process.stdout
    assert middles[0] == 50 and len(middles) <= 7
    assert f"({yes}/{no})" in process.stdout, process.stdout

replies, middles = answers(73)
for wrong in ["", "maybe", "yes", "YES", yes + " ", " " + no,
              "No" if russian else "Нет", "x" * 10000, yes + "\0junk"]:
    process = run(wrong + "\n" + "\n".join(replies) + "\n")
    assert process.returncode == 0, process
    assert process.stderr == invalid, process.stderr
    assert process.stdout.endswith(result % 73), process.stdout
    assert list(map(int, re.findall(question, process.stdout))) == [50] + middles

for text in ["", yes + "\n", "maybe\n"]:
    process = run(text)
    assert process.returncode != 0, process
    assert process.stderr.endswith(closed), process.stderr
    assert ("Ваше число:" if russian else "Your number is") not in process.stdout

# A final answer without a trailing newline is still a complete answer.
replies, _ = answers(1)
process = run("\n".join(replies))
assert process.returncode == 0 and process.stdout.endswith(result % 1), process

print(f"{'Russian' if russian else 'English'}: all 100 numbers, invalid input and EOF: OK")
