import sys
import struct

sys.stdout.buffer.write(bytes(sys.stdin.readline(),"utf-8"))

for line in sys.stdin:
    line=line.rstrip("\n")
    vals=line.split(" ")
    sys.stdout.buffer.write(bytes(vals[0],"utf-8"))
    sys.stdout.buffer.write(bytes(" ","utf-8"))
    for w in vals[1:]:
        sys.stdout.buffer.write(struct.pack("@f",float(w)))
    sys.stdout.buffer.write(bytes("\n","utf-8"))
    
    
