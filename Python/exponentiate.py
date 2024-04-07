# Code that returns any int number to the power of another int
def exp(n, exp):
    n1 = n
    if(exp == 0):
        result = 1
    elif(exp == 1):
        result = n
    for i in range(exp-1):
        result = n * n1
        n1 = result
    return result

txt = "{} to the {} power is: {}"

print("Enter an int base: ")
n = int(input())
print("Enter an exponent: ")
pwr = int(input())
print(txt.format(n, pwr, exp(n, pwr)))