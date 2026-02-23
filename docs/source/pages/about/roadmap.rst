Roadmap
=======

This roadmap outlines the planned development direction of BigOS. It reflects current priorities and may evolve as the project matures.

=============
Current state
=============

BigOS is currently in the very early stages of design and development.
There are no fully functioning subsystems yet, and major architectural decisions haven't been made.

----------------------------------------------------------
Phase 0 - Development Environment & Minimal Working Kernel
----------------------------------------------------------

* Build system
* Define a boot protocol
* Implement a bootloader capable of booting in at least a virtualized environment
* Minimal kernel capable of:

    * Initializing virtual memory
    * Handling interupts

------------------------------------
Phase 1 - Core kernel infrastructure
------------------------------------

- Logging framework
- Virtual file system interface
- System calls interface

---------------------------
Phase 2 - User mode support
---------------------------

- Process model
- Scheduler
- Processes management (spawning, ELF loading, termination, etc.)

--------------------------------
Phase 3 - Minimal interactive OS
--------------------------------

- Essential drivers (framebuffer, filesystems, USB)
- Temporary shell providing basic utilities

------------------------------
Phase 4 - Structured userspace
------------------------------

- Unified rendering interface
- Rendering service implementation
- Dynamic libraries
- User accounts

===========
Version 1.0
===========

Once the above functionality is implemented, the project may be designated as BigOS 1.0.
However, this declaration may be postponed, as we aim to avoid introducing breaking changes to core system APIs beyond this point.
Further development will continue after 1.0.0, including additional implementations and refinements of core subsystems, such as the permissions model, but these changes should not fundamentally alter established interfaces.

--------------------
Ideas for the future
--------------------

- Capability-based security model
- Package manager
- Network stack
- Adding support for ARM32, ARM64 and RV32
- Making BigOS concurrent
- Custom executable format
- Custom file system
- Support for MMU-less hardware

:Author: Oskar Meler
:Date: 23-02-2026
