#!/usr/bin/env python3
"""Validation script to compare generated code with existing patterns."""

import re
from pathlib import Path
from typing import List, Dict, Set


class CodeValidator:
    """Validates generated code against existing XGL patterns."""
    
    def __init__(self, existing_dir: Path, generated_dir: Path):
        self.existing_dir = existing_dir
        self.generated_dir = generated_dir
        self.validation_results: Dict[str, List[str]] = {}
    
    def validate(self) -> bool:
        """Run all validations."""
        self._validate_function_signatures()
        self._validate_code_patterns()
        self._validate_file_structure()
        
        return self._print_results()
    
    def _validate_function_signatures(self) -> None:
        """Validate function signatures match."""
        existing_sigs = self._extract_signatures(self.existing_dir)
        generated_sigs = self._extract_signatures(self.generated_dir)
        
        issues = []
        
        for func_name in existing_sigs:
            if func_name not in generated_sigs:
                issues.append(f"Missing function: {func_name}")
            elif existing_sigs[func_name] != generated_sigs[func_name]:
                issues.append(f"Signature mismatch for {func_name}")
        
        self.validation_results["Function Signatures"] = issues
    
    def _validate_code_patterns(self) -> None:
        """Validate code patterns match XGL style."""
        generated_files = list(self.generated_dir.glob("*.cpp"))
        issues = []
        
        for file_path in generated_files:
            content = file_path.read_text()
            
            # Check required patterns
            if "ApiDevice::ObjectFromHandle" not in content and "VkDevice" in content:
                issues.append(f"{file_path.name}: Missing ObjectFromHandle pattern")
            
            if "namespace vk" not in content:
                issues.append(f"{file_path.name}: Missing vk namespace")
            
            if "VKAPI_ATTR" not in content:
                issues.append(f"{file_path.name}: Missing VKAPI_ATTR")
            
            # Check copyright header
            if "Advanced Micro Devices" not in content:
                issues.append(f"{file_path.name}: Missing AMD copyright")
        
        self.validation_results["Code Patterns"] = issues
    
    def _validate_file_structure(self) -> None:
        """Validate file structure matches expectations."""
        expected_files = {
            "entry_vk_device.cpp", "entry_vk_instance.cpp", 
            "entry_vk_buffer.cpp", "entry_vk_image.cpp"
        }
        
        generated_files = {f.name for f in self.generated_dir.glob("*.cpp")}
        issues = []
        
        for expected in expected_files:
            if expected not in generated_files:
                issues.append(f"Missing expected file: {expected}")
        
        self.validation_results["File Structure"] = issues
    
    def _extract_signatures(self, directory: Path) -> Dict[str, str]:
        """Extract function signatures from C++ files."""
        signatures = {}
        
        for cpp_file in directory.glob("*.cpp"):
            content = cpp_file.read_text()
            
            # Find VKAPI function signatures
            pattern = r'VKAPI_ATTR\s+(\w+)\s+VKAPI_CALL\s+(\w+)\s*\([^)]*\)'
            matches = re.findall(pattern, content, re.MULTILINE | re.DOTALL)
            
            for return_type, func_name in matches:
                signatures[func_name] = f"{return_type} {func_name}"
        
        return signatures
    
    def _print_results(self) -> bool:
        """Print validation results."""
        total_issues = sum(len(issues) for issues in self.validation_results.values())
        
        print(f"\\n=== XGL Entry Point Validation Results ===")
        print(f"Total Issues Found: {total_issues}\\n")
        
        for category, issues in self.validation_results.items():
            print(f"{category}:")
            if not issues:
                print("  ✅ All checks passed")
            else:
                for issue in issues:
                    print(f"  ❌ {issue}")
            print()
        
        return total_issues == 0


def main():
    """Main validation entry point."""
    existing_dir = Path("xgl/icd/api/entry")
    generated_dir = Path("generated/entry_points")  # Adjust as needed
    
    if not existing_dir.exists():
        print(f"Existing entry directory not found: {existing_dir}")
        return False
    
    if not generated_dir.exists():
        print(f"Generated directory not found: {generated_dir}")
        print("Run the generator first!")
        return False
    
    validator = CodeValidator(existing_dir, generated_dir)
    return validator.validate()


if __name__ == "__main__":
    success = main()
    exit(0 if success else 1)