#!/usr/bin/env python3
"""Fetch pinned build inputs and copy the version-matched owned game manual."""
import argparse
import hashlib
from pathlib import Path
import subprocess
import urllib.request
import zipfile

ROOT = Path(__file__).resolve().parents[1]
PIN = 'c105ef104137954887273c80e79fdb05841f681c'
MANUAL_SHA = 'ad2724257c2b4a646eef875bc0233e70aa042dc639092784cda6923ff9b8df0f'
FONT_URL = ('https://github.com/TakWolf/fusion-pixel-font/releases/download/2026.09.25/'
            'fusion-pixel-font-12px-monospaced-bdf-v2026.09.25.zip')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--game-dir', required=True, type=Path)
    args = parser.parse_args()
    manual = (args.game_dir / 'manual.txt').read_bytes()
    if hashlib.sha256(manual).hexdigest() != MANUAL_SHA:
        raise SystemExit('Expected the original Beta 17.1 / 260906a manual.txt; restore it before setup')
    upstream = ROOT / 'upstream/KR-AOS-Cogmind'
    upstream.parent.mkdir(parents=True, exist_ok=True)
    if not upstream.exists():
        subprocess.run(['git', 'clone', 'https://github.com/magicianaos/KR-AOS-Cogmind',
                        str(upstream)], check=True)
        subprocess.run(['git', '-C', str(upstream), 'checkout', '--detach', PIN], check=True)
    actual = subprocess.check_output(['git', '-C', str(upstream), 'rev-parse', 'HEAD'], text=True).strip()
    if actual != PIN:
        raise SystemExit('Existing upstream checkout differs from the pinned commit; no reset performed')
    folder = ROOT / 'upstream/fonts'
    folder.mkdir(parents=True, exist_ok=True)
    archive = folder / 'fusion-pixel-12-bdf.zip'
    if not archive.exists():
        temporary = archive.with_suffix('.download')
        urllib.request.urlretrieve(FONT_URL, temporary)
        temporary.replace(archive)
    name = 'fusion-pixel-12px-monospaced-zh_hans.bdf'
    with zipfile.ZipFile(archive) as bundle:
        data = bundle.read(name)
    expected = 'bb2cdfa029674d83ad9fc6b01bba11752f61779cae1219a722c7ccee96b2d0ae'
    if hashlib.sha256(data).hexdigest() != expected:
        raise SystemExit('Font BDF differs from the evaluated version')
    (folder / name).write_bytes(data)
    destination = ROOT / 'original/bazzite/manual.txt'
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_bytes(manual)
    (ROOT / 'analysis').mkdir(exist_ok=True)
    print('Pinned upstream, pixel font, and original manual are ready')


if __name__ == '__main__':
    main()
