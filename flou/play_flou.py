from flou_class import flou


def play_flou(game_map):
    """ uses the game_map to play flou """

    game = flou(game_map)
    if game.play():
        game.draw(game.game_map)
        return game.solution

    return False


def main():
    """ main function for playing flou """
    map = '''+------+\n|..B..B|\n|.B....|\n|....B.|\n|......|\n|..B...|\n|.....B|\n+------+'''
    print(play_flou(map))


if __name__ == '__main__':
    main()
