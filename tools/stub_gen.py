import os
import sys

class Param:
    def __init__(self):
        self.type = None
        self.name = None
    def __repr__(self):
        # return "%s %s" % (self.type, self.name)
        return "%s" % (self.type)


class Function:
    def __init__(self):
        self.return_type = None
        self.name = None
        self.params = []
    def __repr__(self):
        return "%-20s %-50s(%s) {}" % (self.return_type, self.name, ", ".join([str(p) for p in self.params]))

if __name__ == "__main__":

    os.chdir(os.path.dirname(sys.argv[0]))
    functions = []

    with open("../the_debuginator.h") as file:
        state = "finding_start"
        curr_func = None
        for line in file:
            line = line.strip()
            if state == "finding_start":
                if "API START" in line:
                    state = "parsing_api"
            elif state == "parsing_api":
                if "API END" in line:
                    break
                if line == "":
                    continue
                if line[:2] == "//":
                    continue

                if curr_func == None:
                    curr_func = Function()
                    firstsplit = line.find(" ")
                    curr_func.return_type = line[:firstsplit]
                    parambegin = line.find("(", firstsplit)
                    curr_func.name = line[firstsplit + 1:parambegin]
                    parambegin += 1

                while True:
                    type_end = line.find(" ", parambegin)
                    p = Param()
                    p.type = line[parambegin:type_end]
                    name_begin = type_end + 1
                    name_end = line.find(",", name_begin)
                    if name_end == -1:
                        name_end = line.find(")", name_begin)
                    p.name = line[name_begin:name_end]
                    curr_func.params.append(p)

                    if name_end == -1:
                        break

                    if line[name_end:] == ",":
                        break

                    parambegin = name_end + 2

                    if line[name_end:] == ");":
                        print(curr_func)
                        functions.append(curr_func)
                        curr_func = None
                        break

    # for f in functions:
    #     print(f.name)

