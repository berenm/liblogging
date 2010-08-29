===========================================
liblogging utility library. 
===========================================
-------------------------------------------
nothing very interesting...
-------------------------------------------

Just a small wrapper around boost log v2, to initialize things and to add nice ANSI color to the output.
Provides a few macros to be used in other programs and libraries.
This may evolve in the future to allow automatic detection of ANSI capability (to avoid logging strange chars to files),
and to switch to another logging lib, for example.