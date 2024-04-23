def factorial(n): # function that calculates the factorial of any integer n
    n1 = int(n);
    if(n == 0):
        return 1
    for i in range(1, n):
        n1 = n1 * (i)
    return n1

def approxE(nOfApprox): # Approximate e using the 1/n! sum
    e = 0
    for i in range(0, nOfApprox):
        e += 1/(factorial(i))
    return e

print("Number of times to approximate e?\n")
nOfApprox = int(input())
print("e is approximately equal to: " + str(approxE(nOfApprox)))

