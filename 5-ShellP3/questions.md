1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

To ensure all child processes complete before the shell continues accepting user input, the implementation should use the waitpid() system call for each forked child process. For each child process forked, the parent shell should call waitpid() with the child's PID. The waitpid() call blocks the parent process until the specified child process terminates. If waitpid() is not called for all child processes, The shell might display the next prompt before all commands in the pipeline have finished executing.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

Closing unused pipe ends after calling dup2() is crucial for proper pipe functionality and resource management because it prevents resource from leaksing. Pipes consume kernel memory. If unused file descriptors are not closed, they continue to occupy system resources unnecessarily. In a pipeline, if a process keeps both ends of a pipe open, it can prevent other processes from detecting the end of data transmission

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

The primary reason cd is a built-in command is that it needs to change the current working directory of the shell process itself. An external command runs in a separate process and cannot directly modify the parent shell's environment. Executing cd as an external command would introduce unnecessary overhead, including process creation and destruction, for a simple operation.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

To modify the shell implementation to allow an arbitrary number of piped commands while maintaining efficient memory allocation, we can use a linked list or dynamically resizable array to store command structures. We also need to allocate memory for each new command as it's parsed from the input line. Allowing arbitrary pipes increases flexibility but adds complexity to the code.

