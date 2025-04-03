from flask import Flask, render_template, jsonify
import plotly.graph_objects as go
import plotly.io as pio
import numpy as np

app = Flask(__name__)

# Example time series data
timepoints = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

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
