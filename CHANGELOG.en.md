# Operating System Version 2.0  
*by TheCreatorOfClearCode*

---

## 🚨 System Crash Screen
- Triggered when filesystem mounting fails.
- Activated on errors in the Initialization System.

---

## 📁 File System and Basic File/Directory Commands
Introduced a custom filesystem supporting the following commands:

- `mkdir="directory_name_or_path"` — create a directory.
- `rmdir="directory_name_or_path"` — remove a directory.
- `mkfile="file_name_or_path"` — create a file.
- `rmfile="file_name_or_path"` — delete a file.
- `write="file_path"->"text"` — write text to a file.
- `read="file_path"` — read contents of a file.
- `copy="source_path"->"destination_path"` — copy a file or directory.
- `move="source_path"->"destination_path"` — move a file or directory.
- `cd="path"` — change directory.
- `cd..` — move up one directory level.
- `pwd` — display the current directory path.
- `ls` — list contents of the current directory.
- `ls="path"` — list contents of the specified directory.

**Additional feature:**  
Multiple commands separated by `|`, for example:  
`mkdir=docs|images|bin` — creates three directories at once.

---

## 💻 Updated Command Line Interface
- Added command history.
- New command: `clear-history` — clears the input history.

---

## 🌄 Loading Screen
- Displays during OS startup (after Initialization system starts).
- Displays during shutdown and reboot.

---

## 📃 Logging Improvements
- ✅ **InitSystem** no longer logs to the console — logs are saved to `Root/Logs/Initialization_System.log`.
- ✅ **Kernel** logs are saved to `Root/Logs/Kernel.log`.
- 📂 The `Root/Logs` directory is automatically created if it doesn't exist.

---

## 🔍 Log Viewing Module
Supports paginated viewing with color-coded log types:

- `logs-kernel` — view kernel logs.
- `logs-initialization_system` — view initialization system logs.
- `logs-kernel-clear` — clear kernel logs.
- `logs-initialization_system-clear` — clear initialization system logs.
- `logs-clear` — clear all logs.

---

## ❔ Enhanced `help` Command Interface
- Navigate pages with left/right arrow keys.
- Supports multiple modules with `&`, for example:  
  `help=CLI&Logs`

---

## 📦 Enhanced `installed-modules` Interface
- Supports paginated output.
- Displays total count of installed modules.

---

## ➕ Multiple Command Execution
- Use `&&` to execute multiple commands sequentially.  
  Example:  
  `mkdir=demo&&cd=demo` — creates a directory `demo` and changes into it.

---