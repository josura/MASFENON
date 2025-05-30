# code modified from https://www.askpython.com/python/examples/customizing-pyvis-interactive-network-graphs
import random
from pyvis import network as net
from IPython.display import display, HTML
import pandas as pd
import networkx as nx
import plotly.graph_objects as go
import plotly.express as px

def generate_color():
  return '#%06x' % random.randint(0, 0xFFFFFF)


# load the graph nodes and edges from
nodesFile = "/home/josura/Projects/ccc/datiIdo/inputGraphs/1h/nodesWithLR/AT1-metabolites.tsv"
edgesFile = "/home/josura/Projects/ccc/datiIdo/inputGraphs/1h/graphsWithLR/AT1-metabolites.tsv"
nodes_df = pd.read_csv(nodesFile, sep="\t")
edges_df = pd.read_csv(edgesFile, sep="\t")
network_name = "AT1-metabolites"
g_from_nx = net.Network(height='600px',width='50%',
              bgcolor='white',font_color="red",notebook=True,
              heading="An example Graph for " + network_name,directed=True)

# load the time series data for the values of the nodes through time
timeSeriesFile = "/home/josura/Projects/ccc/datiIdo/inputGraphs/1h/multipleOutputsWithLR/dissipation_0.3-propagation_0.3-conservation_0.3/iterationMatrices/" + network_name + ".tsv"
timeSeries_df = pd.read_csv(timeSeriesFile, sep="\t")
## preprocess the time series data
### drop the last column (useless)
timeSeries_df = timeSeries_df.drop(timeSeries_df.columns[-1], axis=1)
## first row of the matrix is the node names (nodeNames), the rest of the columns are the timepoints (the names go from 0 to the end of the simulation timepoint)
nodeNames = timeSeries_df['nodeNames']
## drop the nodeNames column
timeSeries_df = timeSeries_df.drop('nodeNames', axis=1)
## get the timepoints
timepoints = timeSeries_df.columns
## we need to transpose the matrix so that the columns are the node names and the rows are the timepoints iteration results
timeSeries_df = timeSeries_df.transpose()
## set the column names to be the node names
timeSeries_df.columns = nodeNames
## set the index to be the timepoints
timeSeries_df.index = timepoints
## add a column to be the timepoints
# temp_iterationMatrix['time'] = timepoints
## change index name to be 'time'
timeSeries_df.index.name = 'time'


allNodes = timeSeries_df.columns.tolist()

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
  node_values_every_timepoint[timepoints[i]] = timeSeries_df.iloc[i,:].tolist() 
  node_text_every_timepoint_dict[timepoints[i]] = 'Node: ' + str(node) + '\nValue: '+str(timeSeries_df.iloc[i,:].tolist()))

# create a page with a slider, where the slider is the timepoints
# create a slider for the timepoints



node_trace.marker.color = node_values
node_trace.marker.size = node_sizes_plotly
node_trace.text = node_text

fig = go.Figure(data=[edge_trace, node_trace],
             layout=go.Layout(
                title=dict(
                    text="<br>AT1-metabolites<br>",
                    font=dict(
                        size=16
                    )
                ),
                showlegend=False,
                hovermode='closest',
                margin=dict(b=20,l=5,r=5,t=40),
                annotations=[ dict(
                    text="Python code: <a href='https://plotly.com/python/network-graphs/'> https://plotly.com/python/network-graphs/</a>",
                    showarrow=False,
                    xref="paper", yref="paper",
                    x=0.005, y=-0.002 ) ],
                xaxis=dict(showgrid=False, zeroline=False, showticklabels=False),
                yaxis=dict(showgrid=False, zeroline=False, showticklabels=False))
                )
fig.show()


# # create the pyvis graph
# g_from_nx = g_from_nx.from_nx(nx_graph)
# create the plotly graph


# graph_from_nx_IFrame = g_from_nx.show('AnExampleGraphFor_AT1-metabolites_from_nx.html')
# display(HTML('AnExampleGraphFor_AT1-metabolites_from_nx.html'))



# g_complete =net.Network(height='600px',width='50%',
#               bgcolor='white',font_color="red",notebook=True,
#               heading="An example Graph for " + network_name,directed=True)
# for i in range(0,len(allNodes)):
#   # the color is light blue if value is bigger than 0, otherwise it is light red
#   val = timeSeries_df.iloc[0,i]
#   if(val>=0):
#     c = 'lightblue'
#   else:
#     c = 'lightcoral'
#   borderWidth = random.randint(3,5)
#   g_complete.add_node(allNodes[i],label=str(allNodes[i]),color=c,value=val,
#                       title=str(val),borderWidth=borderWidth)


# colors=[]
# for i in range(1,11):
#   c = generate_color()
#   while(c in colors):
#       c = generate_color()
#   colors.append(c)

#   val = generate_size_node()
#   b = random.randint(3,5)

#   g_complete.add_node(i,label=str(i),color=c,value=val,
#                       title="Hello! I am Node "+str(i),borderWidth=b)

# i=0
# chosen_set = []
# while(i!=20):
#   eg = generate_edge()
#   if(eg[0]!=eg[1] and not (eg in chosen_set)):
#       chosen_set.append(eg)
#       g_complete.add_edge(eg[0],eg[1])
#       i+=1

# g_complete.show_buttons(['physics'])

# graph_IFrame = g_complete.show('AnExampleGraphFor_AT1-metabolites.html')
# display(HTML('AnExampleGraphFor_AT1-metabolites.html'))