import matplotlib.pyplot as plt

for plot_type in ['subtree', 'duplen']:
    fin = open(plot_type + '.csv')
    lines = fin.readlines()[1:]
    x = map(lambda l : int(l.split(",")[0]), lines)
    y = map(lambda l : int(l.split(",")[1]), lines)

    plt.loglog(x, y)
    plt.xlabel(plot_type + 'size')
    plt.ylabel('frequency')
    #plt.axis([0, 1000, 0, 10000])
    plt.savefig(plot_type + '_plot.png')
    plt.clf()
