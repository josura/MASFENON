#!/usr/bin/env python3
import sys
import time
import networkx as nx
import ndlib.models.epidemics as ep
import ndlib.models.ModelConfig as mc
import os

def load_graph(path):
    G = nx.read_weighted_edgelist(path, delimiter='\t', nodetype=int, data=(('weight',float),))
    return G

def run_model(G, model_type='sir', iterations=100, beta=0.01, gamma=0.005, fraction_infected=0.05):
    if model_type == 'sir':
        model = ep.SIRModel(G)
        cfg = mc.Configuration()
        cfg.add_model_parameter('beta', beta)
        cfg.add_model_parameter('gamma', gamma)
        cfg.add_model_parameter('fraction_infected', fraction_infected)
    elif model_type == 'si':
        model = ep.SIModel(G)
        cfg = mc.Configuration()
        cfg.add_model_parameter('beta', beta)
        cfg.add_model_parameter('fraction_infected', fraction_infected)
    else:
        raise ValueError(f"Unsupported model type: {model_type}")
    
    model.set_initial_status(cfg)
    
    start_time = time.time()
    iterations_data = model.iteration_bunch(iterations)
    end_time = time.time()

    elapsed = end_time - start_time
    return iterations_data, elapsed

def save_performance(num_nodes, num_iterations, elapsed_time, filename="NDLib_performance.tsv"):
    header_needed = not os.path.exists(filename)
    with open(filename, "a") as f:
        if header_needed:
            f.write("nodes\titerations\ttime_seconds\n")
        f.write(f"{num_nodes}\t{num_iterations}\t{elapsed_time:.6f}\n")

def main():
    if len(sys.argv) < 2:
        print("Usage: python simulate.py <edgelist-file> [model] [iterations]")
        sys.exit(1)

    fname = sys.argv[1]
    model_type = sys.argv[2].lower() if len(sys.argv) > 2 else 'sir'
    num_iterations = int(sys.argv[3]) if len(sys.argv) > 3 else 100

    print(f"Loading graph from {fname} …")
    G = load_graph(fname)
    print(f"Graph loaded: {G.number_of_nodes()} nodes, {G.number_of_edges()} edges")

    try:
        results, elapsed_time = run_model(G, model_type=model_type, iterations=num_iterations)
    except ValueError as e:
        print(str(e))
        sys.exit(1)

    print(f"Ran {model_type.upper()} model for {num_iterations} iterations in {elapsed_time:.4f} seconds")

    save_performance(G.number_of_nodes(), num_iterations, elapsed_time)

    # Optional: print final state summary
    last = results[-1]["status"]
    counts = {s: list(last.values()).count(s) for s in set(last.values())}
    print(f"Final node status counts: {counts}")

if __name__ == "__main__":
    main()
