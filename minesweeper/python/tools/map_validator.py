

def validate_map(g, s):
    # validates that s and g are equivalent
    return all(i == j for i,j in zip(g,s))