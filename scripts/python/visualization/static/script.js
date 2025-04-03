function updatePlot(timepoint) {
    fetch(`/plot/${timepoint}`)
        .then(response => response.json())
        .then(plotData => {
            Plotly.react('plot', plotData.data, plotData.layout);
        });
}

document.addEventListener('DOMContentLoaded', function() {
    const slider = document.getElementById('slider');
    const label = document.getElementById('timepointLabel');

    slider.oninput = function() {
        label.textContent = this.value;
        updatePlot(this.value);
    };

    // Initialize the first plot
    updatePlot(slider.value);
});
