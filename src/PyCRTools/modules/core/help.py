"""Help functions.
"""

import re

from config import *


def all():
    pass

all.__doc__ = ""
#!!!!HF-FIX!!!!!
# all.__doc__=pycralldocstring


def hhelpstring(func):
    """
    Returns the doc string of a function. Check if the docstring
    contains informations from the CRTOOLS. If so, return only that
    part.
    """
    doc = func.__doc__
    if doc == None:
        return ""
    match = re.search("^PYCRTOOLS .*", doc, re.MULTILINE)
    if (match):
        predoc = doc[match.start():match.end()]
        doc = re.sub("PYCRTOOLS .*", "", doc[match.end():])
        doc = predoc + doc
    return doc

#    Included to overwrite the pydoc help function.


def help():
    """
    This is the basic help function for the PyCRTools
    environment. This message is generated by help() or help(help).

    To get information on a specific function or method type
    help(func).

    For a listing of all available functions type help(all).

    Look at the file
    $LOFARSOFT/src/CR-Tools/implement/Pypeline/tutorial.py
    to get a more basic introduction to the pycrtools.

    With "help(func,brief=False)" or "help(func,False)" you get a more
    extensive listing which includeds references to the uncerlying c++
    code (if the function was wrapped from c++).
    """
    help(help)


def help(func=help, brief=True):
    """
    This is the basic help function for the PyCRTools
    environment. This message is generated by help() or help(help).

    To get information on a specific function or method type
    help(func).

    For a listing of all available functions type help(all).

    Look at the file
    $LOFARSOFT/src/CR-Tools/implement/Pypeline/tutorial.py
    to get a more basic introduction to the pycrtools.

    With "help(func,brief=False)" or "help(func,False)" you get a more
    extensive listing which includeds references to the underlying c++
    code (if the function was wrapped from c++).
    """
    if func == None:
        func = help
    if (hasattr(func, "__class__")):
        print "Class: ", func.__class__
    if brief:
        print hhelpstring(func)
    else:
        print pyhelp(func)
    methods = dir(func)
    methods2 = []
    for m in methods:
        if not ((m.find("_") == 0) | (m.find("func_") == 0) | (m.find("im_") == 0)):
            methods2.append(m)
    if len(methods2) > 0:
        print "\nAvailable methods: ",
        for m in methods2[0:-1]:
            print m + ", ",
        print methods2[-1]
        if (hasattr(func, "__name__")):
            print "\nType help(" + func.__name__ + ".METHOD) to get more info on the individual methods."
        else:
            print "\nType help(func.METHOD) to get more info on the individual methods."
