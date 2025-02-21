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

@test "cd command with no arguments" {
  run ./dsh < <(echo cd)
  [ "$status" -eq 0 ]
}

@test "cd command with one argument" {
  run ./dsh < <(echo "cd /tmp")
  [ "$status" -eq 0 ]
}

@test "external command execution" {
  run ./dsh < <(echo "uname")
  [ "$status" -eq 0 ]
}

@test "quoted string handling" {
  run ./dsh < <(echo 'echo "hello,      world"')
  [ "$status" -eq 0 ]
}

@test "multiple space handling" {
  run ./dsh < <(echo 'echo   test')
  [ "$status" -eq 0 ]
}