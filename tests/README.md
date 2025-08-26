# Unit Tests for KineToxin

This directory contains unit tests for the KineToxin project components.

## Test Structure

- `test_crypto.cpp` - Tests for cryptographic utilities
- `test_logger.cpp` - Tests for logging system
- `test_config.cpp` - Tests for configuration management
- `test_error.cpp` - Tests for error handling
- `test_location.cpp` - Tests for location tracking
- `test_vm_detection.cpp` - Tests for VM detection
- `test_main.cpp` - Main test runner

## Running Tests

To build and run tests:

```bash
mkdir build
cd build
cmake .. -DBUILD_TESTS=ON
make
./tests/KineToxinTests
```

## Test Coverage

The goal is to maintain >80% code coverage across all modules.

## Test Guidelines

1. Each module should have comprehensive unit tests
2. Tests should cover both success and failure scenarios
3. Mock external dependencies where appropriate
4. Use descriptive test names that explain what is being tested
5. Keep tests independent and repeatable

## Adding New Tests

When adding new functionality:

1. Create corresponding test file if it doesn't exist
2. Add test cases for new functions/methods
3. Update this README with any new test files
4. Ensure tests pass before committing code
