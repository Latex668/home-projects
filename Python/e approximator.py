def factorial(n):
    n1 = n;
    if(n == 0):
        return 1
    for i in range(1, n):
        n1 = n1 * (i)
    return n1

def approxE(nOfApprox):
    e = 0
    for i in range(0, nOfApprox):
        e += 1/(factorial(i))
    return e
print("Number of times to approximate e?\n")
nOfApprox = int(input())
print("e is approximately equal to: " + str(approxE(nOfApprox)))

