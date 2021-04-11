import sys

with open(sys.argv[2],'wb') as f:
  for b in open(sys.argv[1], 'rb').read():
    f.write(b'0x%02X,' % b)
