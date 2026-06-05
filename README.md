## keybinds reference


| Keybind | Function |
| :--- | :--- |
| **`q`** | Exit program |
| **`p`** | Toggle metadata permissions column (`drwxrwxrwx`) |
| **`Arrow Up`** | Move selection cursor up |
| **`Arrow Down`** | Move selection cursor down |
| **`Enter`** / **`Arrow Right`** | Open selected directory |
| **`u`** / **`Arrow Left`** | Go back to parent directory (`cd ..`) |
| **`c`** | Copy file (source, destination) |
| **`d`** | Delete file |
| **`m`** | Move or rename file |

## dependencies

### 1. ncurses
used for the entire terminal rendering engine
```bash
sudo apt install libncurses5-dev libncursesw5-dev
```

### 2. feh
used to display images (externally)
```bash
sudo apt install feh
```

