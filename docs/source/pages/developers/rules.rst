==================
Contributing Rules
==================

:Author: Oskar Meler
:Date: 23-02-2026
:Revision: 1

These rules exist to keep the BigOS codebase consistent and readable, and to create clear expectations about code quality.
Any changes not following these rules may not be merged. Revisions to these rules may be made; in such cases, the entire codebase must be adjusted.

--------
Language
--------

The core systems (kernel, bootloader, and basic drivers) must be written in C23.
Assembly is allowed where necessary, however inline assembly is preferred.
Future revisions may allow C++ in non-core infrastructure, provided strict standards are enforced to ensure readability and maintainability.

----------
Formatting
----------

All code must follow clang-format rules. To format your code run `./scripts/format.sh`.

------------------
Naming conventions
------------------

* Use `snake_case` for names, including file names.
* Use `ALL_CAPS` for MACROS.
* Use `ALL_CAPS` for `enum` values and include the name of the `enum` at the beginning of the value name.
* Use `ALL_CAPS` for constant values.
* Append `_t` to type definitions.

--------------
Best practices
--------------

* Names should clearly explain content and purpose.
* Functions should not modify anything they do not take as an argument.
* Avoid using assignments inside conditional statements.
* Avoid using `goto`; its use is allowed, however it must be justified.
* Functions acting as “methods” for structs should start with the struct name and take the struct instance as the first argument.

    .. code-block:: c
        :caption: Example

        dynamic_array_push(dynamic_array_t darr, int value)

* Functions that can fail must explicitly return an error.
* Only :doc:`allowed abbreviations </pages/bigos/general/abbreviations>` may be used:

    * For argument names, if the type makes it clear.

    .. code-block:: c
        :caption: Example

        foo(flags_t f, dynamic_array_t darr)

    * For `enum` field names to abbreviate the `enum` type name.

    .. code-block:: c
        :caption: Example

        typedef enum {
            AF_READ,
            AF_WRITE
        } access_flag_t;

* Prefer `constexpr` where possible; if not, use `const`. Avoid macros for variables.
* Every variable that can be `const`, should.
* Readability should be prioritized unless performance gains are significant.

--------
Comments
--------

- Use Doxygen-style comments for functions and structs.
- Comments explaining *how* the code works are allowed, but only when the code cannot be written in a self-explanatory manner.
  **Update comments when modifying code!**

-------------
Documentation
-------------

*To be defined...*

---
Git
---

* Keep the Git history clean.
* Write meaningful commit messages.
* Each commit should represent one logical change.
* Commit messages must be capitalized and written in past tense.
* For complex commits, include a comment explaining *why* and *how* a change was made.
* Use pull requests, no direct pushes to `main`.
* Be thorough when performing code reviews. Check readability, correctness, adherence to style, and maintainability.
* Pull requests require at least one review before merging.
