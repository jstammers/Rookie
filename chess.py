# -*- coding: utf-8 -*-
from __future__ import unicode_literals


class PieceDict:
    '''
    A container for useful static dictionaries, such as the starting position for each piece, their relative value and movement/capture possibilities 
    '''
    white_rep = {"K": "♚", "Q": "♛", "R": "♜", "B": "♝", "N": "♞", "p": "♟"}
    black_rep = {"K": "♔", "Q": "♕", "R": "♖", "B": "♗", "N": "♘", "p": "♙"}


class Piece:

    '''
    Defines a particular chess piece. Each piece is initialised using the shorthand name, but is represented using the unicode symbol.

    Init:
    piece: the shorthand name of the piece - pawns are denoted by their file letter
    col: defines the colour of the piece
    pos: the current position. If blank, it uses the starting position
    Contains:
    value: the relative value of the piece
    current_pos: the current position of the piece in the form (row_index, col_index)
    '''

    def init(self, piece, col, pos=()):
        self.piece = piece
        self.colour = col
        if not pos:
            self.current_pos


class Board:

    '''
    Defines the chessboard as a nested list. The first element indicates the row and the second indicates the column. 
    '''

    def __init__(self):
        self.grid = [[""] * 8 for _ in range(8)]

    def __str__(self):
        '''
        Prints the board as a grid
        '''
        grid_string = ''
        for row in self.grid:
            grid_string += '|'.join(row) + "\n"
        return grid_string.encode("utf-8")

    def populate(self):
        '''
        Populates the board with each piece in its starting position
        '''
        # Pawns first
        for i in range(8):
            self.grid[1][i] = PieceDict.white_rep["p"]
            self.grid[6][i] = PieceDict.black_rep["p"]

        # Now the rooks, knights and bishops
        for i, val in enumerate(["R", "N", "B"]):
            self.grid[0][i] = PieceDict.white_rep[val]
            self.grid[0][7 - i] = PieceDict.white_rep[val]
            self.grid[7][i] = PieceDict.black_rep[val]
            self.grid[7][7 - 1] = PieceDict.black_rep[val]

        # Finally, the kings and queens
        self.grid[0][3] = PieceDict.white_rep["Q"]
        self.grid[7][3] = PieceDict.black_rep["Q"]

        self.grid[0][4] = PieceDict.white_rep["K"]
        self.grid[7][4] = PieceDict.black_rep["K"]
