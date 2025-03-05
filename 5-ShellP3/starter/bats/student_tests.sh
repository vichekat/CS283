#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Test exit command" {
    run bash -c "echo 'exit' | ./dsh"
    [ "$status" -eq 0 ]
    [[ "$output" == *"exiting..."* ]]
}

