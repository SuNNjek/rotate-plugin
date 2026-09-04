# rotate-plugin
[![AUR version](https://img.shields.io/aur/version/avisynth-plugin-rotate-git?style=flat-square)](https://aur.archlinux.org/packages/avisynth-plugin-rotate-git/)

Rotation plugin for AviSynth+

This is a version of [this plugin](http://www.avisynth.nl/users/fizick/rotate/rotate.html)
that I have slightly rewritten and made compatible with AviSynth+. I also applied the changes made by StainlessS [here](https://forum.doom9.org/showthread.php?p=1916888#post1916888).

## Installation

### Windows

This plugin requires installing the Visual C++ distributable runtime. You can get the latest version here:
https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170#latest-supported-redistributable-version

- Head to the releases and download the latest version.
- Place the DLL for your architecture (if you don't know what that means, it's probably x64) in your AviSynth plugin folder

### Linux

#### Arch
If you're on Arch, there's an AUR package (check the badge at the top of the README).

#### NixOS
If you're on NixOS, the repository contains a flake to build the plugin.

#### Other (Compiling from source)
For other distributions, you will have to compile the plugin from source.

You need to have GCC and cmake installed for this. If you have, just run this in your terminal:
```shell
cmake .
cmake --build .
cmake --install .
```

## Optimizations
It is still missing MMX/SSE2 optimizations. I tried converting the MMX assembly code
into C++ with SSE2 intrinsics but so far this made the code either crash or produce
garbage output. If you happen to have experience with this stuff and would be
interested in helping out with this, feel free to take a look.