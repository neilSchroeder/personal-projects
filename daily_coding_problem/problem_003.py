
class Node:
    def __init__(self, val, left=None, right=None):
        self.val = val
        self.left = left
        self.right = right


    def deserialize(self, s):
        # turns a string into a tree
        
        return Node(s.split(' ')[0], 
                    self.deserialize())
            

        


def main():

    node = Node('root', 
                Node('left', Node('left.left')), 
                Node('right'))

    s = serialize(node)
    print(s)


def serialize(node):
    # turns the tree into a string
        
    s = f"{node.val} "

    if node.left:
        s = s + serialize(node.left)
    else:
        s = s + f"x "
       
    if node.right:
        s = s + serialize(node.right)
    else:
        s = s + f"x "
    return s


def deserialize(s):
    # turns a string into a tree

    s = s.split(" ")[::-1]

    node = None
    value = s.pop()
    
    node = Node(value)
    node.left = deserialize(' '.join())







if __name__ == '__main__':
    main()
