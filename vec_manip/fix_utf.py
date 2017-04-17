import sys
import traceback

for line in sys.stdin.buffer:
    try:
        l=line.decode("utf-8")
        print(l,end="")
    except UnicodeDecodeError:
        spaceidx=line.index(" ".encode("utf-8"))
        for someidx in range(spaceidx-1,0,-1):
            try:
                print(repr(line[:someidx]),file=sys.stderr)
                w=line[:someidx].decode("utf-8")
                break
            except UnicodeDecodeError:
                traceback.print_exc()
                pass
        else:
            raise ValueError("No luck")
        print(w,line[spaceidx+1:].decode("utf-8"))
        
