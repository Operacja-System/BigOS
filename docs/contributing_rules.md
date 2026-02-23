# Contributing rules

## Languages

To keep things simple the only allowed language is C23 and assembly where needed.

## License

All source files should start with the following license header:

```text
<one line to give the program's name and a brief idea of what it does.>
Copyright (C) <year> <name of author>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
```

## Formatting

Ensure your code is properly formatted before submitting a pull request (use `scripts/format.sh`).  

## Naming conventions

- Use `snake_case` for names (including file names).
- Use `_t` at the end of typedef definitions.
- Use `ALL_CAPS` for MACROS.
- Use `ALL_CAPS` for enum values and include the name of the enum at the beginning of the value name.
- Prefix global variables with `g_`.

### Naming rules

- Don't use magic numbers, use #define or const variables to name them (within reason).  
- Don't use acronyms others then the ones specified [here](acronyms.md).  

## Comments

- If using comment labels please write them like this:
    - `//TODO:`
    - `//NOTE:`
    - `//WARNING:`
    - `//BUG:`
    - `//PERF:` – for marking code that needs optimization
    - `//FIX:` – for marking code that needs fixing
    - `//HACK:` – for marking a temporary solution that needs redoing
    - `//TEST:` – for marking code used for testing
- Use Doxygen-style comments for functions and structs.  
  **Update comments when modifying code!**

> The colon after labeled comments is important for `TODO`-searching scripts

## Git

- No Warnings, TODOs, Bugs, Fixes, Hacks and Tests on the main branch.
- Keep the Git history clean.
- Write meaningful commit messages.
- All code must be reviewed before merging.
- Use pull requests, no direct pushes to main.
- Code must pass all checks before merging.
- Code must be tested before merging.
- At least 1 approval required before merging.
