#!/usr/bin/env python3
"""Model the verified manual chapter/row grammar before native word wrapping.

This is an offline input model, not an emulator or proof of runtime behavior.
The byte filter and chapter/subheading branches come from 0x790f20; the
68-column wrapping phase in 0x791c70 is deliberately not approximated here.
"""
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def accepted(byte):
    return 0x20 <= byte <= 0x7e and byte != 0x60


def sanitize(raw):
    # 0x7911e2..0x7912d8 removes an invalid byte, then spaces at that position.
    data = bytearray(raw.rstrip(b"\r\n"))
    index = 0
    while index < len(data):
        if accepted(data[index]):
            index += 1
            continue
        del data[index]
        while index < len(data) and data[index] == 0x20:
            del data[index]
    return data.decode("ascii")


def boundary(line):
    return line.startswith("--")


def subheading(line):
    return len(line) >= 2 and line[0] == " " and line[1].isascii() and line[1].isalnum()


def parse(raw):
    source = raw.splitlines()
    chapters = []
    ignored = []
    current = None
    index = 0
    while index < len(source):
        line = sanitize(source[index])
        number = index + 1
        index += 1
        if current is None and not boundary(line):
            ignored.append(number)
            continue
        if boundary(line):
            if index >= len(source):
                raise ValueError(f"Chapter marker at line {number} has no title")
            title = source[index].decode("ascii").rstrip("\r\n")
            if not title.startswith(" "):
                raise ValueError(f"Expected native title prefix at line {index + 1}")
            current = {"title": title[1:], "title_line": index + 1,
                       "boundary_line": number, "rows": [], "subheading_rows": []}
            chapters.append(current)
            index += 2  # Native branch consumes the title and following rule.
            continue
        if subheading(line):
            current["rows"].append({"text": "", "source_line": None, "kind": "subheading_gap"})
            current["subheading_rows"].append(len(current["rows"]))
            current["rows"].append({"text": line[1:], "source_line": number, "kind": "subheading"})
            index += 1  # Native branch consumes the following underline.
        else:
            current["rows"].append({"text": line, "source_line": number, "kind": "body"})
    return chapters, ignored


def main():
    path = ROOT / "original/bazzite/manual.txt"
    raw = path.read_bytes()
    chapters, ignored = parse(raw)
    report = {"source_sha256": hashlib.sha256(raw).hexdigest(),
              "evidence": "analysis/native-manual-source-path.json",
              "scope": "Offline model of source parsing, before native 68-column word wrapping; not runtime verification.",
              "chapter_count": len(chapters), "ignored_preamble_lines": ignored,
              "chapters": chapters,
              "limitations": ["Native stream/allocator behavior is not emulated.",
                              "Word wrapping and later UI navigation have not been modeled or verified."]}
    output = ROOT / "analysis/native-manual-input-model.json"
    output.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n")
    print(json.dumps({"chapters": len(chapters),
                      "subheadings": sum(len(c["subheading_rows"]) for c in chapters),
                      "input_rows": sum(len(c["rows"]) for c in chapters)}))


if __name__ == "__main__":
    main()
