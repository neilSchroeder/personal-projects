import os
import tkinter as tk
from tkinter import filedialog, messagebox, ttk
import cv2
import numpy as np
from dataclasses import dataclass
from typing import List, Tuple
import logging

@dataclass
class FrameAnalysis:
    """
    Data class to store the analysis results for a single video frame.

    Attributes:
        frame_number (int): The sequential number of the frame in the video
        mean_intensity (float): Average brightness of the frame
        intensity_change (float): Magnitude of intensity change from previous frame
        potential_edit (bool): Whether this frame likely contains an edit point
    """
    frame_number: int
    mean_intensity: float
    intensity_change: float
    potential_edit: bool

class VideoAnalyzer:
    """
    A memory-efficient video analyzer that detects potential edit points 
    based on frame intensity changes.

    Attributes:
        sensitivity (float): Threshold for detecting edits (0.0 to 1.0)
        prev_intensity (float): Mean intensity of the previous frame
    """
    def __init__(self, sensitivity: float = 0.15):
        """
        Initialize the video analyzer with configurable sensitivity.
        
        Args:
            sensitivity (float): Threshold for detecting edits (0.0 to 1.0)
        """
        self.sensitivity = sensitivity
        self.prev_intensity = None
    
    def detect_edit(self, curr_intensity: float) -> Tuple[bool, float]:
        """
        Detect if a frame represents an edit point based on intensity change.

        Args:
            curr_intensity (float): Mean intensity of the current frame

        Returns:
            Tuple[bool, float]: (is_edit, intensity_change)
        """
        if self.prev_intensity is None:
            self.prev_intensity = curr_intensity
            return False, 0.0
        
        intensity_change = abs(curr_intensity - self.prev_intensity)
        is_edit = intensity_change > (self.sensitivity * 255)
        self.prev_intensity = curr_intensity
        
        return is_edit, intensity_change

    def process_frame(self, frame_data: Tuple[int, np.ndarray]) -> FrameAnalysis:
        """
        Process a single frame to detect potential edits.

        Args:
            frame_data (Tuple[int, np.ndarray]): Tuple of (frame_number, frame)

        Returns:
            FrameAnalysis: Analysis results for the frame
        """
        frame_number, frame = frame_data
        
        # Downscale frame for faster processing
        small_frame = cv2.resize(frame, (0, 0), fx=0.5, fy=0.5)
        gray = cv2.cvtColor(small_frame, cv2.COLOR_BGR2GRAY)
        mean_intensity = np.mean(gray)
        
        # Detect edits
        is_edit, intensity_change = self.detect_edit(mean_intensity)
        
        return FrameAnalysis(
            frame_number=frame_number,
            mean_intensity=mean_intensity,
            intensity_change=intensity_change,
            potential_edit=is_edit
        )

def analyze_video(video_path: str, sensitivity: float = 0.15, progress_callback=None) -> str:
    """
    Analyze a video file to detect potential edit points using a memory-efficient approach.
    
    Args:
        video_path (str): Path to the video file
        sensitivity (float): Sensitivity threshold for edit detection
        progress_callback (callable): Function to call with progress updates

    Returns:
        str: Analysis report including edit detection results
    """
    cap = cv2.VideoCapture(video_path)
    if not cap.isOpened():
        return "Error: Could not open video."

    frame_count = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    fps = cap.get(cv2.CAP_PROP_FPS)
    duration = frame_count / fps
    
    # Initialize report
    report = [
        f"Video analysis report for: {video_path}",
        f"Total frames: {frame_count}",
        f"Frame rate: {fps} fps",
        f"Duration: {duration:.2f} seconds\n"
    ]
    
    analyzer = VideoAnalyzer(sensitivity=sensitivity)
    edit_frames = []
    
    try:
        frame_number = 0
        while True:
            ret, frame = cap.read()
            if not ret:
                break
                
            # Process frame
            analysis = analyzer.process_frame((frame_number, frame))
            
            if analysis.potential_edit:
                edit_frames.append(frame_number)
                report.append(
                    f"Frame {frame_number}: Potential edit detected "
                    f"(intensity change: {analysis.intensity_change:.2f})"
                )
            
            # Release frame memory
            del frame
            frame_number += 1
            
            # Update progress
            if progress_callback and frame_number % 10 == 0:  # Update every 10 frames
                progress = (frame_number / frame_count) * 100
                progress_callback(progress, f"Processing frame {frame_number}/{frame_count}")
                
    finally:
        cap.release()
    
    # Add summary of edits
    if edit_frames:
        report.append(f"\nDetected {len(edit_frames)} potential edits at frames: {edit_frames}")
    else:
        report.append("\nNo potential edits detected")
    
    return "\n".join(report)

class ProcessingWindow(tk.Toplevel):
    """
    A window that shows the progress of video analysis.
    
    Attributes:
        progress_bar (ttk.Progressbar): Progress bar widget
        status_label (tk.Label): Label showing current status
        cancel_button (tk.Button): Button to cancel processing
    """
    def __init__(self, parent):
        super().__init__(parent)
        self.title("Processing Video")
        self.geometry("400x150")
        
        # Center the window
        self.update_idletasks()
        width = self.winfo_width()
        height = self.winfo_height()
        x = (self.winfo_screenwidth() // 2) - (width // 2)
        y = (self.winfo_screenheight() // 2) - (height // 2)
        self.geometry(f"{width}x{height}+{x}+{y}")
        
        # Progress bar
        self.progress_var = tk.DoubleVar()
        self.progress_bar = ttk.Progressbar(
            self,
            variable=self.progress_var,
            maximum=100,
            length=300,
            mode='determinate'
        )
        self.progress_bar.pack(pady=20)
        
        # Status label
        self.status_label = tk.Label(
            self,
            text="Initializing...",
            font=("Arial", 10)
        )
        self.status_label.pack(pady=10)
        
        # Make the window modal
        self.transient(parent)
        self.grab_set()
        
    def update_progress(self, progress: float, status: str):
        """Update the progress bar and status label"""
        self.progress_var.set(progress)
        self.status_label.config(text=status)
        self.update()

def open_file_dialog():
    """
    Open a file dialog to select a video file for analysis. 
    Shows progress in a GUI window and saves the analysis report.
    """
    initial_dir = os.getcwd()+"/videos"
    file_path = filedialog.askopenfilename(
        initialdir=initial_dir, 
        filetypes=[
            ("MP4 files", "*.mp4"),
            ("AVI files", "*.avi"),
            ("MOV files", "*.mov"),
            ("All files", "*.*")
        ]
    )
    
    if file_path:
        # Create processing window
        processing_window = ProcessingWindow(root)
        
        try:
            # Analyze video with progress updates
            report = analyze_video(
                file_path,
                progress_callback=processing_window.update_progress
            )

            # Save report
            with open("reports/video_analysis_report.txt", "w") as report_file:
                report_file.write(report)
            messagebox.showinfo(
                "Analysis Complete", 
                "The video analysis report has been saved to 'reports/video_analysis_report.txt'"
            )
        except Exception as e:
            messagebox.showerror("Error", f"An error occurred during analysis: {str(e)}")
        finally:
            # Close processing window
            processing_window.destroy()
    else:
        messagebox.showwarning("No file selected", "Please select a video file to analyze.")

def main():
    """
    Main entry point for the Video Edit Detector application.
    Creates and runs the main GUI window.
    """
    global root
    root = tk.Tk()
    root.title("Video Edit Detector")
    root.geometry("400x150")
    
    # Center the window
    root.update_idletasks()
    width = root.winfo_width()
    height = root.winfo_height()
    x = (root.winfo_screenwidth() // 2) - (width // 2)
    y = (root.winfo_screenheight() // 2) - (height // 2)
    root.geometry(f"{width}x{height}+{x}+{y}")
    
    # Add widgets
    tk.Label(
        root, 
        text="Select a video file to analyze for edits:",
        font=("Arial", 12)
    ).pack(pady=20)
    
    tk.Button(
        root, 
        text="Browse", 
        command=open_file_dialog,
        width=20,
        height=2
    ).pack(pady=10)

    root.mainloop()

if __name__ == "__main__":
    main()