"""Unit tests for main module."""

import unittest
from unittest.mock import patch, MagicMock
from pathlib import Path
from tempfile import TemporaryDirectory
import sys

# Add the parent directory to the path for imports
sys.path.insert(0, str(Path(__file__).parent.parent))

import main


class TestMainFunction(unittest.TestCase):
    """Test cases for main function."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.temp_dir = TemporaryDirectory()
        self.temp_path = Path(self.temp_dir.name)
        
        # Create a dummy XML file
        self.xml_file = self.temp_path / 'vk.xml'
        self.xml_file.write_text('<?xml version="1.0"?><registry></registry>')
        
        self.output_dir = self.temp_path / 'output'
    
    def tearDown(self):
        """Clean up test fixtures."""
        self.temp_dir.cleanup()
    
    @patch('main.VulkanRegistryParser')
    @patch('main.EntryPointGenerator')
    @patch('sys.argv')
    def test_main_with_required_args(self, mock_argv, mock_generator_class, mock_parser_class):
        """Test main function with required arguments."""
        # Mock command line arguments
        mock_argv.__getitem__.side_effect = [
            'main.py',  # Program name
            '--xml-path', str(self.xml_file),
            '--output-dir', str(self.output_dir)
        ]
        mock_argv.__len__.return_value = 5
        
        # Mock the classes
        mock_parser = MagicMock()
        mock_generator = MagicMock()
        mock_parser_class.return_value = mock_parser
        mock_generator_class.return_value = mock_generator
        
        # Call main
        main.main()
        
        # Verify parser was created with correct path
        mock_parser_class.assert_called_once_with(self.xml_file)
        
        # Verify generator was created with parser and output dir
        mock_generator_class.assert_called_once_with(mock_parser, self.output_dir)
        
        # Verify generate was called
        mock_generator.generate.assert_called_once()
    
    @patch('main.VulkanRegistryParser')
    @patch('main.EntryPointGenerator')
    @patch('sys.argv')
    def test_main_with_verbose_flag(self, mock_argv, mock_generator_class, mock_parser_class):
        """Test main function with verbose flag."""
        # Mock command line arguments with verbose
        mock_argv.__getitem__.side_effect = [
            'main.py',
            '--xml-path', str(self.xml_file),
            '--output-dir', str(self.output_dir),
            '--verbose'
        ]
        mock_argv.__len__.return_value = 6
        
        # Mock the classes
        mock_parser = MagicMock()
        mock_generator = MagicMock()
        mock_parser_class.return_value = mock_parser
        mock_generator_class.return_value = mock_generator
        
        # Call main
        main.main()
        
        # Verify verbose was enabled
        mock_generator.enable_verbose.assert_called_once()
    
    @patch('sys.argv')
    def test_main_with_missing_xml_file(self, mock_argv):
        """Test main function with missing XML file."""
        # Mock command line arguments with non-existent XML file
        non_existent_xml = self.temp_path / 'missing.xml'
        mock_argv.__getitem__.side_effect = [
            'main.py',
            '--xml-path', str(non_existent_xml),
            '--output-dir', str(self.output_dir)
        ]
        mock_argv.__len__.return_value = 5
        
        # Should raise FileNotFoundError
        with self.assertRaises(FileNotFoundError):
            main.main()
    
    @patch('main.VulkanRegistryParser')
    @patch('main.EntryPointGenerator')  
    @patch('sys.argv')
    def test_output_directory_creation(self, mock_argv, mock_generator_class, mock_parser_class):
        """Test that output directory is created if it doesn't exist."""
        # Use non-existent output directory
        non_existent_output = self.temp_path / 'new_output_dir'
        
        mock_argv.__getitem__.side_effect = [
            'main.py',
            '--xml-path', str(self.xml_file),
            '--output-dir', str(non_existent_output)
        ]
        mock_argv.__len__.return_value = 5
        
        # Mock the classes
        mock_parser = MagicMock()
        mock_generator = MagicMock()
        mock_parser_class.return_value = mock_parser
        mock_generator_class.return_value = mock_generator
        
        # Call main
        main.main()
        
        # Verify directory was created
        self.assertTrue(non_existent_output.exists())
        self.assertTrue(non_existent_output.is_dir())


class TestArgumentParsing(unittest.TestCase):
    """Test cases for argument parsing."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.temp_dir = TemporaryDirectory()
        self.temp_path = Path(self.temp_dir.name)
        self.xml_file = self.temp_path / 'vk.xml'
        self.xml_file.write_text('<?xml version="1.0"?><registry></registry>')
    
    def tearDown(self):
        """Clean up test fixtures."""
        self.temp_dir.cleanup()
    
    @patch('sys.argv', ['main.py', '--help'])
    @patch('sys.exit')
    def test_help_argument(self, mock_exit):
        """Test help argument parsing."""
        with patch('builtins.print'):  # Suppress help output
            try:
                main.main()
            except SystemExit:
                pass  # argparse calls sys.exit after showing help
    
    @patch('sys.argv', ['main.py'])
    def test_missing_required_arguments(self):
        """Test behavior with missing required arguments."""
        with self.assertRaises(SystemExit):
            main.main()
    
    @patch('main.VulkanRegistryParser')
    @patch('main.EntryPointGenerator')
    @patch('sys.argv')
    def test_all_arguments_parsed_correctly(self, mock_argv, mock_generator_class, mock_parser_class):
        """Test that all arguments are parsed correctly."""
        output_dir = self.temp_path / 'output'
        
        mock_argv.__getitem__.side_effect = [
            'main.py',
            '--xml-path', str(self.xml_file),
            '--output-dir', str(output_dir),
            '--verbose'
        ]
        mock_argv.__len__.return_value = 6
        
        mock_parser = MagicMock()
        mock_generator = MagicMock()
        mock_parser_class.return_value = mock_parser
        mock_generator_class.return_value = mock_generator
        
        main.main()
        
        # Verify correct arguments were used
        mock_parser_class.assert_called_once_with(self.xml_file)
        mock_generator_class.assert_called_once_with(mock_parser, output_dir)
        mock_generator.enable_verbose.assert_called_once()
        mock_generator.generate.assert_called_once()


if __name__ == '__main__':
    unittest.main()