import re
import sys

lines = []
labels = {}
next_dest_global = []

vars = {}
stack = []
register = {
    "rsp": 0,
    "rbp": 0,
    "rax": -1,
    "rdi": -1,
}
flags = {
    "CF": 0,
}

def executeAssembly(text):
    if (text.startswith("mov")):
        execute_mov(text)
    elif (text.startswith("push")):
        push_value = text[5:]
        if (push_value.isdigit()):
            stack.append(int(push_value))
        else:
            stack.append(register[push_value])
    elif (text.startswith("pop")):
        pop_value = text[4:]
        register[pop_value] = stack.pop()
    elif (text.startswith("add")):
        register_name = text[4:7]
        add_value = text[9:]
        if (add_value.isdigit()):
            register[register_name] += int(add_value)
        else:
            register[register_name] += register[add_value]
    elif (text.startswith("sub")):
        register_name = text[4:7]
        sub_value = text[9:]
        if (sub_value.isdigit()):
            register[register_name] -= int(sub_value)
        else:
            register[register_name] -= register[sub_value]
    elif (text.startswith("cmp")):
        register_name = text[4:7]
        if (register[register_name] == 0):
            flags["CF"] = 1
        else:
            flags["CF"] = 0
    elif (text.startswith("je")):
        label = text[3:]
        if (flags["CF"]):
            next_dest_global.append(label)
    elif (text.startswith("jmp")):
        label = text[4:]
        next_dest_global.append(label)



 

def execute_mov(text):
    pattern = r'^mov (\[?\w+\]?), (\[?\w+\]?)$'
    
    match = re.match(pattern, text)
    if match:
        dest, src = match.groups()
        if '[' in dest and '[' in src:
            print("no op")
        elif '[' in dest:
            vars[register[dest[1:-1]]] = register[src]
        elif '[' in src:
            register[dest] = vars[register[src[1:-1]]]
        else:
            register[dest] = register[src]
    else:
        print("no op")

def print_current(index, text):
    print(index + 1, "行目 : ", text)
    print("  register: ", register)
    print("  stack: ", stack)
    print("  vars: ", vars)
    print("  flags: ", flags)

def main():
    if len(sys.argv) > 1:
        try:
            target_line = int(sys.argv[1])
        except ValueError:
            return
    else:
        target_line = 0

    with open('tmp.s', 'r', encoding='utf-8') as file:
        line = file.readline()
        i = 0
        while line:
            a_line = line.strip()
            lines.append(a_line)
            if (a_line.startswith(".")):
                labels[a_line[:-1]] = i
            line = file.readline()
            i += 1

    print(labels)
    index = 0
    while index < len(lines):
        input()
        text = lines[index].strip()
        executeAssembly(text)
        print_current(index, text)
        if (len(next_dest_global) == 0):
            index += 1
            continue
        else:
            next_dest = next_dest_global.pop()
            index = labels[next_dest]

    print("return: ", register["rax"])

if __name__ == "__main__":
    main()
