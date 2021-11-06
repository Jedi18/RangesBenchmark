import csv
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np
from os import listdir
import os

def readCsv():
    labelToCol = {
        'seq' : 'red',
        'par' : 'blue',
        'ranges' : 'green'
    }

    folder_path = 'C:\\Users\\targe\Documents\\ranges_csv\\ranges_benchmarks'
    folders = listdir(folder_path + '\\csv_files')

    for scan_file in folders:
        fig, ax = plt.subplots()

        inputSizes = []
        seqTime = []
        parTime = []
        rangesTime = []

        parRelative = []
        rangesRelative = []

        scan_file_path = folder_path + '\\csv_files\\' + scan_file
        with open(scan_file_path) as csvfile:
            valueReader = csv.reader(csvfile)
            first = True
            c = 5
            for row in valueReader:
                N = float(row[0])
                seq = float(row[1])
                par = float(row[2])
                ranges = float(row[3])

                inputSizes.append(c)
                c += 1
                seqTime.append(seq)
                parTime.append(par)
                rangesTime.append(ranges)

                parRelative.append(seq/par)
                rangesRelative.append(seq/ranges)
        
        ax.set_xticks(range(5, 32, 1))

        ax.plot(inputSizes, parRelative, color=labelToCol['par'])
        ax.plot(inputSizes, rangesRelative, color=labelToCol['ranges'])
        ax.plot([5, 30], [1, 1], color=labelToCol['seq'])

        ax.set_xlabel('i where the input size is 2^i')
        ax.set_ylabel('Relative speedup to sequential')

        handles = []
        handles.append(mpatches.Patch(color=labelToCol['seq'], label="Seq"))
        handles.append(mpatches.Patch(color=labelToCol['par'], label="Par"))
        handles.append(mpatches.Patch(color=labelToCol['ranges'], label="Ranges"))
        handles.append(mpatches.Patch(color='white', label=""))
        handles.append(mpatches.Patch(color='white', label="L1 Cache - 32 KiB"))
        handles.append(mpatches.Patch(color='white', label="L2 Cache - 512 KiB"))
        handles.append(mpatches.Patch(color='white', label="L3 Cache - 16 MiB"))

        plt.legend(loc="upper left", handles=handles)
        plt.savefig(folder_path + '\\plots\\' + os.path.splitext(scan_file)[0] + '.png')

if __name__ == "__main__":
    readCsv()