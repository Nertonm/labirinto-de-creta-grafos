#!/usr/bin/env python3
import json
import os
import subprocess
import sys

SCENARIOS = [
    'data/fuga',
    'data/escape',
    'data/perseguicao_imediata',
    'data/first_test',
    'data/beco_sem_saida',
    'data/morte_fome',
    'data/teste_distante',
]

BIN = os.path.join('build', 'bin', 'simulador')


def run_scenario(path: str):
    cmd = [BIN, path, '--json-only']
    res = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    if res.returncode != 0:
        print(f"Scenario {path} failed to run. RC={res.returncode}. stderr=\n{res.stderr}")
        return None
    try:
        data = json.loads(res.stdout)
        return data
    except json.JSONDecodeError:
        print(f"Scenario {path} produced non-JSON output:\n{res.stdout}\n--- stderr ---\n{res.stderr}")
        return None


def main():
    if not os.path.exists(BIN):
        print(f"Binary not found: {BIN}")
        sys.exit(1)

    failures = 0
    for s in SCENARIOS:
        if not os.path.exists(s):
            print(f"Skipping missing scenario: {s}")
            continue
        data = run_scenario(s)
        if data is None:
            failures += 1
            continue
        # Basic shape checks
        for k in ['sobreviveu', 'tempo', 'tempoReal', 'kits', 'posP', 'posM', 'minotauroVivo', 'encontro']:
            if k not in data:
                print(f"Scenario {s}: missing key '{k}' in JSON")
                failures += 1
        # Consistency: tempo vs tempoReal rounding down
        if 'tempo' in data and 'tempoReal' in data:
            if data['tempo'] != int(data['tempoReal']):
                print(f"Scenario {s}: tempo != floor(tempoReal) ({data['tempo']} vs {data['tempoReal']})")
                # Not fatal, just warn
        # encontro object shape
        enc = data.get('encontro', {})
        for k in ['ok', 'tipo', 't']:
            if k not in enc:
                print(f"Scenario {s}: encontro missing '{k}'")
                failures += 1

    if failures:
        print(f"CI validation: {failures} issues found")
        sys.exit(1)
    print("CI validation: OK")


if __name__ == '__main__':
    main()
