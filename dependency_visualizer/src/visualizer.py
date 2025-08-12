import colorsys
import math
import os
import sys

import matplotlib.patches as patches
import matplotlib.pyplot as plt
import networkx as nx
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
from matplotlib.figure import Figure
from PyQt5.QtWidgets import (
    QApplication,
    QComboBox,
    QFileDialog,
    QLabel,
    QMainWindow,
    QPushButton,
    QVBoxLayout,
    QWidget,
)

layout_dict = {
    "Circular": nx.circular_layout,
    "Kamada-Kawai": nx.kamada_kawai_layout,
    "Spring": nx.spring_layout,
    "Arf": nx.arf_layout,
    "Bipartite": nx.bipartite_layout,
    "Random": nx.random_layout,
    "Spectral": nx.spectral_layout,
    "Spiral": nx.spiral_layout,
    "Multipartite": nx.multipartite_layout,
}


def new_color_generator():
    """
    Generator that continuously yields visually distinct colors in hex format.
    Instead of a fixed saturation/value, we alternate presets and change the hue step.
    """
    h = 0.0
    # Cycle saturation/value tuples for additional variation.
    sat_val_pairs = [(0.6, 0.95), (0.8, 0.90), (0.7, 0.92)]
    pair_index = 0
    # Instead of the golden ratio conjugate, adjust the hue step as needed.
    hue_step = 0.618033988749895
    while True:
        s, v = sat_val_pairs[pair_index]
        rgb = colorsys.hsv_to_rgb(h, s, v)
        hex_color = "#{0:02x}{1:02x}{2:02x}".format(
            int(rgb[0] * 255), int(rgb[1] * 255), int(rgb[2] * 255)
        )
        yield hex_color
        # Cycle saturation/value presets.
        pair_index = (pair_index + 1) % len(sat_val_pairs)
        h = (h + hue_step) % 1.0


COLOR_GENERATOR = new_color_generator()


class DependencyViewer(QMainWindow):
    """
    A PyQt5-based GUI application for visualizing dependency relationships between files and packages.

    ...
    Parameters:
        None
    Attributes:
        main_widget (QWidget): The main widget for the application
        status_label (QLabel): The status label for displaying messages
        load_button (QPushButton): The button for loading dependency files
        layout_selector (QComboBox): The combo box for selecting layout algorithms
        figure (Figure): The figure object for rendering the graph
        canvas (FigureCanvas): The canvas for drawing the graph
        toolbar (NavigationToolbar): The toolbar for zooming and panning
        ax (Axes): The axis for drawing the graph
        graph (networkx.Graph): The dependency graph to visualize
        current_layout (str): The current layout algorithm to use
        selected_node (str): The currently selected node (or None if none selected)

    Methods:
        __init__() -> None:
            Initialize the DependencyViewer window
        on_layout_change(text: str) -> None:
            Event handler for changing the layout algorithm
        on_click(event) -> None:
            Node event handler for selecting a node on the graph
        load_file() -> None:
            Opens a file dialog for user to select a dependency file
        parse_dependency_file(filename: str) -> None:
            Parse a dependency file and build a directed graph from its contents
        draw_graph() -> None:
            Renders the dependency graph visualization with the current layout and selected node

    Key Features:
        - Interactive node selection
        - Multiple layout algorithms
        - Directory structure visualization using concentric circles
        - Color-coded directory hierarchy
        - Edge highlighting for selected nodes
        - Zoom and pan navigation
        - File loading via dialog

    The visualization represents:
        - Files as nodes with concentric circles showing directory structure
        - Dependencies as directed edges between nodes
        - Selected nodes and their connections highlighted
        - Directory hierarchy through color-coding

    Dependencies:
        - PyQt5 for the GUI framework
        - NetworkX for graph operations
        - Matplotlib for visualization
    """

    def __init__(self):
        """
        Initialize the DependencyViewer window.

        ...
        Parameters:
            None

        Attributes:
            self.main_widget (QWidget): The main widget for the application
            self.status_label (QLabel): The status label for displaying messages
            self.load_button (QPushButton): The button for loading dependency files
            self.layout_selector (QComboBox): The combo box for selecting layout algorithms
            self.figure (Figure): The figure object for rendering the graph
            self.canvas (FigureCanvas): The canvas for drawing the graph
            self.toolbar (NavigationToolbar): The toolbar for zooming and panning
            self.ax (Axes): The axis for drawing the graph
            self.graph (networkx.Graph): The dependency graph to visualize
            self.current_layout (str): The current layout algorithm to use
            self.selected_node (str): The currently selected node (or None if none selected)

        Returns:
            None

        Side Effects:
            - Initializes the main window and layout
            - Creates widgets for status, buttons, combo box, and canvas
            - Sets up the graph, layout, and event handlers
        """
        super().__init__()
        # Create the main widget and layout
        self.main_widget = QWidget()
        self.setCentralWidget(self.main_widget)
        layout = QVBoxLayout(self.main_widget)

        # Create status label
        self.status_label = QLabel("Ready")
        layout.addWidget(self.status_label, 0)

        # Create load button
        self.load_button = QPushButton("Load Dependencies")
        self.load_button.clicked.connect(self.load_file)
        layout.addWidget(self.load_button, 0)

        self.change_colors_button = QPushButton("Change Colors")
        self.change_colors_button.clicked.connect(self.on_color_change)
        layout.addWidget(self.change_colors_button, 0)
        self.dir_color_mapping = {}  # initialize directory color mapping

        # Create layout selection combo box
        self.layout_selector = QComboBox()
        self.layout_selector.addItems(list(layout_dict.keys()))
        self.layout_selector.currentTextChanged.connect(self.on_layout_change)
        layout.addWidget(self.layout_selector, 0)

        # Create the figure and canvas
        self.figure = Figure()
        self.canvas = FigureCanvas(self.figure)
        layout.addWidget(self.canvas, 1)

        # Add navigation toolbar for zooming and panning
        self.toolbar = NavigationToolbar(self.canvas, self)
        layout.addWidget(self.toolbar, 0)

        # Get the axis for drawing
        self.ax = self.figure.add_subplot(111)

        # Create the graph
        self.graph = nx.Graph()
        self.graph = nx.DiGraph()

        # Set the default layout style
        self.current_layout = "Circular"

        # Store selected node. None indicates no selection.
        self.selected_node = None

        # Connect the click event.
        self.canvas.mpl_connect("button_press_event", self.on_node_select)

        # Set window properties
        self.setWindowTitle("Dependency Visualizer")
        self.setGeometry(10, 10, 810, 610)

        # remove the axis
        self.ax.set_axis_off()

    def on_layout_change(self, text):
        """
        Event handler for changing the layout algorithm.

        ...
        Parameters:
            text (str): The new layout algorithm to use
        Attributes:
            self.current_layout (str): The layout algorithm to use
        Returns:
            None
        Side Effects:
            - Updates the current layout algorithm
            - Redraws the graph visualization
        """
        self.current_layout = text
        self.draw_graph()

    def on_color_change(self):
        """
        Change the colors of the directory structure.

        ...
        Parameters:

        Returns:
            None
        Side Effects:
            - Updates the directory color mapping
            - Redraws the graph visualization
        """
        # do nothing if no directory color mapping
        if not self.dir_color_mapping:
            return
        self.dir_color_mapping = {
            d: next(COLOR_GENERATOR) for d in self.dir_color_mapping.keys()
        }
        self.draw_graph()

    def on_node_select(self, event):
        """
        Node event handler for selecting a node on the graph.

        ...
        Parameters:
            event (matplotlib event): The event object containing click information
        Attributes:
            self.pos (dict): The node positions in the current layout
            self.selected_node (str): The currently selected node (or None if none selected)
        Returns:
            None

        This method is called when the user clicks on the graph canvas.
        It checks if the click is near a node and updates the selected node accordingly.
        The graph is then redrawn to highlight the selected node and its neighbors.
        """
        if event.inaxes is None:
            return
        # Get click coordinates in data space.
        click_x, click_y = event.xdata, event.ydata
        # Tolerance (in data coordinates) for detecting a click near a node.
        # set tolerance using the graph scale
        tolerance = 0.05
        selected = None
        for node, (x, y) in self.pos.items():
            if math.hypot(click_x - x, click_y - y) < tolerance:
                selected = node
                break
        self.selected_node = selected
        self.draw_graph(recompute_layout=False)

    def load_file(self):
        """
        Opens a file dialog for user to select a dependency file.

        ...
        Parameters:
            None
        Attributes:
            self.graph (networkx.Graph): The dependency graph to visualize
        Returns:
            None
        Side Effects:
            - Opens a file dialog for user to select a dependency file
            - Parses the dependency file and updates the graph
            - Redraws the graph visualization
        """

        filename, _ = QFileDialog.getOpenFileName(
            self, "Open Dependency File", "", "Text Files (*.txt);;All Files (*)"
        )
        if filename:
            self.parse_dependency_file(filename)
            # clear the directory color mapping
            self.dir_color_mapping = {}
            self.draw_graph(recompute_layout=True)

    def parse_dependency_file(self, filename):
        """
        Parse a dependency file and build a directed graph from its contents.
        The file should contain lines representing dependencies between packages,
        using either '-->' for direct dependencies or '-.>' for package hierarchy relationships.
        Args:
            filename (str): Path to the dependency file to parse
        Returns:
            None
        Side Effects:
            - Clears and updates the internal graph (self.graph)
            - Updates the status label with count of nodes and edges
            - Adds all parsed edges to the graph
        Example file format:
            package1 --> package2
            module1 -.> module2
        """

        self.graph.clear()
        edges = []

        with open(filename, "r") as f:
            lines = f.readlines()

        for line in lines:
            line = line.strip()
            if "-->" in line:  # Direct dependency
                source, target = line.split("-->")
                source = source.strip()
                target = target.strip()
                if (source, target) not in edges:
                    edges.append((target, source))

        # Add all edges to the graph
        self.graph.add_edges_from(edges)
        self.status_label.setText(
            f"Loaded {len(self.graph.nodes)} nodes and {len(edges)} edges"
        )

    def draw_graph(self, recompute_layout=True):
        """
        Renders the dependency graph visualization with the current layout and selected node.

        ...
        Parameters:
            None

        Returns:
            None

        Attributes:
            self.graph (networkx.Graph): The dependency graph to visualize
            self.current_layout (str): The layout algorithm to use
            self.selected_node (str): The currently selected node (or None if none selected)
            self.figure (matplotlib.figure.Figure): The figure object for rendering
            self.canvas (matplotlib.backends.backend_qt5agg.FigureCanvasQTAgg): The canvas for drawing

        Side Effects:
            - Clears the figure and redraws the graph visualization
            - Updates the canvas to display the new visualization
        """
        self.figure.clear()
        ax = self.figure.add_subplot(111)
        ax.set_facecolor("#001f3f")
        self.figure.patch.set_facecolor("#001f3f")
        ax.set_aspect("equal")

        # only recompute layout if necessary
        if recompute_layout:
            # Choose the layout algorithm based on the current selection.
            if self.current_layout in layout_dict:
                layout_fn = layout_dict[self.current_layout]

            else:
                layout_fn = nx.circular_layout
                self.current_layout = "Circular"

            # Compute the layout positions for the graph.
            self.pos = (
                layout_fn(self.graph)
                if self.current_layout != "Bipartite"
                else layout_fn(self.graph, self.graph.nodes())
            )

        # Determine neighbors of the selected node.
        selected = self.selected_node
        neighbors = (
            list(self.graph.neighbors(selected))
            + list(self.graph.predecessors(selected))
            if selected
            else []
        )

        # Draw all edges normally.
        nx.draw_networkx_edges(
            self.graph,
            self.pos,
            edge_color="gray",
            arrows=True,
            arrowstyle="-|>",
            arrowsize=20,
            alpha=0.5,
            ax=ax,
        )
        # Re-draw edges incident to the selected node with bold style.
        if selected:
            selected_edges_in = [
                edge for edge in self.graph.edges() if selected == edge[0]
            ]
            selected_edges_out = [
                edge for edge in self.graph.edges() if selected == edge[1]
            ]
            nx.draw_networkx_edges(
                self.graph,
                self.pos,
                edgelist=selected_edges_in,
                edge_color="#0074D9",  # dull blue
                width=3,
                arrows=True,
                arrowstyle="-|>",
                arrowsize=25,
                ax=ax,
            )
            nx.draw_networkx_edges(
                self.graph,
                self.pos,
                edgelist=selected_edges_out,
                edge_color="#ff6666",  # dull red
                width=3,
                arrows=True,
                arrowstyle="-|>",
                arrowsize=25,
                ax=ax,
            )

        # Parameters for drawing concentric circles.
        outer_radius = 0.03  # Radius for the outermost circle.
        shrink_factor = 0.6  # Inner circle is 80% the size of its parent.

        # graph scale

        max_x = max(x for x, y in self.pos.values())
        min_x = min(x for x, y in self.pos.values())
        max_y = max(y for x, y in self.pos.values())
        min_y = min(y for x, y in self.pos.values())

        # re-scale the graph
        ax.set_xlim(min_x - 0.5, max_x + 0.1)
        ax.set_ylim(min_y - 0.1, max_y + 0.1)

        # Draw each node as concentric circles.
        for node in self.graph.nodes():
            x, y = self.pos[node]
            parts = node.split("/")
            directories = parts[:-1]  # Parent directories
            n = len(directories)
            # set subset key to the directory name
            self.graph.nodes[node]["subset"] = (
                directories[-1] if directories else "root"
            )

            # Assign a color per directory name. Only generate new colors if Change Colors button is clicked
            for d in directories:
                if d not in self.dir_color_mapping:
                    self.dir_color_mapping[d] = next(COLOR_GENERATOR)

            # Draw concentric circles for each parent directory.
            for i, d in enumerate(directories):
                radius = outer_radius * (shrink_factor**i)
                color = self.dir_color_mapping[d]
                # Increase linewidth if this node is selected or a neighbor.
                lw = 5
                if node == selected:
                    lw = 8
                elif node in neighbors:
                    lw = 7
                circle = plt.Circle(
                    (x, y), radius, color=color, fill=False, linewidth=lw, alpha=0.7
                )
                ax.add_patch(circle)

            # Draw the file marker as a filled circle inside the innermost circle.
            file_radius = (
                outer_radius * (shrink_factor**n)
                if n > 0
                else outer_radius * shrink_factor
            )
            file_color = "#001f3f"
            if node == selected or node in neighbors:
                file_color = "green"
            file_circle = plt.Circle((x, y), file_radius, color=file_color, fill=True)
            ax.add_patch(file_circle)

        # Add a legend for directory colors.
        legend_handles = []
        for d, color in self.dir_color_mapping.items():
            legend_handles.append(patches.Patch(color=color, label=d))
        ax.legend(handles=legend_handles, loc="upper left", fontsize=8)

        # Draw labels using only the file name. Bold labels
        # for the selected node and its neighbors.
        labels = {}
        for node in self.graph.nodes():
            weight = "normal"
            f_size = 10
            f_color = "white"
            if node == selected:
                # selected node and its neighbors
                weight = "bold"
                f_size = 12
                f_color = "white"
            if node in neighbors:
                # non-selected neighbors
                f_color = "white"
                f_size = 12
            if selected != None and node not in neighbors and node != selected:
                # non-selected non-neighbors
                f_color = "#c0c0c0"
                f_size = 8
            labels[node] = os.path.basename(node)
            # Draw each label individually to set font weight.
            x, y = self.pos[node]
            ax.text(
                x,
                y,
                os.path.basename(node),
                fontsize=f_size,
                fontweight=weight,
                color=f_color,
                horizontalalignment="center",
                verticalalignment="center",
                rotation=15,
            )
        # After drawing nodes, edges, labels, etc.
        ax.set_axis_off()
        self.figure.tight_layout()
        self.canvas.draw()


def main():
    app = QApplication(sys.argv)
    viewer = DependencyViewer()
    viewer.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
