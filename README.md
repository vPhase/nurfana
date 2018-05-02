** nurfana (NU RF ANAlysis) **

Yet another radio neutrino analyzer, this time for the phased array. 

Written in C++11. Requires a recent compiler, `ROOT6`, `GSL 2+`, and `fftw3`.
To read in nuphase data you need `nuphaseroot`. 
To read in ARA data you need `AraEvent`

Incorporates cubature as a git submodule (since you're unlikely to have it).  

"Never ever ever rewrite software from scratch" - someone smarter than me. 

Configure by editing m.config (this file does not exist by and will be copied
from m.config.example the first time). I probably should have used CMake, but I really hate it. 


