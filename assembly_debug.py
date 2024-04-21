import re
import sys

lines = []
labels = {}
next_dest = ""

vars = {}
stack = []
register = {
    "rsp": 0,
    "rbp": 0,
    "rax": 0,
    "rdi": 0,
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
            next_dest = label



 

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

def print_current():
    print("register: ", register)
    print("stack: ", stack)
    print("vars: ", vars)    

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
        i = 1
        while line:
            a_line = line.strip()
            lines.append(a_line)
            if (a_line.startswith(".")):
                labels[a_line] = i
            line = file.readline()
            i += 1

    index = 0
    while index < len(lines):
        executeAssembly(lines[i].strip())
        if (index + 1 == target_line):
            print_current()
            return
        index += 1
        if (next_dest == ""):
            continue
        else:
            index = labels[next_dest]
            next_dest = ""
    
    print_current()

if __name__ == "__main__":
    main()
