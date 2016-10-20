import sys

if not sys.argv[1] or not sys.argv[2]:
	print "Need a file to read and write"
	sys.exit(1);

with open(sys.argv[1], 'r') as fin:
	with open(sys.argv[2], 'w') as fout:
		for line in fin:
			res = [c for c in line.upper() if ord(c) in range(ord('A'), ord('Z') + 1)]
			fout.write(''.join(res))
		fout.close()
	fin.close()
