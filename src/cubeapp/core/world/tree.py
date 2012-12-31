# -*- encoding: utf-8 -*-

class Child:

    tfl = top_front_left = 0
    tfr = top_front_right = 1
    tbl = top_back_left = 2
    tbr = top_back_right = 4
    bfl = bottom_front_left = 5
    bfr = bottom_front_right = 6
    bbl = bottom_back_left = 7
    bbr = bottom_back_right = 8

class TreeNode:

    def __init__(self, level):
        self.level = level
        self.childs = {
            Child.tfl: None,
            Child.tfr: None,
            Child.tbl: None,
            Child.tbr: None,
            Child.bfl: None,
            Child.bfr: None,
            Child.bbl: None,
            Child.bbr: None,
        }

class Tree:

    def __init__(self):
        self.root = TreeNode(64)

