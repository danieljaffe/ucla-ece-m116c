lines = 200 # change line of test here

from random import randint, seed

test_case = "0, 0, 0, 0"
base = randint(0, 1500)

for i in range(lines - 1):
    seed()
    a = randint(0, 1)
    cur_adr = randint(base, base + lines // 3)

    if a == 0:
        test_case += "\n0, 1, " + str(cur_adr) + ", 0"
    elif a == 1:
        test_case += "\n1, 0, " + str(cur_adr) + ", 0"

print(test_case, end = '')