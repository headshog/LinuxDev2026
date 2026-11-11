import os
import subprocess


bindir = os.path.abspath(os.environ.get("MAZE_BINDIR", "."))
checks = 0


def run(name, args):
    return subprocess.run([os.path.join(bindir, name), *map(str, args)],
                          capture_output=True, text=True, timeout=10)


def check(name, args, size, symbols=".#"):
    global checks
    process = run(name, args)
    assert process.returncode == 0, (name, args, process.stderr)
    assert not process.stderr, process.stderr
    lines = process.stdout.splitlines()
    passage, wall = symbols
    width = 2 * size + 1
    assert len(lines) == width and all(len(line) == width for line in lines)
    assert set("".join(lines)) <= set(symbols)
    assert lines[0] == lines[-1] == wall * width
    assert all(line[0] == line[-1] == wall for line in lines)
    assert all(lines[r][c] == wall for r in range(0, width, 2)
               for c in range(0, width, 2))
    assert all(lines[2 * r + 1][2 * c + 1] == passage
               for r in range(size) for c in range(size))
    assert sum(line.count(wall) for line in lines) * 100 >= 15 * width * width

    seen, pending = {(0, 0)}, [(0, 0)]
    while pending:
        row, col = pending.pop()
        for dr, dc in [(-1, 0), (0, 1), (1, 0), (0, -1)]:
            nr, nc = row + dr, col + dc
            if (0 <= nr < size and 0 <= nc < size and
                    lines[2 * row + 1 + dr][2 * col + 1 + dc] == passage and
                    (nr, nc) not in seen):
                seen.add((nr, nc))
                pending.append((nr, nc))
    assert len(seen) == size * size, "Some rooms are disconnected"

    edges = sum(lines[2 * r + 1][2 * c + 2] == passage
                for r in range(size) for c in range(size - 1))
    edges += sum(lines[2 * r + 2][2 * c + 1] == passage
                 for r in range(size - 1) for c in range(size))
    assert edges == size * size - 1, "The maze has cycles"
    possible = 2 * size * (size - 1)
    assert (possible - edges) * 100 >= 15 * possible
    checks += 1
    return process.stdout


check("maze", [], 6)
for size in [1, 2, 3, 6, 15, 40]:
    check("maze-size", [size], size)
    for symbols in [".#", " @", "# ", "ab", "[]", "#.", "'#"]:
        check("maze-symbols", [symbols, size], size, symbols)
    for seed in [0, 1, 42, 123456, 4294967295]:
        first = check("maze-seed", [seed, ".#", size], size)
        second = check("maze-seed", [seed, ".#", size], size)
        assert first == second, "The same seed produced different mazes"

mazes = {check("maze-seed", [seed, ".#", 6], 6) for seed in range(10)}
assert len(mazes) > 1, "The seed does not affect the maze"

invalid = [("maze", [6]), ("maze-size", []), ("maze-symbols", [".#"]),
           ("maze-seed", [42, ".#"]), ("maze-seed", [42, ".#", 6, 7])]
for size in ["", "0", "-1", "1001", "2x", "999999999999999999999999"]:
    invalid += [("maze-size", [size]), ("maze-symbols", [".#", size]),
                ("maze-seed", [42, ".#", size])]
for symbols in ["", ".", "...", "##", "\n#", "\x7f#", "é#"]:
    invalid += [("maze-symbols", [symbols, 6]), ("maze-seed", [42, symbols, 6])]
for seed in ["", "-1", "+1", "x", "1x", "4294967296", "999999999999999999999999"]:
    invalid.append(("maze-seed", [seed, ".#", 6]))
for name, args in invalid:
    process = run(name, args)
    assert process.returncode != 0 and not process.stdout and process.stderr, (name, args)
    checks += 1

print(f"{checks} checks: dimensions, walls, connectivity, unique paths, seeds and errors: OK")
