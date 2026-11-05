import re
import sys
import subprocess
from pathlib import Path

HERE = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else Path(__file__).resolve().parent
SUFFIX = ".exe" if sys.platform == "win32" else ""
def reader(data):
    p = subprocess.run([str(HERE / ('test_saisie' + SUFFIX))], input=data, capture_output=True, timeout=10)
    assert p.returncode == 0, p.stderr
    return p.stdout.decode().splitlines()

assert reader(b'') == []
assert reader(b'1 2 3') == ['1 1 0']
assert reader(b'1 2 3\r\n Q \n\n') == ['1 1 0', '1 0 1', '1 0 0']
assert reader(b' ' * 122 + b'1 2 3\n') == ['1 1 0']  # 127 caracteres
assert reader(b' ' * 123 + b'1 2 3\n1 2 3\n') == ['-1 0 0', '1 1 0']
assert reader(b'9' * 200) == ['-1 0 0']
assert reader(b'q\x00junk\n1 2 3\n') == ['-1 0 0', '1 1 0']

def until(p, marker):
    text = ''
    while not text.endswith(marker):
        c = p.stdout.read(1)
        if not c:
            raise AssertionError('Fin inattendue : ' + text)
        text += c
    return text

def send(p, text):
    p.stdin.write(text + '\n')
    p.stdin.flush()

def start(level):
    p = subprocess.Popen([str(HERE / ('sudoku' + SUFFIX)), '--console'], stdin=subprocess.PIPE,
                         stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    until(p, 'difficile : ')
    send(p, str(level))
    out = until(p, 'ou q : ')
    rows = re.findall(r'^[1-9] \| (.+) \|$', out, re.M)
    assert len(rows) == 9
    grid = [[int(v) if v != '.' else 0 for v in row.replace('|', '').split()] for row in rows]
    return p, grid

def solve(g):
    for i in range(9):
        for j in range(9):
            if g[i][j] == 0:
                blocked = set(g[i]) | {g[r][j] for r in range(9)}
                blocked |= {g[r][c] for r in range(i//3*3, i//3*3+3)
                            for c in range(j//3*3, j//3*3+3)}
                for v in range(1, 10):
                    if v not in blocked:
                        g[i][j] = v
                        if solve(g):
                            return True
                g[i][j] = 0
                return False
    return True

for level, limit in [(1, 9), (2, 5), (3, 3)]:
    p, initial = start(level)
    solution = [r[:] for r in initial]
    assert solve(solution)
    for invalid in ['1', '1 2', '1 2 3 4', '123', '0 2 3', '01 2 3', '+1 2 3', '9'*200]:
        send(p, invalid)
        out = until(p, 'ou q : ')
        assert 'Entrer trois chiffres' in out and f'Erreurs : 0/{limit}' in out
    i, j = next((i,j) for i in range(9) for j in range(9) if initial[i][j])
    send(p, f'{i+1} {j+1} {initial[i][j]}')
    out = until(p, 'ou q : ')
    assert 'protegee' in out and f'Erreurs : 0/{limit}' in out
    i, j = next((i,j) for i in range(9) for j in range(9) if not initial[i][j])
    send(p, f'{i+1} {j+1} {solution[i][j] % 9 + 1}')
    assert f'Erreurs : 1/{limit}' in until(p, 'ou q : ')
    empty = [(i,j) for i in range(9) for j in range(9) if not initial[i][j]]
    for i,j in empty[:-1]:
        send(p, f'{i+1} {j+1} {solution[i][j]}')
        assert f'Erreurs : 1/{limit}' in until(p, 'ou q : ')
    i,j = empty[-1]
    out, err = p.communicate(f'{i+1} {j+1} {solution[i][j]}\n', timeout=10)
    assert p.returncode == 0 and 'Victoire !' in out, (out,err)

p, initial = start(3)
solution = [r[:] for r in initial]
assert solve(solution)
i,j = next((i,j) for i in range(9) for j in range(9) if not initial[i][j])
bad = f'{i+1} {j+1} {solution[i][j] % 9 + 1}\n'
out, err = p.communicate(bad * 3, timeout=10)
assert p.returncode == 0 and "Defaite : limite d'erreurs atteinte." in out

for data, expected in [('1\nq\nn\nQ\nO\n', 'Partie abandonnee.'),
                       ('2\n', 'Fin de saisie.'), ('3\nq\n', 'Confirmer'),
                       ('abc\n4\n1\nq\no', 'Partie abandonnee.')]:
    p = subprocess.run([str(HERE / ('sudoku' + SUFFIX)), '--console'], input=data, text=True,
                       capture_output=True, timeout=10)
    assert p.returncode == 0 and expected in p.stdout

print('OK : saisie, EOF, limites du tampon, commandes, protection, victoire aux 3 niveaux, defaite et abandon.')
