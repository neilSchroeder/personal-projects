import ast
import os
from pathlib import Path
from typing import Dict, List, Set


class ImportAnalyzer(ast.NodeVisitor):
    def __init__(self, current_module: str):
        self.imports = set()
        self.current_module = current_module
        self.base_path = Path(current_module).parent

    def resolve_relative_import(self, level: int, name: str) -> str:
        """Resolve a relative import to its absolute path."""
        if level == 0:
            return name

        # Split the current module into parts
        parts = self.current_module.split(".")

        # Go up 'level' number of directories
        if len(parts) < level:
            raise ImportError("Attempted relative import beyond top-level package")

        base = ".".join(parts[:-level])
        return f"{base}.{name}" if base else name

    def visit_Import(self, node):
        """Handle regular imports (import x.y.z)."""
        for alias in node.names:
            self.imports.add(alias.name)
        self.generic_visit(node)

    def visit_ImportFrom(self, node):
        """Handle 'from' imports, including relative ones."""
        if node.level == 0:  # Absolute import
            if node.module:
                for alias in node.names:
                    self.imports.add(f"{node.module}.{alias.name}")
        else:  # Relative import
            if node.module:
                base = self.resolve_relative_import(node.level, node.module)
                for alias in node.names:
                    self.imports.add(f"{base}.{alias.name}")
            else:
                base = self.resolve_relative_import(node.level, "")
                for alias in node.names:
                    self.imports.add(f"{base}{alias.name}")


def analyze_file(file_path: Path, module_name: str) -> Set[str]:
    """Analyze a Python file and return its imports."""
    try:
        with open(file_path, "r", encoding="utf-8") as f:
            tree = ast.parse(f.read())
        analyzer = ImportAnalyzer(module_name)
        analyzer.visit(tree)
        return analyzer.imports
    except Exception as e:
        print(f"Error analyzing {file_path}: {e}")
        return set()


def analyze_project(project_path: str) -> Dict[str, Set[str]]:
    """Analyze all Python files in a project directory."""
    project_path = Path(project_path)
    dependencies = {}

    for file_path in project_path.rglob("*.py"):
        if file_path.name == "__init__.py":
            # For __init__.py files, use the directory name as the module name
            relative_path = file_path.parent.relative_to(project_path)
        else:
            # For regular .py files, include the filename without extension
            relative_path = file_path.relative_to(project_path)
            if relative_path.name == "__main__.py":
                continue  # Skip __main__.py files

        # Convert path to module notation
        module_parts = list(relative_path.parts)
        if file_path.name != "__init__.py":
            module_parts[-1] = module_parts[-1][:-3]  # Remove .py extension
        module_name = ".".join(module_parts)

        imports = analyze_file(file_path, module_name)
        if module_name:  # Skip empty module names
            dependencies[module_name] = imports

    return dependencies


def generate_mermaid(dependencies: Dict[str, Set[str]]) -> str:
    """Generate a Mermaid graph definition from dependencies."""
    mermaid = ["graph TD"]

    # Create nodes with proper module hierarchy
    nodes = set()
    for module in dependencies:
        parts = module.split(".")
        current = ""
        for i, part in enumerate(parts):
            parent = current
            current = f"{parent}.{part}" if parent else part
            if current not in nodes:
                safe_name = current.replace(".", "/")
                mermaid.append(f'    {safe_name}["{current}"]')
                if parent:  # Add submodule relationship
                    safe_parent = parent.replace(".", "/")
                    mermaid.append(f"    {safe_parent} -.-> {safe_name}")
                nodes.add(current)

    # Create edges for imports
    for module, imports in dependencies.items():
        safe_module = module.replace(".", "/")
        for imp in imports:
            # Find the longest matching project module
            matching_modules = [
                m for m in dependencies.keys() if imp.startswith(m + ".") or imp == m
            ]
            if matching_modules:
                target_module = max(matching_modules, key=len)
                safe_imp = target_module.replace(".", "/")
                if safe_module != safe_imp:  # Avoid self-references
                    mermaid.append(f"    {safe_module} --> {safe_imp}")

    return "\n".join(mermaid)


def analyze_and_visualize(project_path: str) -> str:
    """Analyze a project and return a Mermaid graph definition."""
    dependencies = analyze_project(project_path)
    return generate_mermaid(dependencies)


# Example usage
if __name__ == "__main__":
    project_path = "/home/nschroed/Documents/work/cms-ecal-scales-and-smearings"
    mermaid_graph = analyze_and_visualize(project_path)
    print(mermaid_graph)
