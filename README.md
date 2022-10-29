# File Explorer
### Terminal based file explorer, with a basic feature set.
The application would start in the Normal mode, which is the default mode and can be used to explore the current directory and navigate around in the filesystem.
There is another mode called Command Mode in which you can do basic file(directories too) operations like move, rename, delete, etc.
The root of the application will be the directory where the application was
started.
The last line of the display screen shows the status bar(normal mode/command mode).
##
### Normal Mode:
1. The file explorer reads and displays list of files and directories in the current folder-
    1. It shows each file(directories too) in the directory. The following attributes are mentioned for each file:
        1. Ownership (User & Group) & Permissions
        2. File size (in bytes)
        3. Last modified
        4. File name
    2. The application handles scrolling (vertical overflow) in case the directory has a lot of files.
    3. User can navigate up & down the file list using corresponding arrow keys.

2. Open files & directories-
    1. When 'enter' key is pressed-
        1. Directory - It navigates into the directory and
        show the files & directories inside it as specified in point 1.
        2. Files - The application will open files using the
        corresponding default application.

3. Navigation:
    1. Go back: On pressing left arrow key the explorer goes back to
       the previously visited directory if any.
    2. Go forward: On pressing right arrow key the explorer goes
       forward to the next directory (Similar to forward/back feature of web
       pages).
    3. Up one level: On pressing backspace it goes one level up in the directory structure.
    4. Home: On pressing ‘h’ key, it goes to the home folder (the folder where
       application was started).

### Command Mode:
* The application enters the command mode whenever the “:” (colon) key is pressed. Upon entering the command mode the user is able to enter different commands. All commands appear in a bottom status bar.
* **Note**: You can enter relative and absolute paths both for a file(or directory). Note that the root directory will be the directory where you run the application, so the path '/' means that directory.
    
        Command list:
        1. Move: ‘move <source_file(s)> <destination_directory>’
            1. Ex: move foo.txt bar.txt ~/foobar
            2. You can also move directories
        
        2. Copy: ‘copy <source_file(s)> <destination_directory>’
            1. Ex: copy foo.txt bar.txt foobar
            2. You can also copy directories
        
        3. Search: ‘search <filename>’
            1. Search for the given filename under the current directory recursively
            2. Output is the absolute paths which is shown in the normal mode
            
        4. Rename: ‘rename <old_filename> <new_filename>’
            Ex: rename foo.txt bar.txt

        5. Create file: ‘create_file <file_name> <destination_path>’
           Create directory: ‘create_dir <dir_name> <destination_path>’
            Ex: create_file foo.txt ~/foobar
            Ex: create_file foo.txt .
            Ex: create_dir folder_name ~/foobar

        6. Delete file: ‘delete_file <file_path>’
           Delete directory: ‘delete_dir <directory_path>’
            Ex: delete_file superbad.mp4
            Ex: delete_directory ~/Downloads/Project

        7. Goto: 'goto <location>'
            Ex: goto ~/Documents

        8. Snapshotting the filesystem and dumping into a file: ‘snapshot <folder> <dumpfile>’
            Given a base directory this command recursively crawls the
            directory and stores the output in the dumpfile.

        9. On pressing ‘ESC’ key the application goes back to Normal Mode.
        
        10. On pressing ‘q’ key the application closes.
## Technologies used
* The code is completely written in C++ with most of libraries being of C language.
* No OS commands are used to implement command mode functions. Commands like ls, cp, mv, mkdir aren't used.
* Also 'ncurses' library isn't used in the code.
