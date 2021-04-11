import sys

with open(sys.argv[2],'wb') as f:
  f.write(b'"')
  for b in open(sys.argv[1], 'rb').read():
    if b == 10:
      f.write(b'\\n"\n"')
    elif b == 13:
      pass
    elif b >= 32 and b <= 126:
      f.write(bytes([ b ]))
    else:
      f.write(b'\\x%02X,' % b)
  f.write(b'"')
