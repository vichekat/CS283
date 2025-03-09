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

@test "Basic Command Execution: ls" {
    run ./dsh <<EOF
ls
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="dsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check that output contains dsh3> twice and ends with cmd loop returned 0
    [[ "$stripped_output" =~ "dsh3>dsh3>cmdloopreturned0" ]]
    [ "$status" -eq 0 ]
}

@test "Built-in Command: cd /tmp" {
    run ./dsh <<EOF
cd /tmp
pwd
EOF
    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"

    # Check that pwd outputs /tmp
    [[ "$output" =~ "/tmp" ]]
    [ "$status" -eq 0 ]
}

@test "Single Pipeline: echo Hello | wc -c" {
    run ./dsh <<EOF
echo Hello | wc -c
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="6dsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # More flexible check
    [[ "$stripped_output" =~ "6dsh3>dsh3>cmdloopreturned0" ]]
    [ "$status" -eq 0 ]
}

@test "Test exit command" {
    run bash -c "echo 'exit' | ./dsh"
    [ "$status" -eq 0 ]
    [[ "$output" == *"exiting..."* ]]
}



