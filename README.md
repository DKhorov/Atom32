
<div align="center">
  <img src="https://www.atomglide.com/4.JPG" alt="Atom32 Logo" width="200" style="border-radius: 10px;"/>
  <h3>Atom32 Logo "Lora Atom"</h3>
</div>

<br/>

# AtomGlide Atom32 OS®

[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen.svg)](#)
[![Architecture](https://img.shields.io/badge/Architecture-x86_32bit-blue.svg)](#)
[![License](https://img.shields.io/badge/License-Proprietary-red.svg)](#)
[![Developer](https://img.shields.io/badge/Developer-AtomGlide_Labs-darkblue.svg)](#)

Atom32 is an independent, 32-bit bare-metal operating system engineered entirely from scratch. Developed strictly in x86 Assembly and pure C, it operates completely independent of standard POSIX environments or standard C libraries (libc). The system features a custom-built Hardware Abstraction Layer (HAL), proprietary memory management, and a deterministic graphical user interface (GUI) driven by the native WinBSI API. 

Designed for industrial, scientific, and educational research, Atom32 provides absolute hardware control, predictable execution, and a zero-overhead environment.

---

### • System Architecture: How It Works

Atom32 operates exclusively in Ring 0 (Kernel Mode), providing direct and unrestricted access to CPU registers, physical memory, and hardware ports. 
* **Boot Process:** The OS is multiboot-compliant. Upon execution, the bootloader transitions the CPU from Real Mode to 32-bit Protected Mode, configures the Global Descriptor Table (GDT), and establishes the Interrupt Descriptor Table (IDT).
* **Graphics Subsystem:** The system utilizes a linear framebuffer provided by VESA BIOS Extensions (VBE). Rendering is handled by a custom 2D graphics engine utilizing double-buffering techniques to prevent visual tearing.
* **WinBSI API:** A proprietary windowing and event-management system that handles UI primitives, typography, mouse state tracking, and keyboard interrupts deterministically.

---

### • Installation and Booting

**Acquiring the Image**
This repository does not contain raw build scripts (makefiles or bash execution scripts). To test or deploy the operating system, you must download the pre-compiled `.iso` image directly from the **GitHub Releases** section of this repository.

**Creating a Bootable USB Drive**
1. Download the `atomglide.iso` file from the Releases page.
2. Utilize imaging software (e.g., Rufus) to write the image to a USB flash drive.
3. For comprehensive, step-by-step instructions on properly flashing the drive and preparing boot sectors, refer strictly to the official documentation at: **atomglide.com/lib**.

---

### • System Requirements and Hardware Compatibility

**Supported Processors (CPU)**
Atom32 requires an x86 architecture processor. It fully supports legacy 32-bit processors and modern 64-bit processors (Intel and AMD) operating in 32-bit Protected Mode. Supported families include Intel Pentium, Core 2 Duo, Core i3/i5/i7 (up to generations supporting legacy boot), and AMD Athlon/Ryzen processors with CSM enabled.

**Random Access Memory (RAM)**
* Minimum required: 128 MB
* Recommended: 512 MB to 4 GB
* Note: Due to the 32-bit architecture limits, the maximum addressable physical memory is 4 GB.

**Supported Monitors and Displays**
The system is compatible with any monitor or built-in display panel that supports VESA BIOS Extensions (VBE) standards. It scales reliably on aspect ratios from standard 4:3 up to 16:9 widescreen resolutions (e.g., 1920x1080).

**Available System Drivers**
* **Video:** Universal VBE/VESA Linear Framebuffer Driver.
* **Input:** PS/2 Keyboard Controller and PS/2 Mouse Driver.
* **Audio:** Programmable Interval Timer (PIT) PC Speaker Driver (Capable of Frequency-Shift Keying for Data-over-Sound transmission).
* **Storage:** Basic ATA (PIO mode) driver integration.

---

### • Deployment Instructions

**Deploying on a Desktop PC**
To run Atom32 natively on a desktop workstation, you must adjust motherboard settings:
1. Access the BIOS/UEFI configuration utility during system startup.
2. Locate the "Secure Boot" parameter and set it to **Disabled**.
3. Locate the boot mode configuration and set it to **Legacy Boot** or enable the **CSM (Compatibility Support Module)**.
4. Modify the Boot Priority to boot from the USB drive first.

**Deploying on a Laptop**
Laptop configurations require identical BIOS/UEFI adjustments. Ensure that "Secure Boot" is strictly disabled and "Legacy OS" support is enabled. Some modern laptops may lack PS/2 emulation for touchpads; an external USB mouse operating in legacy PS/2 emulation mode may be required depending on the motherboard logic.

**Supported Lenovo ThinkPad Hardware**
Atom32 has been extensively developed, compiled, and tested on Lenovo ThinkPad hardware. The OS provides excellent compatibility with the following specific models:
* **X-Series:** X200, X201, X220, X230, X240, X250, X260, X270, X280
* **T-Series:** T400, T410, T420, T430, T440, T450, T460, T470, T480, T520, T530
* **W/P-Series:** W520, W530, W540, P50, P51
* **L/E-Series:** L430, L440, E420, E430
* ThinkPad X1 Carbon Gen 1-6

---

### • Virtualization Environments

If you prefer not to deploy on bare metal, Atom32 runs efficiently in virtualized environments.

**QEMU Execution**
Run the following command in your terminal to initialize the OS with audio support:
qemu-system-i386 -cdrom atomglide.iso -m 512 -vga std -machine pc,pcspk-audiodev=audio0 -audiodev pa,id=audio0



**Oracle VirtualBox**

1. Create a new Virtual Machine. Set Type to "Other" and Version to "Other/Unknown (32-bit)".
2. Allocate a minimum of 512 MB of RAM.
3. In System settings, ensure that **"Enable EFI (special OSes only)" is UNCHECKED**.
4. In Storage settings, mount the `atomglide.iso` file to the optical drive and start the machine.

---

### • Project Infrastructure and Communications

**Official Resources**

* Main Project Hub (Drivers, Documentation, API): [atomglide.com/atom32](https://www.google.com/search?q=https://atomglide.com/atom32&utm_source=gemini)
* Official News and Articles: [atomglide.com/journal](https://www.google.com/search?q=https://atomglide.com/journal&utm_source=gemini)
* GitHub Repository: [https://github.com/DKhorov/Atom32.git](https://github.com/DKhorov/Atom32.git?utm_source=gemini)

**Social Networks and Communities**

* Atom32 OS Community Channel: [https://t.me/AtomGlideOS](https://t.me/AtomGlideOS?utm_source=gemini)
* Developer Technical Channel: [https://t.me/dkdevelop](https://www.google.com/search?q=https://t.me/dkdevelop&utm_source=gemini)
* AtomGlide Corporate Channel: [https://t.me/AtomGlide](https://t.me/AtomGlide?utm_source=gemini)
* Instagram: [@atomglide](https://www.google.com/search?q=https://instagram.com/atomglide&utm_source=gemini)
* TikTok: [@jpegweb](https://www.google.com/search?q=https://tiktok.com/%2540jpegweb&utm_source=gemini)

---

### • Corporate Information

**Project Initiation Date:** September 6, 2026

**Ownership:** A proprietary project developed and maintained by **AtomGlide Labs©**

**Author and Chief Developer:**
**Dmitry Khorov** (Founder and CEO, AtomGlide Labs)

* Email: toktybclassic@gmail.com
* Telegram: @jpegweb
* AtomGlide Network: @jpegweb

© 2026 AtomGlide Labs. All rights reserved. Registered trademarks and service marks are the property of their respective owners.
