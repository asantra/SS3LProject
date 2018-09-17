print "Sorting..."    
fin = open("log_all.txt","r")
fout = open("sort_log_all.txt","w")
evnum = []
for l in fin:
    evnum.append(int(l))
evnum.sort()
for e in evnum:
    fout.write(str(e))
    fout.write("\n")
print "Done!"
