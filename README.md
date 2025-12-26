# ICE-x86_64-32

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

ICE is a hobbyist operating system for x86_64 architecture, designed with modularity, multitasking, and modern filesystem support. The project is under active development, featuring a reliable TTY terminal and an experimental graphical interface called Frost.

---

## Features

- **Filesystem:** EXT2  
- **Boot Interface:**  
  - TTY Terminal: Fully functional and stable  
  - GUI "Frost": Experimental; supports apps but may be unstable  
- **Memory Management:** Paging, heap allocation  
- **Task Scheduling:** Preemptive multitasking  
- **Drivers & Networking:** Basic drivers included; networking will first appear in GUI once stable, later integrated with TTY  
- **Development Tools:** Buildable on Linux with `nasm`, `qemu`, and `grub`

---

## Development Status

| Component        | Status                                                                |
|------------------|-----------------------------------------------------------------------|
| TTY Terminal     | Stable and fully functional                                           |
| Frost GUI        | Experimental; may break                                               |
| EXT2 Filesystem  | Fully integrated                                                      |
| Networking       | Planned for GUI first; TTY integration later                          |
| GUI Apps         | Will be prioritized once GUI stabilizes; later ported to TTY          |

---

## Build & Run

1. **Clone the repository (SSH recommended):**  
   ```bash
   git clone git@github.com:Definetly-a-username/ICE-X86_64-32.git
   cd ICE-X86_64-32

    Build the OS:

make

Run in QEMU:

make run

Create bootable ISO:

    make iso

Contribution

Contributions are welcome. Open issues or submit pull requests for bug fixes, features, or documentation improvements.
License

This project is licensed under the GPL-3.0 License
.

