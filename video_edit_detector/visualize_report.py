import os
import re
from typing import List, Tuple
import cv2
from PIL import Image
import numpy as np
import tkinter as tk
from tkinter import filedialog, messagebox, ttk

class EditVisualizer:
    def __init__(self, frames_before: int = 30, frames_after: int = 30, cluster_threshold: int = 120):
        """
        Initialize the visualizer with configuration for GIF creation.
        
        Args:
            frames_before (int): Number of frames to include before edit point
            frames_after (int): Number of frames to include after edit point
            cluster_threshold (int): Frame difference threshold to consider edits as part of same cluster
        """
        self.frames_before = frames_before
        self.frames_after = frames_after
        self.cluster_threshold = cluster_threshold
        self.output_dir = os.path.join("reports", "visualizations")
        os.makedirs(self.output_dir, exist_ok=True)
    
    def extract_video_info(self, report_path: str) -> Tuple[str, List[Tuple[int, float]]]:
        """
        Extract video path and edit points with their intensity changes from the report.
        
        Args:
            report_path (str): Path to the video analysis report file
            
        Returns:
            Tuple[str, List[Tuple[int, float]]]: Video path and list of (frame_number, intensity) tuples
        """
        video_path = None
        edit_points = []
        
        with open(report_path, 'r') as f:
            first_line = f.readline().strip()
            # Extract video path from first line
            video_path_match = re.match(r'Video analysis report for: (.+)$', first_line)
            if video_path_match:
                video_path = video_path_match.group(1)
            
            # Extract frame numbers and intensity changes
            for line in f:
                match = re.search(r'Frame (\d+): Potential edit detected \(intensity change: ([\d.]+)\)', line)
                if match:
                    frame_num = int(match.group(1))
                    intensity = float(match.group(2))
                    edit_points.append((frame_num, intensity))
        
        return video_path, edit_points
    
    def cluster_edit_points(self, edit_points: List[Tuple[int, float]]) -> List[List[Tuple[int, float]]]:
        """
        Group edit points that are close together into clusters.
        
        Args:
            edit_points: List of (frame_number, intensity) tuples
            
        Returns:
            List of clusters, where each cluster is a list of edit points
        """
        clusters = []
        current_cluster = []
        
        for i, (frame, intensity) in enumerate(edit_points):
            if not current_cluster:
                current_cluster.append((frame, intensity))
            else:
                prev_frame = current_cluster[-1][0]
                if frame - prev_frame <= self.cluster_threshold:
                    current_cluster.append((frame, intensity))
                else:
                    clusters.append(current_cluster)
                    current_cluster = [(frame, intensity)]
        
        if current_cluster:
            clusters.append(current_cluster)
            
        return clusters
    
    def create_gif(self, video_path: str, cluster: List[Tuple[int, float]], progress_callback=None) -> str:
        """
        Create a GIF showing frames around a cluster of edit points.
        
        Args:
            video_path (str): Path to the source video
            cluster: List of (frame_number, intensity) tuples in the cluster
            progress_callback (callable): Optional callback for progress updates
            
        Returns:
            str: Path to the generated GIF file
        """
        cap = cv2.VideoCapture(video_path)
        if not cap.isOpened():
            raise ValueError("Could not open video file")
        
        # Calculate frame range for the cluster
        start_frame = max(0, cluster[0][0] - self.frames_before)
        end_frame = cluster[-1][0] + self.frames_after
        
        frames = []
        current_frame = 0
        
        try:
            while current_frame <= end_frame:
                ret, frame = cap.read()
                if not ret:
                    break
                
                if current_frame >= start_frame:
                    # Add a red marker on frames that are edit points
                    for edit_frame, intensity in cluster:
                        if current_frame == edit_frame:
                            # Add red text with frame number and intensity
                            text = f"Frame {edit_frame} (Intensity: {intensity:.1f})"
                            frame = cv2.putText(frame, text, (10, 30), 
                                              cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
                    
                    # Resize frame for reasonable GIF size
                    frame = cv2.resize(frame, (640, 360))  # Increased size for better visibility
                    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
                    frames.append(Image.fromarray(frame_rgb))
                    
                    if progress_callback:
                        progress = ((current_frame - start_frame) / 
                                  (end_frame - start_frame) * 100)
                        progress_callback(progress, f"Processing frame {current_frame}")
                
                current_frame += 1
        finally:
            cap.release()
        
        # Save as GIF
        cluster_start = cluster[0][0]
        cluster_end = cluster[-1][0]
        gif_path = os.path.join(self.output_dir, f"edit_cluster_{cluster_start}-{cluster_end}.gif")
        
        # Save with longer duration for edit point frames
        durations = [50] * len(frames)  # 50ms = 20fps
        frames[0].save(
            gif_path,
            save_all=True,
            append_images=frames[1:],
            duration=durations,
            loop=0
        )
        
        return gif_path
    

class VisualizerGUI:
    """
    GUI application for creating visualizations of detected edit points.
    """
    
    def __init__(self):
        """Initialize the GUI window and components."""
        self.root = tk.Tk()
        self.root.title("Edit Point Visualizer")
        self.root.geometry("500x250")  # Reduced height since we removed one button
        
        # Center window
        self.center_window()
        
        # Create visualizer instance
        self.visualizer = EditVisualizer()
        
        # Create GUI elements
        self.create_widgets()
    
    def center_window(self):
        """Center the window on the screen."""
        self.root.update_idletasks()
        width = self.root.winfo_width()
        height = self.root.winfo_height()
        x = (self.root.winfo_screenwidth() // 2) - (width // 2)
        y = (self.root.winfo_screenheight() // 2) - (height // 2)
        self.root.geometry(f"{width}x{height}+{x}+{y}")
    
    def create_widgets(self):
        """Create and arrange GUI widgets."""
        # Instructions
        tk.Label(
            self.root,
            text="Generate GIFs for detected edit points",
            font=("Arial", 12, "bold")
        ).pack(pady=20)
        
        # Single button for report selection
        tk.Button(
            self.root,
            text="Select Report",
            command=self.select_report,
            width=20,
            height=2
        ).pack(pady=10)
        
        # Status label
        self.status_label = tk.Label(
            self.root,
            text="Select report file to begin",
            font=("Arial", 10)
        )
        self.status_label.pack(pady=20)
        
        # Progress bar (hidden initially)
        self.progress_var = tk.DoubleVar()
        self.progress_bar = ttk.Progressbar(
            self.root,
            variable=self.progress_var,
            maximum=100,
            length=400,
            mode='determinate'
        )
        
        # Store file paths
        self.report_path = None
    
    def select_report(self):
        """Handle report file selection and start processing."""
        self.report_path = filedialog.askopenfilename(
            filetypes=[
                ("Text files", "*.txt"),
                ("All files", "*.*")
            ]
        )
        
        if self.report_path:
            self.status_label.config(text="Processing...")
            self.progress_bar.pack(pady=20)
            self.root.update()
            self.process_video()
    
    def update_progress(self, progress: float, status: str):
        """Update progress bar and status label."""
        self.progress_var.set(progress)
        self.status_label.config(text=status)
        self.root.update()
    
    def process_video(self):
        """Process the video and create GIFs for each edit point."""
        try:
            # Extract video path and edit points from report
            video_path, edit_points = self.visualizer.extract_video_info(self.report_path)
            
            if not video_path:
                raise ValueError("Could not find video path in report")
            
            if not os.path.exists(video_path):
                raise FileNotFoundError(f"Video file not found: {video_path}")
            
            # Group edit points into clusters
            clusters = self.visualizer.cluster_edit_points(edit_points)
            
            for i, cluster in enumerate(clusters):
                overall_progress = (i / len(clusters)) * 100
                self.status_label.config(
                    text=f"Processing cluster {i+1}/{len(clusters)}"
                )
                
                gif_path = self.visualizer.create_gif(
                    video_path,
                    cluster,
                    progress_callback=self.update_progress
                )
            
            self.status_label.config(
                text=f"Complete! Generated {len(clusters)} GIFs in {self.visualizer.output_dir}"
            )
            messagebox.showinfo(
                "Processing Complete",
                f"Generated {len(clusters)} GIFs in {self.visualizer.output_dir}"
            )
            
        except Exception as e:
            messagebox.showerror("Error", str(e))
        finally:
            self.progress_bar.pack_forget()
            self.report_path = None
    
    def run(self):
        """Start the GUI application."""
        self.root.mainloop()

if __name__ == "__main__":
    app = VisualizerGUI()
    app.run()