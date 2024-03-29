
import argparse
import sys

from .application import Application

def parse_args(args):
    parser = argparse.ArgumentParser(
        prog = args[0],
        description = "Serve cube framework",
    )

    parser.add_argument(
        'directory',
        action = 'store',
        help = 'Released frameworks directory'
    )

    parser.add_argument(
        'address',
        nargs = '?',
        default = 'localhost:9000',
        help = 'Address to bind'
    )
    return parser, parser.parse_args(args[1:])

def main(argv):
    parser, args = parse_args(argv)
    app = Application(args.directory, args.address)
    try:
        app.run()
    except KeyboardInterrupt:
        pass

if __name__ == '__main__':
    main(sys.argv)
