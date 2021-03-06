# rotate-plugin
[![AUR version](https://img.shields.io/aur/version/avisynth-plugin-rotate-git?style=flat-square)](https://aur.archlinux.org/packages/avisynth-plugin-rotate-git/)

Rotation plugin for AviSynth+

This is a version of [this plugin](http://www.avisynth.nl/users/fizick/rotate/rotate.html)
that I have slightly rewritten and made compatible with AviSynth+. I also applied the changes made by StainlessS [here](https://forum.doom9.org/showthread.php?p=1916888#post1916888).

## Optimizations
It is still missing MMX/SSE2 optimizations. I tried converting the MMX assembly code
into C++ with SSE2 intrinsics but so far this made the code either crash or produce
garbage output. If you happen to have experience with this stuff and would be
interested in helping out with this, feel free to take a look.