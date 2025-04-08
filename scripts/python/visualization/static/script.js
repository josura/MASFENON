let intervalId = null;

function updatePlot(timepoint) {
    // cast timepoint to double
    timepoint = parseFloat(timepoint);
    if (isNaN(timepoint)) {
        console.error("Invalid timepoint:", timepoint);
        return;
    }
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
    const typeSelect = document.getElementById("typeSelect");

    // Fetch types from the server
    fetch("/read_types") 
        .then(response => {
            if (!response.ok) {
                throw new Error("Failed to fetch types");
            }
            return response.json();
        })
        .then(types => {
            // Populate the dropdown
            types.forEach(type => {
                const option = document.createElement("option");
                option.value = type;
                option.textContent = type;
                typeSelect.appendChild(option);
            });
        })
        .catch(error => {
            console.error("Error fetching types:", error);
        });

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
