1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  We use fork/execvp instead of just calling execvp directly to creates a new child process that is an exact copy of the parent process. This allows the shell to continue running in the parent process while the child process executes the new program.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If the fork() system call fails, The implementation uses the perror("fork") function to print an error message to stderr, which includes the string "fork" followed by a colon and the error description associated with the current value of errno.



3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  The execvp() function finds the command to execute through the following process by searching for the executable in the directories listed in the PATH environment variable. PATH is a colon-separated list of directories where the system looks for executable files.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  The purpose of calling wait() in the parent process after forking is important because This ensures that the shell doesn't prompt for the next command until the current command has finished running. If we didn't call it, The shell would have no way to determine if the executed command succeeded or failed.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS() provides the exit status of a child process that has terminated normally. It allows the parent process to determine if the child process completed successfully or encountered an error. By using WEXITSTATUS(), the code ensures it's interpreting the correct portion of the status value, focusing solely on the intentional exit code provided by the child process rather than other termination information.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  The implementation of build_cmd_buff() handles quoted argument by using a flag in_quotes to track whether it's currently parsing within a quoted section. When a double quote (") is encountered, it sets in_quotes to 1 and moves past the quote.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  Handling of quoted arguments: The build_cmd_buff function now includes logic to handle quoted strings. It uses an in_quotes flag to track whether it's parsing within quotes, and preserves spaces within quoted sections as part of a single argument.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals in Linux serve as a mechanism for interprocess communication and system-level notifications in a Linux system. Signals are lightweight and don't require complex data structures or buffers comparing to other forms of interprocess communication (IPC).

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL: Purpose: Immediately terminate a process. Their typical use cas is when a process is unresponsive or needs to be forcefully stopped.
    SIGTERM: Purpose: Request graceful termination of a process. Their typical use case is when default signal sent by the kill command, allowing processes to clean up before exiting.
    SIGINT: Purpose: Interrupt a running process. Their typical use case is when a user presses Ctrl+C in the terminal.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  When a process receives SIGSTOP, The process is immediately suspended. SIGSTOP cannot be caught, blocked, or ignored, unlike SIGINT. The process remains in a stopped state until it receives a SIGCONT signal. SIGSTOP cannot be caught or ignored because it's designed as a non-blockable signal, similar to SIGKILL.