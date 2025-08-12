import unittest
from unittest.mock import patch, mock_open, MagicMock
import numpy as np
from visualize_report import EditVisualizer

class TestEditVisualizer(unittest.TestCase):
    def setUp(self):
        self.visualizer = EditVisualizer()

    @patch("builtins.open", new_callable=mock_open, read_data="Video analysis report for: dummy_path.mp4\nFrame 1: Potential edit detected (intensity change: 50.0)\nFrame 2: Potential edit detected (intensity change: 60.0)")
    def test_extract_video_info(self, mock_file):
        video_path, edit_points = self.visualizer.extract_video_info("dummy_report.txt")
        self.assertEqual(video_path, "dummy_path.mp4")
        self.assertEqual(edit_points, [(1, 50.0), (2, 60.0)])

    def test_cluster_edit_points(self):
        edit_points = [(1, 50.0), (2, 60.0), (150, 70.0), (300, 80.0)]
        clusters = self.visualizer.cluster_edit_points(edit_points)
        self.assertEqual(len(clusters), 3)
        self.assertEqual(clusters[0], [(1, 50.0), (2, 60.0)])
        self.assertEqual(clusters[1], [(150, 70.0)])
        self.assertEqual(clusters[2], [(300, 80.0)])

    @patch("visualize_report.cv2.VideoCapture")
    @patch("visualize_report.Image.fromarray")
    @patch("visualize_report.os.makedirs")
    def test_create_gif(self, mock_makedirs, mock_fromarray, mock_videocapture):
        mock_cap = MagicMock()
        mock_videocapture.return_value = mock_cap
        mock_cap.isOpened.return_value = True
        mock_frame = np.zeros((100, 100, 3), dtype=np.uint8)
        mock_cap.read.side_effect = [(True, mock_frame)] * 100 + [(False, None)]
        
        cluster = [(10, 50.0), (20, 60.0)]
        gif_path = self.visualizer.create_gif("dummy_path.mp4", cluster)
        
        self.assertTrue(gif_path.endswith(".gif"))
        mock_cap.release.assert_called_once()
        mock_fromarray.assert_called()

if __name__ == '__main__':
    unittest.main()