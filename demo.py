import sys
import os
import time

def parse_maps() -> tuple[list[str], list[str]]:
    file_path = "/proc/" + sys.argv[-1] + "/maps"
    addrs = []
    with open(file_path) as f:
        for line in f:
            if "heap" in line or "stack" in line:
                addrs.append(line)
    # 55d51dad0000-55d51daf1000 rw-p 00000000 00:00 0                          [heap]
    # 7ffc5bc66000-7ffc5bc87000 rw-p 00000000 00:00 0                          [stack]
    heap_addr = addrs[0].split(" ")[0].split("-")
    stack_addr = addrs[1].split(" ")[0].split("-")
    return heap_addr, stack_addr

def parse_mem(addr: list[str, str]) -> bytes:
    file_path = "/proc/" + sys.argv[-1] + "/mem"
    start = int(addr[0], 16)
    end   = int(addr[1], 16)
    with open(file_path, "rb") as f:
        f.seek(start)
        buf = f.read(end - start)
    return buf

heap_addr, stack_addr = parse_maps()
parse_target = stack_addr        # <-
cols, rows = os.get_terminal_size()
print("\033[?25l\033[H\033[2J", end="")
print("demo flush in 0.5s\n")
for i in range(1, rows-2):
    for j in range(1, cols):
        print("|", end="")
    print()
buf_size = int(parse_target[1], 16) - int(parse_target[0], 16)
row_range = buf_size // (rows - 2)
uni_range = row_range // cols
old_buf = parse_mem(parse_target)
try:
    while True:
        new_buf = parse_mem(parse_target)
        for i in range(0, buf_size, uni_range):
            if new_buf[i:i+uni_range] != old_buf[i:i+uni_range]:
                print(f"\033[{(i // row_range) + 2};{((i % row_range) // uni_range) + 1}H\033[31mX", end="", flush=True)
            else:
                print(f"\033[{(i // row_range) + 2};{((i % row_range) // uni_range) + 1}H\033[37m|", end="", flush=True)
        old_buf = new_buf
        time.sleep(0.5)
except KeyboardInterrupt:
    print("\033[H\033[2J\033[?25h\033[0m", end="")
    exit()
