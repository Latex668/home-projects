from prettytable import PrettyTable
table = PrettyTable()
table.field_names = ['A','B', 'C','F']

def incr(lst, inc):
    for i in range(0, len(lst)):
        lst[i] += inc

# print("Enter number of variables: ")
# nVar = int(input())
# nRows = 2**nVar

# print("Enter a function: ")
# F = input()
# j = [0,0,0]
# for i in range(nRows):
#     for x in range(0, (2**(nVar-1))):
#         print()
fuckinlist = [1, 1, 1, 1]
table.add_row([fuckinlist[0], fuckinlist[1], fuckinlist[2], fuckinlist[3]])
# incr(j, 1)
# print(j)
    
print(table)
