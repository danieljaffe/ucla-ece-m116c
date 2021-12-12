import sys

test_case = open(sys.argv[1], 'r')
lines = test_case.read().split("\n")
test_case.close()

#initialize counter and tag dicts
line_count, read_count, write_count = 0, 0, 0
dm_tags, fa_tags, sa_tags = [], [], []
dm_miss, fa_miss, sa_miss = 0, 0, 0
dm_cycles, fa_cycles, sa_cycles = 0, 0, 0

for i in range(16): #initialize the dm_tags and fa_tags
    dm_tags += [-1]
    fa_tags += [-1]

for i in range(4):
    sub_list = []
    for j in range(4):
        sub_list += [-1]
    sa_tags += [sub_list]

for line in lines:
    #get information from the file
    line_count += 1
    signals = line.split(",")
    mem_r, mem_w, cur_adr, cur_data = signals[0], signals[1], int(signals[2]), int(signals[3])

    if mem_r == "1" and mem_w == "1":   # edge case
        print("both read and write signals are on at line:", line_count)
        exit()
    elif mem_r == "1":  # lw instruction
        read_count += 1

        if (cur_adr & 0xfffffff0) != dm_tags[cur_adr & 0xf]:    # miss for dm
            dm_tags[cur_adr & 0xf] = cur_adr & 0xfffffff0
            dm_miss, dm_cycles = dm_miss + 1, dm_cycles + 4

        if cur_adr in fa_tags:  # hit for fa
            fa_tags.remove(cur_adr)
            fa_tags.insert(0, cur_adr)
        elif cur_adr not in fa_tags:    # miss for fa
            fa_tags.pop()
            fa_tags.insert(0, cur_adr)
            fa_miss, fa_cycles = fa_miss + 1, fa_cycles + 4

        if (cur_adr & 0xfffffffc) in sa_tags[cur_adr & 0x3]:    # hit for sa
            sa_tags[cur_adr & 0x3].remove(cur_adr & 0xfffffffc)
            sa_tags[cur_adr & 0x3].insert(0, cur_adr & 0xfffffffc)
        elif (cur_adr & 0xfffffffc) not in sa_tags[cur_adr & 0x3]:  # miss for sa
            sa_tags[cur_adr & 0x3].pop()
            sa_tags[cur_adr & 0x3].insert(0, cur_adr & 0xfffffffc)
            sa_miss, sa_cycles = sa_miss + 1, sa_cycles + 4

    elif mem_w == " 1":  #sw instruction
        write_count += 1

    dm_cycles, fa_cycles, sa_cycles = dm_cycles + 1, fa_cycles + 1, sa_cycles + 1
    
# print the read and write count
print("read count: ", read_count)
print("write count: ", write_count)

#print the direct map result
print(("*" * 8) + "DM" + ("*" * 8))
print("miss: ", dm_miss)
print("miss rate: ", dm_miss / read_count)
print("clock: ", dm_cycles)

#print the fully associativity result
print(("*" * 8) + "FA" + ("*" * 8))
print("miss: ", fa_miss)
print("miss rate: ", fa_miss / read_count)
print("clock: ", fa_cycles)

#print the set associativity result
print(("*" * 8) + "SA" + ("*" * 8))
print("miss: ", sa_miss)
print("miss rate: ", sa_miss / read_count)
print("clock: ", sa_cycles)