# code modified from https://www.askpython.com/python/examples/customizing-pyvis-interactive-network-graphs
import random
from pyvis import network as net
from IPython.display import display, HTML
import pandas as pd
import networkx as nx
 
def generate_color():
  return '#%06x' % random.randint(0, 0xFFFFFF)
 

# load the graph nodes and edges from
nodesFile = "/home/josura/Projects/ccc/datiIdo/inputGraphs/1h/nodesWithLR/AT1-metabolites.tsv"
edgesFile = "/home/josura/Projects/ccc/datiIdo/inputGraphs/1h/graphsWithLR/AT1-metabolites.tsv"
nodes_df = pd.read_csv(nodesFile, sep="\t")
edges_df = pd.read_csv(edgesFile, sep="\t")
network_name = "AT1-metabolites"
g_complete =net.Network(height='600px',width='50%',
              bgcolor='white',font_color="red",notebook=True,
              heading="An example Graph for " + network_name,directed=True)

# load the time series data for the values of the nodes through time
timeSeriesFile = "/home/josura/Projects/ccc/datiIdo/inputGraphs/1h/multipleOutputsWithLR/dissipation_0.3-propagation_0.3-conservation_0.3/iterationMatrices/AT1-metabolites.tsv"
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
 
g_complete.show_buttons(['physics'])
 
graph_IFrame = g_complete.show('AnExampleGraphFor_AT1-metabolites.html')
display(HTML('AnExampleGraphFor_AT1-metabolites.html'))