"""
Authors: Neil Schroeder
Description:
    Generates a password of varying strength based on user input.

"""

import random

_RAND_LOWER_CHARS_ = list('abcdefghijklmnopqrstuvwxyz')
_RAND_UPPER_CHARS_ = list('ABCDEFGHIJKLMNOPQRSTUVWXYZ')
_RAND_NUM_CHARS_ = list('1234567890')
_RAND_SPEC_CHARS_ = list('\\!@#$%.&*()?,<>[]{}/|`~"')


def get_L33T(word):
    """changes a word into l33t"""
    l33t_dict = {
            'a': '4',
            'e': '3',
            'i': '1',
            't': '7',
            'o': '0',
            's': '5',
            'z': '2',
            }

    for key in l33t_dict.keys():
        if key in word:
            word = word.replace(key, l33t_dict[key])

    return word


def generate_weak_password(num_words=2):
    """generates a random weak password by getting num_words random words"""
    with open('/usr/share/dict/words', 'r') as f:
        content = f.readlines()
    content = [x.strip() for x in content]

    for x in content:
        if "'" in x:
            content.remove(x)

    ret = ""
    while num_words > 0:
        if ret == "":
            ret += content[random.randint(0, len(content)-1)]
        else:
            ret += str(content[random.randint(0, len(content)-1)]).capitalize()
        num_words -= 1

    return ret


def generate_medium_password(num_words=2):
    """generates a random medium password.
       makes a weak password and makes it LEET"""

    password = generate_weak_password(num_words)
    return get_L33T(password)


def generate_strong_password(num_words=2, num_changes=5):
    """generates a random hard password.
       makes a weak medium password and changes random characters"""

    password = generate_medium_password(num_words)

    pwd = list(password)
    while num_changes > 0:
        pwd_index = random.randint(0, len(pwd)-1)
        char_index = random.randint(0, len(_RAND_SPEC_CHARS_)-1)
        pwd[pwd_index] = _RAND_SPEC_CHARS_[char_index]
        num_changes -= 1
    return "".join(pwd)


def main():
    """generates the password"""

    # prompt the user for the minimum length and strength of the password
    strengths = {
            "weak": generate_weak_password,
            "medium": generate_medium_password,
            "strong": generate_strong_password,
            }

    valid_strength = False
    valid_num_words = False
    valid_length = False
    while not valid_strength:
        print("choose a strength from the available list:")
        strength = input('{}\n'.format(strengths.keys()))
        if strength not in strengths.keys():
            valid_strength = False
        else:
            valid_strength = True

        if valid_strength:

            while not valid_num_words:
                msg = "choose the num. of words in the pwd (default is 2)\n"
                num_words = int(input(msg))
                if num_words < 1:
                    valid_num_words = False
                    print("invalid number of words")
                valid_num_words = True
                if strength != 'strong':
                    print("your random password is:")
                    print(strengths[strength](num_words))
                else:
                    while not valid_length:
                        msg = "how many special chars to use (default is 5)\n"
                        length = int(input(msg))
                        if length < 1:
                            valid_length = False
                            print("invalid number")
                        else:
                            valid_length = True
                            print("your random password is:")
                            print(strengths[strength](num_words, length))

    return


if __name__ == '__main__':
    main()
