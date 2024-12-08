import sys

with open(sys.argv[1], 'r') as f:
    lines = "".join(f.readlines())

lines = lines.replace('def getBarcodeLines(self) -> set:', 'def getBarcodeLines(self) -> set[Barline]:')

with open(sys.argv[1], 'w') as f:
    f.writelines(lines)
