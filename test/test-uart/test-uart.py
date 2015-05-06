#!/usr/bin/python

import os, sys

library_path = os.path.abspath('../../library')
sys.path.append(lib_path)

import Serial

def program():
    print "Hello world!"

def main():
    program()
    
if __name__ == "__main__":
    main()
