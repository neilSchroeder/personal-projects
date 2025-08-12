def add(a, b):
    if a and b:
        return a+b
    return None


def subtract(a, b):
    if a and b:
        return a-b
    return None


def divide(a, b):
    if b != 0:
        return a/b
    return None


def times(a, b):
    if a and b:
        return a*b
    return None


ops = [add, subtract, times, divide]
op_str = ["+", "-", "*", "/"]


class equation:

    def __init__(self, nums, target):
        self.nums = [x for x in nums]
        self.target = target
        self.operations = []
        self.op_indices = []
        self.equation_string = ""

    def solve(self):
        if len(self.nums) < 2:
            return False

        global op, op_str

        for i in range(len(self.nums)-1):
            possible_ops = [
                self.nums[:i] + [op(self.nums[i], self.nums[i+1])]
                + self.nums[i+2::] if len(self.nums) > 2
                else [op(self.nums[i], self.nums[i+1])] for op in ops]

            if [self.target] in possible_ops:
                self.operations.append(
                    op_str[possible_ops.index([self.target])]
                )
                self.op_indices.append((i, i+1))
                return True

            for p in possible_ops:
                if len(p) < 2:
                    continue
                if None in p:
                    continue
                if any([round(x, 2) != x for x in p]):
                    continue
                possible_eqn = equation(p, self.target)
                if possible_eqn.solve():
                    self.operations = (
                        [op_str[possible_ops.index(p)]]
                        + possible_eqn.operations)
                    self.op_indices = [(i, i+1)] + possible_eqn.op_indices
                    return True

        return False

    def build_equation_string(self):
        for i in range(len(self.operations)):
            self.equation_string = f'({self.nums[self.op_indices[i][0]]}{self.operations[i]}{self.nums[self.op_indices[i][1]]})'
            self.nums = (
                self.nums[:self.op_indices[i][0]]
                + [self.equation_string]
                + self.nums[self.op_indices[i][1]+1::]
            )
