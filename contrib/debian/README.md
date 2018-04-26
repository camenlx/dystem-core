
Debian
====================
This directory contains files used to package dystemd/dystem-qt
for Debian-based Linux systems. If you compile dystemd/dystem-qt yourself, there are some useful files here.

## dystem: URI support ##


dystem-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install dystem-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your dystemqt binary to `/usr/bin`
and the `../../share/pixmaps/dystem128.png` to `/usr/share/pixmaps`

dystem-qt.protocol (KDE)

