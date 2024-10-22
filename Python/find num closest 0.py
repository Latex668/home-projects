# find the closest number to 0 in any array
from random import randrange
nums = [randrange(-10,10),randrange(-10,10),randrange(-10,10),randrange(-10,10),randrange(-10,10),randrange(-10,10),randrange(-10,10)]
print(nums)
def findClosestNum(nums):
  closest = nums[0]
  for i in nums:
    if(abs(i) < abs(closest)):
      closest = i
  if(closest<0 and abs(closest) in nums):
    return abs(closest)
  else:
    return closest

print(findClosestNum(nums))