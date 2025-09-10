#!/usr/bin/env python3
"""Format generated code using black (if available)."""

import subprocess
import sys
from pathlib import Path


def format_python_files():
    """Format Python files with black."""
    try:
        # Check if black is available
        subprocess.run(["black", "--version"], 
                      capture_output=True, check=True)
        
        # Format all Python files
        python_files = list(Path(".").glob("*.py"))
        python_files.extend(Path("tests").glob("*.py"))
        
        for py_file in python_files:
            print(f"Formatting {py_file}")
            subprocess.run(["black", str(py_file)], check=True)
        
        print("✅ Python formatting complete")
        return True
        
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("⚠️  Black not available, skipping Python formatting")
        return False


def check_code_quality():
    """Run basic code quality checks."""
    issues = []
    
    # Check all Python files
    python_files = list(Path(".").glob("*.py"))
    python_files.extend(Path("tests").glob("*.py"))
    
    for py_file in python_files:
        content = py_file.read_text()
        lines = content.split('\\n')
        
        for i, line in enumerate(lines, 1):
            # Check line length (PEP 8: max 88 chars with black)
            if len(line) > 88:
                issues.append(f"{py_file}:{i} - Line too long ({len(line)} chars)")
            
            # Check for tabs
            if '\\t' in line:
                issues.append(f"{py_file}:{i} - Uses tabs instead of spaces")
    
    if issues:
        print("\\n❌ Code quality issues found:")
        for issue in issues[:10]:  # Show first 10 issues
            print(f"  {issue}")
        if len(issues) > 10:
            print(f"  ... and {len(issues) - 10} more issues")
        return False
    else:
        print("✅ Code quality checks passed")
        return True


def main():
    """Main formatting entry point."""
    print("=== XGL Entry Point Generator - Code Formatting ===\\n")
    
    # Format Python code
    format_success = format_python_files()
    
    # Check code quality
    quality_success = check_code_quality()
    
    # Summary
    print("\\n=== Formatting Summary ===")
    if format_success and quality_success:
        print("✅ All code formatting and quality checks passed")
        return True
    else:
        print("⚠️  Some issues found - see output above")
        return False


if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)