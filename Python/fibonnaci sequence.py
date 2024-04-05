# code to produce any n fibonnaci sequence

def fibonnaci(seriesnum):
    n0 = 0
    n1 = 1
    sn = 0
    for i in range(seriesnum):
        sn+=1 
        result = n0 + n1
        n0 = n1
        n1 = result
        print(format(str(sn) + " number " + str(result)))
    
print("Number of passes?")
seriesnum = int(input())
fibonnaci(seriesnum)
