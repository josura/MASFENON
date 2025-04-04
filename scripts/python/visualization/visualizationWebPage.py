from flask import Flask, render_template, jsonify
import plotly.graph_objects as go
import plotly.io as pio
import numpy as np
import networkx as nx
import pandas as pd

app = Flask(__name__)

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

allNodes = timeSeries_df.columns.tolist()

# Function generating Plotly plot for each timepoint
def create_plot(timepoint):
    np.random.seed(timepoint)
    x = np.linspace(0, 10, 100).tolist()  # <-- convert to list explicitly
    y = (np.sin(np.array(x) + timepoint) + np.random.normal(0, 0.1, size=len(x))).tolist()  # <-- also convert y
    fig = go.Figure(data=[go.Scatter(x=x, y=y, mode='lines+markers')])
    fig.update_layout(
        title=f"Timepoint: {timepoint}",
        xaxis_title='X Axis',
        yaxis_title='Y Axis',
        margin=dict(l=40, r=40, t=40, b=40)
    )
    return fig.to_dict()

@app.route('/')
def index():
    return render_template('index.html', timepoints=timepoints)

@app.route('/plot/<int:timepoint>')
def plot(timepoint):
    plot_json = create_plot(timepoint)
    return jsonify(plot_json)

if __name__ == '__main__':
    app.run(debug=True)
