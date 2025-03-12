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

@test "Basic local command execution" {
    echo "ls" | ./dsh -c &
    sleep 1
    SERVER_PID=$!
    run ./dsh -s
    kill $SERVER_PID
    [ "$status" -eq 0 ]
    [[ "$output" =~ "dsh_cli.c" ]]
}

@test "Pipe commands" {
    echo "ls | grep .c" | ./dsh -c &
    sleep 1
    SERVER_PID=$!
    run ./dsh -s
    kill $SERVER_PID
    [ "$status" -eq 0 ]
    [[ "$output" =~ "dsh_cli.c" ]]
}

@test "Built-in cd command" {
    (echo "cd /tmp"; echo "pwd"; echo "exit") | ./dsh -c &
    sleep 1
    SERVER_PID=$!
    run ./dsh -s
    kill $SERVER_PID
    [ "$status" -eq 0 ]
    [[ "$output" =~ "/tmp" ]]
}

@test "Exit command" {
    echo "exit" | ./dsh -c &
    sleep 1
    SERVER_PID=$!
    run ./dsh -s
    [ "$status" -eq 0 ]
    [[ "$output" =~ "client exited" ]]
}

@test "Stop-server command" {
    echo "stop-server" | ./dsh -c
    run ./dsh -s
    [ "$status" -eq 0 ]
    [[ "$output" =~ "client requested server to stop" ]]
}

@test "Multiple clients" {
    ./dsh -s -p 5678 &
    SERVER_PID=$!
    sleep 1
    
    # First client
    echo "echo Client 1" | ./dsh -c -p 5678 &
    CLIENT1_PID=$!
    
    # Second client
    echo "echo Client 2" | ./dsh -c -p 5678 &
    CLIENT2_PID=$!
    
    sleep 2
    kill $SERVER_PID
    [ -n "$SERVER_PID" ]
}

@test "Custom port and interface" {
    ./dsh -s -i 127.0.0.1 -p 6789 &
    SERVER_PID=$!
    sleep 1
    
    run echo "ls" | ./dsh -c -i 127.0.0.1 -p 6789
    
    kill $SERVER_PID
    [ "$status" -eq 0 ]
    [[ "$output" =~ "dsh_cli.c" ]]
}
