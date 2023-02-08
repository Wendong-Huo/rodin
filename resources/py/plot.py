"""
Description
-----------

Parses and plots the contents of a one column file. This is useful for
generating uniform plots of the files generated by some of the shape
optimization examples.

Usage
-----
python plot.py <filename>

"""

import numpy as np
from matplotlib import pyplot as plt
plt.style.use('grayscale')
import numpy as np
import pandas as pd
import argparse

if __name__ == '__main__':
    description = '''Parses and plots the contents of a one column file. This is
    useful for generating uniform plots of the files generated by some of the shape
    optimization examples.'''
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('filename', type=str,
                        help='Name of file to plot.')
    args = parser.parse_args()

    data = pd.read_csv(args.filename)
    data = pd.DataFrame(data)

    x, y = data.iloc[1:, 0], data.iloc[1:, 1]
    m, b = np.polyfit(np.log10(x), np.log10(y), 1)
    print('m: %f, b: %f' % (m, b))
    plt.plot(x, y)
    plt.grid(alpha=0.1, aa=True)
    plt.xscale('log')
    plt.yscale('log')
    plt.xlabel(data.columns[0], fontsize=18)
    plt.ylabel(data.columns[1], fontsize=18)
    plt.savefig(args.filename + '.svg', format='svg')
    plt.show()

