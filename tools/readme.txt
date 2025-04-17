*** For Windows developers only ***

For those of you developing under Windows, the SHL script comes with a flaw.
The line end characters of text files often result in 0x0d 0x0a aka CR (carriage return)
and LF (line feed), so two characters while e.g. Linux only uses LF.

Note: meson does work together with MSYS2 under Windows, but the issue with the CRLF
flaw is still there.

I.e. when you let the SHL script produce the single chess.hpp file and you want to
upload it to github, this can result in strange behaviour in the code change display.
Github thinks every line of code has changed.

To adress this issue, there is a little tool called RemoveCR.exe which simply removes all
CR characters in the file include/chess.hpp.

For convinience, there is also a bat file (Windows), that does the whole SHL production
and CR removal with a simple double click on that file.

