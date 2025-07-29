import pandas as pd
import plotly.express as px
import sys
import os

#perforrmance times files
NDlib_performance_file = "../NDlib/NDLib_performance.tsv"
netlogo_performance_file = "../netlogo/netlogo_performance.tsv"
MASFENON_performance_file = "../MASFENON/times.tsv"

#memory usage files
NDlib_memory_file = "../NDlib/spaceOccupied.tsv"
netlogo_memory_file = "../netlogo/netlogo_memory.tsv"
MASFENON_memory_file = "../MASFENON/spaceOccupied.tsv"

#reading the performance data into dataframes
ndlib_performance = pd.read_csv(NDlib_performance_file, sep="\t", header=0)
netlogo_performance = pd.read_csv(netlogo_performance_file, sep="\t", header=0)
masfenon_performance = pd.read_csv(MASFENON_performance_file, sep="\t", header=0)

# add a column to masfenon_performance for the time in seconds
masfenon_performance['time_seconds'] = masfenon_performance['time_milliseconds'] / 1000.0

#add a column to every dataframe for the source of the data
ndlib_performance['source'] = 'NDlib'
netlogo_performance['source'] = 'NetLogo'
masfenon_performance['source'] = 'MASFENON'
# concatenate the dataframes on the same columns
performance_data = pd.concat([ndlib_performance, netlogo_performance, masfenon_performance], ignore_index=True)
## select only the relevant columns
performance_data = performance_data[['nodes', 'iterations', 'time_seconds', 'source']]

# select only the nodes from 1000 to 10000 in steps of 1000 (ignore the other nodes configurations )
relevant_nodes = performance_data['nodes']
relevant_nodes = [n for n in relevant_nodes if n >= 1000 and n <= 10000 and n % 1000 == 0]
relevant_performance_data = performance_data[performance_data['nodes'].isin(relevant_nodes)]

# plot the performance data with box plots
fig = px.box(relevant_performance_data, x='nodes', y='time_seconds', color='source',
             title='Performance Comparison of NDlib, NetLogo, and MASFENON',
             labels={'nodes': 'Number of Nodes', 'time_seconds': 'Time (seconds)', 'source': 'Source'},
             points='all')
fig.update_layout(xaxis_title='Number of Nodes', yaxis_title='Time (seconds)')
fig.show()

# doing the same for memory usage data
ndlib_memory = pd.read_csv(NDlib_memory_file, sep="\t", header=0)
netlogo_memory = pd.read_csv(netlogo_memory_file, sep="\t", header=0)
masfenon_memory = pd.read_csv(MASFENON_memory_file, sep="\t", header=0)