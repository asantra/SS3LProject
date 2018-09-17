import os, sys

if len(sys.argv) < 2: 
    print "no search expression found"; sys.exit(1)

expr = sys.argv[1]
if not len(expr): 
    print "no search expression found"; sys.exit(1)

expr = "rucio list-dids {0} | sort | grep user &> list.txt".format(expr) 
os.system(expr)

txtFile = open("list.txt","rw")
endFile = open("list.sh", "wr")

for line in txtFile:
    dataset = line.split(" ")[1]
    newLine = "rucio download {0}".format(dataset); print newLine
    newLine = newLine+"\n"
    endFile.write(newLine)

os.remove("list.txt")
print "--> Created {0}".format(endFile.name)
endFile.close
