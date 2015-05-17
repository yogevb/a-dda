This style description aims to help different parts of `ADDA` look similar. Most of the below "rules" are just conventions, each being one of several equivalent options. Still, adhering to one of such options seems beneficial. This style is based on the current state of the code, which is a result of hard-to-track evolution. Moreover, it may change in the future. The style is rather short, so it is recommended to use existing files as examples.



# C sources and headers #

## File structure ##
Each source file has the following components:
  * Starting comment:
```
File: <filename>
$Date:: #$  
Descr: <short description>
Copyright (C) <years> ADDA contributors
<standard copyright statement>
```
  * `#include` directives. Start with `const.h` (needed for all source files), followed by other project headers, and then system headers.
  * File-scope variable declarations.  First semi-global variables are defined (used in this and one more source file), then - local variables (declared as `static`).
  * External function declarations (for some specific functions which do not have corresponding header files).
  * Function definitions, each preceded by a delimiter `//=====...=====` and with descriptive comment before opening `{`.

Header files follow the same structure (where applicable) and have everything except the starting comment enclosed in guarding construct:
```
#ifndef __<name>_h
#define __<name>_h
...
#endif // __<name>_h
```

## Format ##
  * Indentation is done using tabs only. Tab size = 4. Function bodies and all blocks are indented. Preprocessor directives are also indented inside conditionals, but `#` is kept in the line beginning.
```
#ifdef AAA
#   include "a.h"
#endif
```
  * Opening brace { is on the next line for a function body, and on the same line for everything else.
```
void foo(void)
{
    if (i==1) {
        i++;
    }
}
```
  * White spaces are added around bracketed control statements and before opening `{`, but neither inside brackets nor between function arguments. Spaces in expressions can be used when visually appropriate.
```
if (a==b) {
    x = (fun(a,b)+y)*z + (fun(b,c)+z)*y; 
}
```
  * `else` is placed on a new line, `while` (after `do`) - not. Simple (one-statement) "then" statements (after `if`, `while`, `for`, etc.) are placed on the same line and not enclosed in `{}` (when this is semantically equivalent).
```
if (i==0) return;
else if (j==0) return;
else for (i=0;i<j;i++) printf("%d",i);
```
  * Maximum line width = 120. Wrapped part is indented. Wrapping can be done at (strategic) points between larger constructs if that doesn't increase the total number of lines.
```
if (............................)
    printf(a,b,..................................);
```
  * Multi-line mathematical expressions can be aligned at operands.
```
x = a*(b+c) + b*(a+c)
  + c*(a+b);
```
  * One-line comments are made with `//`. If made on a line with the code, this comment is separated by space from the code. Also, space is inserted between `//` and comment text. All multi-lines comments should be made with `/*...*/`. More specifically,
```
/* line 1
 * line 2
 */
```

Most of the above format settings are available as [configuration file](http://a-dda.googlecode.com/svn/wiki/eclipseCDT_style.xml) to be imported into [Eclipse CDT](http://www.eclipse.org/cdt/).

# Makefiles and other (shell) scripts #
  * Makefiles contain starting comment similar to source files.
  * All comments start with "# " (note the space).
  * Maximum line width = 120, wrapped by `\`.
  * Indentation is done with two spaces (except for action part of the Makefiles where tabs are required).
  * `then` is kept on the same line with `if`.
```
if [ $# -ne 1 ]; then
  exit 1
fi
```