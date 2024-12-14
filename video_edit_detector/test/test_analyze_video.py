import unittest
from unittest.mock import patch, MagicMock
import numpy as np
from analyze_video import VideoAnalyzer, analyze_video, FrameAnalysis

class TestVideoAnalyzer(unittest.TestCase):
    def setUp(self):
        self.analyzer = VideoAnalyzer(sensitivity=0.15)

    def test_detect_edit_no_previous_frame(self):
        is_edit, intensity_change = self.analyzer.detect_edit(100.0)
        self.assertFalse(is_edit)
        self.assertEqual(intensity_change, 0.0)

    def test_detect_edit_with_previous_frame(self):
        self.analyzer.prev_intensity = 100.0
        is_edit, intensity_change = self.analyzer.detect_edit(150.0)
        self.assertTrue(is_edit)
        self.assertEqual(intensity_change, 50.0)

    def test_process_frame(self):
        frame_data = (1, np.zeros((100, 100, 3), dtype=np.uint8))
        analysis = self.analyzer.process_frame(frame_data)
        self.assertIsInstance(analysis, FrameAnalysis)
        self.assertEqual(analysis.frame_number, 1)
        self.assertAlmostEqual(analysis.mean_intensity, 0.0)
        self.assertFalse(analysis.potential_edit)

class TestAnalyzeVideo(unittest.TestCase):
    @patch('analyze_video.cv2.VideoCapture')
    def test_analyze_video(self, mock_video_capture):
        mock_cap = MagicMock()
        mock_video_capture.return_value = mock_cap
        mock_cap.isOpened.return_value = True
        mock_cap.read.side_effect = [(True, np.zeros((100, 100, 3), dtype=np.uint8))] * 10 + [(False, None)]
        mock_cap.get.side_effect = [10, 1.0]  # frame count, fps

        report = analyze_video("dummy_path.mp4", sensitivity=0.15)
        self.assertIn("Video analysis report for: dummy_path.mp4", report)
        self.assertIn("Total frames: 10", report)
        self.assertIn("Frame rate: 1.0 fps", report)
        self.assertIn("Duration: 10.00 seconds", report)
        self.assertIn("No potential edits detected", report)

    @patch('analyze_video.cv2.VideoCapture')
    def test_analyze_video_with_edits(self, mock_video_capture):
        mock_cap = MagicMock()
        mock_video_capture.return_value = mock_cap
        mock_cap.isOpened.return_value = True
        frames = [
            (True, np.zeros((100, 100, 3), dtype=np.uint8)),
            (True, np.ones((100, 100, 3), dtype=np.uint8) * 255)
        ] * 5 + [(False, None)]
        mock_cap.read.side_effect = frames
        mock_cap.get.side_effect = [10, 1.0]  # frame count, fps

        report = analyze_video("dummy_path.mp4", sensitivity=0.15)
        self.assertIn("Detected 9 potential edits at frames:", report)

if __name__ == '__main__':
    unittest.main()