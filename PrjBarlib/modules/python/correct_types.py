import sys

with open(sys.argv[1], 'r') as f:
    lines = "".join(f.readlines())

lines = lines.replace('def getBarcodeLines(self) -> list:', 'def getBarcodeLines(self) -> list[Barline]:')

with open(sys.argv[1], 'w') as f:
    f.writelines(lines)
