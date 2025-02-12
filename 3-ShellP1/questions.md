1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  fgets() is a good choice for getting user input in the main while loop of this shell implementation because fgets() reads input until it encounters a newline character or reaches the end of the file. This behavior is ideal for a command-line interface where users typically enter commands one line at a time.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  It is a waste if we use fixed-size arrays especially when the size is big or an buffering overflow is the size is small. malloc() can handle commands of any lengths.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Trimming leading and trailing spaces from each command in the build_cmd_list() function is necessary because by removing extraneous spaces, the shell can interpret commands consistently regardless of how users type them.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  Here are three redirection examples we should implement in our custom shell:
    Output redirection (>): Redirects stdout to a file, e.g., ls > file.txt
    Challenge: Ensuring the file is properly opened, written to, and closed.
    Input redirection (<): Takes input from a file instead of stdin, e.g., sort < unsorted.txt
    Challenge: Reading from the file correctly and passing its contents as input to the command.
    Error redirection (2>): Redirects stderr to a file, e.g., grep foo bar 2> errors.log
    Challenge: Separating stderr from stdout and redirecting it to the specified file.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  The key differences between redirection and piping are: redirection sends output to or takes input from files, while piping connects the output of one command to the input of another.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  Keeping STDERR and STDOUT separate in a shell is important because separating error messages allows for easier error detection and handling in scripts and programs. Also, users can redirect STDOUT and STDERR independently, allowing for more control over output management.


- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  Our custom shell should handle errors from failed commands by displaying STDERR by default. Error messages should be displayed on the console to alert the user of issues.