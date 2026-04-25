A = 5
B = 7
C = 3
# Finding the greatest number
if A >= B and A >= C:
    greatest = A
elif B >= A and B >= C:
    greatest = B
else:
    greatest = C
# Finding the smallest number
if A <= B and A <= C:
    smallest = A
elif B <= A and B <= C:
    smallest = B
else:
    smallest = C

print("Greatest number:", greatest)
print("Smallest number:", smallest)
