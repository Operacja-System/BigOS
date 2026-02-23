Vision
======

**Big Operating System** is an operating system developed as part of a project at the University of Wrocław.
Its primary goal is to make operating system development accessible and easier to understand.
The project aims to provide a platform for education and experimentation with system components that,
in most existing operating systems, are too deeply integrated to be easily modified.

---------------
Design Goals
---------------

BigOS is designed to fulfill two primary objectives.

First, it serves as a comprehensive learning platform for individuals seeking to deepen their understanding of operating system design,
with a particular emphasis on practical implementation.
It is intended for those who find traditional resources insufficient, whether due to their focus on high-level theoretical overviews or their reliance on minimal “Hello World”-style systems that omit essential subsystems and frequently depend on convenient but unmaintainable design decisions.

Second, BigOS provides a platform for structured experimentation.
This includes exploration of alternative kernel architectures, custom hardware integrations, and higher-level system software, like browsers and the web stack.
The project intentionally encourages work on the foundational aspects of modern computing infrastructure,
such as developing a custom RISC-V ISA implementation (potentially with custom extensions) or designing an entirely custom network stack.

---------------
Implementation Philosophy
---------------

BigOS is developed with intelligibility and modifiability as core principles.

To support these goals, the project enforces strict contribution standards.
All code must be simple, readable, and thoroughly documented. Documentation assumes only a solid understanding of the C programming language as prior knowledge.
Everything else is explained in detail, including both their operational behavior and the rationale behind them.

---------------
Standards and Stability
---------------

BigOS does not commit to strict adherence to external standards unless doing so meaningfully advances the project’s goals.
Standards may be adopted where appropriate, particularly in areas such as the boot process, to ensure compatibility with real hardware.

The project does not guarantee long-term stability of APIs or internal interfaces.
Design decisions may be revised if substantially better solutions are identified. While such changes are made with careful consideration,
flexibility is prioritized over premature standardization.

:Author: Oskar Meler
:Date: 23-02-2026
