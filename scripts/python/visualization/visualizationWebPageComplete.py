from flask import Flask, render_template, jsonify
import plotly.graph_objects as go
import plotly.io as pio
import numpy as np
import networkx as nx
import pandas as pd

app = Flask(__name__)

# TODO these are variables that will be read from the config file in the future
OutputDirectory = "/home/josura/Projects/ccc/datiIdo/inputGraphs/1h/multipleOutputsWithLR/dissipation_0.3-propagation_0.3-conservation_0.3/iterationMatrices/"
AugmentedGraphDirectory = "/home/josura/Projects/ccc/datiIdo/inputGraphs/1h/outputWithLR/augmentedGraphs/"
InputValuesDirectory = "/home/josura/Projects/ccc/datiIdo/inputGraphs/1h/nodeValuesWithLR/"

# create the list of available types in the folder
types = []
# function to read the file names from a directory and returns them as a list, without the .tsv extension
def read_file_names(directory):
    import os
    file_names = []
    for filename in os.listdir(directory):
        if filename.endswith(".tsv"):
            file_names.append(filename[:-4])
    return file_names

types = read_file_names(OutputDirectory)

# control if the directory is empty
if len(types) == 0:
    raise ValueError("The directory is empty, please check the path: " + OutputDirectory) 

# the network name will be read when a call to the web page /types/<type> is made, network name defaults to the first type in the list
current_network_name = types[0]


# load the graph edges from the augmented graph, graph nodes will be directly taken from the time series data
# edgesFile = "/home/josura/Projects/ccc/datiIdo/inputGraphs/1h/graphsWithLR/" + current_network_name +  ".tsv"
edgesFile = AugmentedGraphDirectory + current_network_name +  ".tsv"
nodesValuesFile = InputValuesDirectory + current_network_name +  ".tsv"
edges_df = pd.read_csv(edgesFile, sep="\t")
nodes_values_df = pd.read_csv(nodesValuesFile, sep="\t")
# contains a column (name) and another column (value) with the initial values of the nodes

# utility function to control if two real numbers are approximately equal
def approximately_equal(a, b, tolerance=1e-9):
    return abs(a - b) < tolerance

# function to read timeseries data
def read_timeseries_data(directoryTimeSeries, directoryInitialInput, type):
    timeSeriesFile = directoryTimeSeries + type +  ".tsv"
    timeSeries_df = pd.read_csv(timeSeriesFile, sep="\t")
    ## preprocess the time series data
    ### drop the last column (useless)
    timeSeries_df = timeSeries_df.drop(timeSeries_df.columns[-1], axis=1)
    ## first row of the matrix is the node names (nodeNames), the rest of the columns are the timepoints (the names go from 0 to the end of the simulation timepoint)
    nodeNames = timeSeries_df['nodeNames']
    ## drop the nodeNames column
    timeSeries_df = timeSeries_df.drop('nodeNames', axis=1)
    ## read timepoints
    tmp_timepoints = timeSeries_df.columns
    ## we need to transpose the matrix so that the columns are the node names and the rows are the timepoints iteration results
    timeSeries_df = timeSeries_df.transpose()
    ## set the column names to be the node names
    timeSeries_df.columns = nodeNames
    ## set the index to be the timepoints
    timeSeries_df.index = tmp_timepoints
    ## add a column to be the timepoints
    # temp_iterationMatrix['time'] = timepoints
    ## change index name to be 'time'
    timeSeries_df.index.name = 'time'

    # reading node_values for the first timepoint
    nodesValuesFile = directoryInitialInput + type +  ".tsv"
    nodes_values_df = pd.read_csv(nodesValuesFile, sep="\t")

    ## adding the 1h real values to the simulated data, since it's the first timepoint
    ### shifting all the timepoints by the minimum interval between them (that is the intra timestep)
    timestep = timeSeries_df.index.astype(float)[1] - timeSeries_df.index.astype(float)[0]
    timeSeries_df.index = timeSeries_df.index.astype(float) + timestep
    ### adding the real values as the 0 timepoint to the data
    pd_row_to_add = pd.DataFrame(index=[0], columns=timeSeries_df.columns)
    for i in range(0,len(timeSeries_df.columns)):
        node_name = timeSeries_df.columns[i]
        if node_name in nodes_values_df['name'].values:
            val = nodes_values_df[nodes_values_df['name'] == node_name]['value'].values[0]
        else:
            val = 0
        pd_row_to_add.iloc[0,i] = val
    timeSeries_df = pd.concat([pd_row_to_add, timeSeries_df], ignore_index=False)
    return timeSeries_df


# load the time series data for the values of the nodes through time
timeSeriesFile = OutputDirectory + current_network_name + ".tsv"
timeSeries_df = read_timeseries_data(OutputDirectory, InputValuesDirectory, current_network_name)
## get the timepoints
timepoints = timeSeries_df.index

allNodes = timeSeries_df.columns.tolist()

# changes edges_df names in case they are not Start and End (for example, if they are 'Source' and 'Target')
# if 'Source' in edges_df.columns and 'Target' in edges_df.columns:
#     edges_df.rename(columns={'Source': 'Start', 'Target': 'End'}, inplace=True)
if 'Source' in edges_df.columns:
    edges_df.rename(columns={'Source': 'Start'}, inplace=True)
if 'Target' in edges_df.columns:
    edges_df.rename(columns={'Target': 'End'}, inplace=True)
if 'weight' in edges_df.columns:
    edges_df.rename(columns={'weight': 'Weight'}, inplace=True)
if 'source' in edges_df.columns:
    edges_df.rename(columns={'source': 'Start'}, inplace=True)
if 'target' in edges_df.columns:
    edges_df.rename(columns={'target': 'End'}, inplace=True)
# also for 

# create the networkx graph
nx_graph = nx.Graph()
# add nodes to the graph
nx_graph.add_nodes_from(allNodes)
# add values to the nodes as size
dict_size = {}
for i in range(0,len(allNodes)):
  val = timeSeries_df.iloc[0,i]
  dict_size[allNodes[i]] = val
nx.set_node_attributes(nx_graph, dict_size, 'size')
# add the values to the nodes as attribute
dict_attr = {}
for i in range(0,len(allNodes)):
  val = timeSeries_df.iloc[0,i]
  dict_attr[allNodes[i]] = val
nx.set_node_attributes(nx_graph, dict_attr, 'value')

# add edges to the graph, the relevant features are Start, End and Weight
for i in range(0,len(edges_df)):
  start = edges_df["Start"].iloc[i]
  end = edges_df["End"].iloc[i]
  weight = edges_df["Weight"].iloc[i]
  nx_graph.add_edge(start,end,weight=weight)

# generate the positions of the nodes
pos = nx.spring_layout(nx_graph, seed=42)  # positions for all nodes

# plotly plotting
edge_x = []
edge_y = []
weights_str = []
for edge in nx_graph.edges():
    x0, y0 = pos[edge[0]]
    x1, y1 = pos[edge[1]]
    weight = nx_graph.edges[edge]['weight']
    weights_str.append(str(weight))
    edge_x.append(x0)
    edge_x.append(x1)
    edge_x.append(None)
    edge_y.append(y0)
    edge_y.append(y1)
    edge_y.append(None)

edge_trace = go.Scatter(
    x=edge_x, y=edge_y,
    line=dict(width=0.3, color='#888'),
    hoverinfo='text',
    mode='lines',
    text=weights_str,
    textposition='top center',
    textfont=dict(
        family='sans serif',
        size=12,
        color='#000'
    ),
    marker=dict(
        line=dict(width=0.5, color='#888')
    )
)


node_x = []
node_y = []
node_sizes_plotly = []
for node in nx_graph.nodes():
    x, y = pos[node]
    node_x.append(x)
    node_y.append(y)
    node_sizes_plotly.append((timeSeries_df[node].iloc[0] + 1) * 10)

node_trace = go.Scatter(
    x=node_x, y=node_y,
    mode='markers',
    hoverinfo='text',
    marker=dict(
        showscale=True,
        # colorscale options
        #'Greys' | 'YlGnBu' | 'Greens' | 'YlOrRd' | 'Bluered' | 'RdBu' |
        #'Reds' | 'Blues' | 'Picnic' | 'Rainbow' | 'Portland' | 'Jet' |
        #'Hot' | 'Blackbody' | 'Earth' | 'Electric' | 'Viridis' |
        colorscale='YlGnBu',
        reversescale=True,
        color=[],
        size=10,
        colorbar=dict(
            thickness=15,
            title=dict(
              text='Values',
              side='right'
            ),
            xanchor='left',
        ),
        line_width=2))




node_values = []
node_text = []
for node in nx_graph.nodes():
    value = nx_graph.nodes[node]['value']
    node_values.append(value)
    # node_text.append('Node: '+str(node))
    node_text.append('Node: '+ str(node) + '\nValue: '+str(value))

node_values_every_timepoint_dict = {}
node_sizes_every_timepoint_dict = {}
node_text_every_timepoint_dict = {}
for i in range(0,len(timepoints)):
    node_values_every_timepoint_dict[timepoints[i]] = timeSeries_df.iloc[i,:].tolist() 
    # node_sizes_every_timepoint_dict[timepoints[i]] = ((timeSeries_df.iloc[i,:]  + 1) * 10).tolist()
    # normalize the values  of the sizes to be between 4 and 22, but take the absolute value of the values so that the size is big even if the value is negative
    node_sizes_every_timepoint_dict[timepoints[i]] = ((timeSeries_df.iloc[i,:].abs()  - timeSeries_df.iloc[i,:].abs().min()) / (timeSeries_df.iloc[i,:].abs().max() - timeSeries_df.iloc[i,:].abs().min()) * 18 + 4).tolist()
    #node_sizes_every_timepoint_dict[timepoints[i]] = ((timeSeries_df.iloc[i,:]  - timeSeries_df.iloc[i,:].min()) / (timeSeries_df.iloc[i,:].max() - timeSeries_df.iloc[i,:].min()) * 18 + 2).tolist()
    node_text_every_timepoint_dict[timepoints[i]] = []
    for j in range(0,len(allNodes)):
        node_text_every_timepoint_dict[timepoints[i]].append('Node: ' + str(allNodes[j]) + '\nValue: '+str(timeSeries_df.iloc[i,j]))


def create_plot_network(timepoint):
    indexTimepoint = -1
    for i in range(0,len(timepoints)):
        timepoint_float = float(timepoint)
        current_timepoint_float = float(timepoints[i])
        if approximately_equal(current_timepoint_float,timepoint_float):
            indexTimepoint = i
            break
    if indexTimepoint == -1:
        raise ValueError("Timepoint not found in the data.")
    node_trace.marker.color = node_values_every_timepoint_dict[timepoints[i]]
    node_trace.marker.size = node_sizes_every_timepoint_dict[timepoints[i]]
    node_trace.text = node_text_every_timepoint_dict[timepoints[i]]
    # plotting the figure
    fig = go.Figure(data=[edge_trace, node_trace],
                layout=go.Layout(
                    title=dict(
                        text="<br>" + current_network_name +  "<br>",
                        font=dict(
                            size=16
                        )
                    ),
                    showlegend=False,
                    hovermode='closest',
                    margin=dict(b=20,l=5,r=5,t=40),
                    annotations=[ dict(
                        text="Conservation:0.3 Dissipation:0.3 Propagation:0.3",
                        showarrow=False,
                        xref="paper", yref="paper",
                        x=0.005, y=-0.002 ) ],
                    xaxis=dict(showgrid=False, zeroline=False, showticklabels=False),
                    yaxis=dict(showgrid=False, zeroline=False, showticklabels=False))
                    )
    # show the figure
    return fig.to_dict()

def create_plot_network_singular(timepoint,timepoints,
                        node_values_every_timepoint_dict,
                        node_sizes_every_timepoint_dict,
                        node_text_every_timepoint_dict):
    indexTimepoint = -1
    for i in range(0,len(timepoints)):
        if timepoints[i] == str(timepoint):
            indexTimepoint = i
            break
    if indexTimepoint == -1:
        raise ValueError("Timepoint not found in the data.")
    node_trace.marker.color = node_values_every_timepoint_dict[str(timepoint)]
    node_trace.marker.size = node_sizes_every_timepoint_dict[str(timepoint)]
    node_trace.text = node_text_every_timepoint_dict[str(timepoint)]
    # plotting the figure
    fig = go.Figure(data=[edge_trace, node_trace],
                layout=go.Layout(
                    title=dict(
                        text="<br>" + current_network_name +  "<br>",
                        font=dict(
                            size=16
                        )
                    ),
                    showlegend=False,
                    hovermode='closest',
                    margin=dict(b=20,l=5,r=5,t=40),
                    annotations=[ dict(
                        text="Conservation:0.3 Dissipation:0.3 Propagation:0.3",
                        showarrow=False,
                        xref="paper", yref="paper",
                        x=0.005, y=-0.002 ) ],
                    xaxis=dict(showgrid=False, zeroline=False, showticklabels=False),
                    yaxis=dict(showgrid=False, zeroline=False, showticklabels=False))
                    )
    # show the figure
    return fig.to_dict()


@app.route('/read_types')
def read_types():
    # read the file names from the directory and returns them as a list, without the .tsv extension
    file_names = read_file_names(OutputDirectory)
    return jsonify(file_names)


@app.route('/')
def index():
    return render_template('index.html', timepoints=timepoints)

@app.route('/types/<string:type>')
def type(type):
    if type in types:
        if type == current_network_name:
            # if the type is already selected, do nothing and return a status
            return jsonify({'status': 'already selected'})
        # fill the trace with the data of the selected type
    else:
        raise ValueError("Type not found in the data.")

# routing for both floats and integers, since the slider has a problem of consistency on what is throwing out
@app.route('/plot/<int:timepoint>')
@app.route('/plot/<float:timepoint>')
def plot(timepoint):
    # plot_json = create_plot(timepoint)
    plot_json = create_plot_network(timepoint)
    return jsonify(plot_json)

@app.route('/plot_singular/<string:type>/<float:timepoint>')
def plot_singular(type, timepoint):
    if type == 'node':
        plot_json = create_plot_network_singular(timepoint,timepoints,
                        node_values_every_timepoint_dict,
                        node_sizes_every_timepoint_dict,
                        node_text_every_timepoint_dict)
    else:
        raise ValueError("Type not found in the data.")
    return jsonify(plot_json)

if __name__ == '__main__':
    app.run(debug=True)
