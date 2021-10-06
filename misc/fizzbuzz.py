"""
Author: Neil Schroeder
Description:
    This program will propt the user to input an integer greater than 0
    with which the program will print fizzbuzz between 1 and the entered
    integer.
"""


def valid_fizzbuzz(integer):
    """checks to see if the integer is a valid fizzbuzz number"""
    if not integer > 0:
        print("not an acceptable integer, please try again")
        return False
    else:
        return True


def get_fizzbuzz(integer):
    """determines the fizzbuzz of integer"""
    ret = ""
    if integer % 3 == 0:
        ret = ret + "fizz"
    if integer % 5 == 0:
        ret = ret + "buzz"

    return ret if ret != "" else None


def get_fizzbuzz_list(integer):
    """gets the list of fizzbuzz for 1 to n"""
    return [str(i) if get_fizzbuzz(i) is None
            else get_fizzbuzz(i) for i in range(1, integer+1)]


def main():
    """
    takes an integer from the user
    and prints fizzbuzz for integers
    between 1 and this number
    """

    valid_num = False
    # don't let the user enter an invalid entry
    while not valid_num:
        try:
            msg = "please enter a positive integer for fizzbuzz: "
            num_fizzbuzz = int(input(msg))
            valid_num = valid_fizzbuzz(num_fizzbuzz)
        # allow the user to interrupt safely if they get upset
        except KeyboardInterrupt:
            print()
            return
        except:
            print("invalid entry, please try again")

    for val in get_fizzbuzz_list(num_fizzbuzz):
        print(val)

    return


if __name__ == '__main__':
    main()
