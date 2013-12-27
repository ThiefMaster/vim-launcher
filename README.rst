Vim Launcher
============

On Windows, Vim installs its binaries to ``$VIM\vim74``. This is extremely annoying when you assign
some file types to open with Vim. And while that can be somewhat easily fixed by searching for the
old Vim path using *RegEdit*, you might have configured Vim to be launched in some other programs,
too.

TL;DR: Executable paths that change with each update are annoying on Windows.

My program gets rid of the problem by launching the newest (based on the number in the ``vimXY``
folder) Vim.


Usage
-----

Download ``vim-launcher.exe``, rename it to ``gvim.exe`` and copy it right your ``$VIM`` folder. If
you want a the same thing for the classic CLI-based Vim (does anyone use that on Windows?!), do the
same thing but name the file ``vim.exe``.


License
-------

Copyright © 2013 Adrian Mönnich (adrian@planetcoding.net). All code is released under the WTFPL
License, see `LICENSE`_ for details. The Vim icon is subject to the Vim license.

This program is free software. It comes without any warranty, to the extent permitted by applicable
law. You can redistribute it and/or modify it under the terms of the WTFPL, Version 2, as published
by Sam Hocevar. See `LICENSE`_ and/or http://www.wtfpl.net for more details.

.. _LICENSE: https://github.com/ThiefMaster/vim-launcher/blob/master/LICENSE
