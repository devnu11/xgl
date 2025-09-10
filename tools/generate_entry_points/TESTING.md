# Testing Guide for XGL Entry Point Generator

## Overview

The XGL Entry Point Generator includes comprehensive CTest integration with multiple test targets, parallel execution, coverage analysis, and detailed reporting.

## Test Categories

### Unit Tests
Individual module testing with fast execution:
```bash
# Run all unit tests in parallel
make test_generator_unit

# Run specific module tests
ctest -R "entry_generator_test_xml_parser" -V
ctest -R "entry_generator_test_type_mapper" -V
```

### Integration Tests  
Full workflow testing:
```bash
# Run integration tests
make test_generator_integration

# Run complete test suite
ctest -R "entry_generator_test_suite" -V
```

### Code Quality Tests
Style and formatting validation:
```bash
# Run code quality checks
make test_generator_quality

# Individual quality check
ctest -R "entry_generator_code_quality" -V
```

### Coverage Analysis
Test coverage reporting:
```bash
# Run tests with coverage
make test_generator_coverage

# View coverage report
ctest -R "entry_generator_test_with_coverage" -V
```

## CTest Commands

### Basic Usage
```bash
# Run all entry point generator tests
ctest --label-regex "entry_generator" --output-on-failure

# Run tests in parallel (4 jobs)
ctest --label-regex "entry_generator" --parallel 4

# Run with verbose output
ctest --label-regex "entry_generator" --verbose
```

### Advanced Usage
```bash
# Run only failed tests from last run
ctest --rerun-failed --output-on-failure

# Run specific test pattern
ctest -R "entry_generator_test_.*" --output-on-failure

# List all available tests
ctest --show-only=json-v1 | grep -A5 -B5 "entry_generator"
```

### Test Reports
```bash
# Generate XML test report
ctest --label-regex "entry_generator" --output-junit test_results.xml

# Generate dashboard submission (if configured)
ctest -D Experimental
```

## Make Targets

### Primary Targets
- `make test_generator` - Run all tests with parallel execution
- `make test_generator_verbose` - Run all tests with detailed output

### Category Targets
- `make test_generator_unit` - Unit tests only (fast)
- `make test_generator_integration` - Integration tests only  
- `make test_generator_quality` - Code quality checks only
- `make test_generator_coverage` - Coverage analysis

## Test Configuration

### Parallel Execution
Tests run in parallel by default using up to 4 cores or the number of available CPU cores, whichever is smaller.

### Timeouts
- Unit tests: 30 seconds
- Integration tests: 60 seconds  
- Coverage tests: 120 seconds

### Test Labels
Tests are organized with labels for easy filtering:
- `entry_generator` - All generator tests
- `unit_tests` - Individual module tests
- `integration_tests` - Full workflow tests
- `code_quality` - Style and format checks
- `coverage` - Coverage analysis
- `fixtures` - Setup/cleanup tests

### Test Dependencies
- All tests use fixtures for proper setup/cleanup
- Resource locks prevent Python path conflicts
- Validation tests depend on code generation completion

## Output Files

### Test Reports
- `test_reports/test_results.xml` - CTest-compatible XML report
- `coverage.xml` - Python coverage XML report (if coverage.py available)

### Coverage Reports  
- Console coverage summary during test run
- Detailed coverage.xml for CI/CD integration
- Missing line reports for incomplete coverage

## Troubleshooting

### Common Issues
1. **Python module not found**: Check PYTHONPATH environment variable
2. **Test timeout**: Increase timeout in CMakeLists.txt for slow systems
3. **Parallel test failures**: Use `--parallel 1` to run sequentially

### Debug Commands
```bash
# Run single test with maximum verbosity
ctest -R "entry_generator_test_xml_parser" --verbose --output-on-failure

# Check test dependencies
ctest --show-only=json-v1 | jq '.tests[] | select(.name | contains("entry_generator"))'

# Verify test fixture setup
ctest -R "entry_generator_setup_fixture" --verbose
```

## Integration with XGL Build

The test targets integrate seamlessly with the XGL build system:

```bash
# From XGL root directory
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make generate_entry_points
make test_generator
```

Tests automatically run as part of the CI/CD pipeline and provide detailed feedback for code quality and functionality verification.