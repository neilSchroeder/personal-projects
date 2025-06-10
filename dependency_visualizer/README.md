# Dependency Visualizer

A Python tool for visualizing dependencies between modules in Python projects.

## Components

- **mapper.py**: Analyzes Python projects and generates dependency graphs
- **visualizer.py**: Interactive GUI for visualizing dependency relationships

## Features

- Static analysis of Python module dependencies
- Interactive graph visualization
- Multiple layout options (Kamada-Kawai, Spring, Circular)
- Directory-based color coding
- Click-to-highlight dependencies
- Zoom and pan capabilities

## Requirements

- Python 3.x
- PyQt5
- matplotlib
- networkx

## Installation

```bash
pip install PyQt5 matplotlib networkx
```

## Usage

1. Run the mapper to analyze your project:
```bash
python mapper.py > dependencies.txt
```

2. Launch the visualizer:
```bash
python visualizer.py
```

3. Click "Load Dependencies" and select your dependencies.txt file

## License

MIT License

## Author

Nick Schroeder