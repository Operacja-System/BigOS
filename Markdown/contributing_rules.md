### The only allowed language is C99.
### Formatting
- Ensure your code is properly formatted before submitting a pull request (use make format).  
### Naming conventions
- Use snake_case for variable and function names.  
- Use PascalCase for struct and enum names.  
- Use _t at the end of typedef definitions.  
- Use ALL_CAPS for MACROS.  
- Use ALL_CAPS for enums values and include the name of the enum at the beginning of the value name.  
- Prefix global variables with g_.  
- For file names use snake_case.  
### Naming rules
- Don't use magic numbers, use #define or const variables to name them (within reason).  
- Don't use acronyms others then the ones specified [here](acronyms.md).  
### Comments
- If using comment labels please write them like this:
    - `//TODO:` 
    - `//NOTE:`
    - `//WARNING:`
    - `//BUG:`
    - `//PERF:` - for marking code that needs optimization
    - `//FIX:` - for marking code that needs fixing
    - `//HACK:` - for marking a temporary solution that needs redoing
    - `//TEST:` - for marking code used for testing
    (The colon is important)
- Use Doxygen-style comments for functions and structs.  
(Update comments when modifying code!)  
### Git
- No Warnings, TODOs, Bugs, Fixes, Hacks and Tests on the main branch
- Keep the Git history clean.  
- Write meaningful commit messages.  
- All code must be reviewed before merging.  
- Use pull requests, no direct pushes to main.  
- At least 2 approvals required before merging.  
- Code must be tested before merging.  
