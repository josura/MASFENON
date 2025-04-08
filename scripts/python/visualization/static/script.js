let intervalId = null;

function updatePlot(timepoint) {
    fetch(`/plot/${timepoint}`)
        .then(response => response.json())
        .then(plotData => {
            Plotly.react('plot', plotData.data, plotData.layout);
        })
        .catch(err => console.error("Error fetching plot:", err));
}

document.addEventListener('DOMContentLoaded', () => {
    const slider = document.getElementById('slider');
    const label = document.getElementById('timepointLabel');
    const playButton = document.getElementById('playButton');

    // Initialize plot
    updatePlot(slider.value);

    slider.oninput = function() {
        label.textContent = this.value;
        updatePlot(this.value);
    };

    playButton.onclick = function() {
        if (intervalId) {
            clearInterval(intervalId);
            intervalId = null;
            playButton.textContent = "▶️ Play";
        } else {
            playButton.textContent = "⏸️ Pause";
            intervalId = setInterval(() => {
                let currentValue = parseFloat(slider.value);
                const max = parseFloat(slider.max);
                const step = parseFloat(slider.step);

                if (currentValue + step > max) {
                    slider.value = slider.min;  // loop back to start
                } else {
                    slider.value = (currentValue + step).toFixed(1);
                }

                label.textContent = slider.value;
                updatePlot(slider.value);
            }, 500);
        }
    };
});
