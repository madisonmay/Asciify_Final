AsciiPy: A Software Design Project by Bill, Jacob, Madison, and Mitchell
------------------------------------------------------------------------

User-executed Files
-------------------
setup.py - used to install dependencies and grant permission to executables
asciify - main wrapper for conversion of videos, images, and folders to an edge-detected ascii art file
test_images - used to determine the ideal white threshold value for a video
fast_asciify - wrapper for printing gradient ascii in the terminal

Internal Files
--------------
textify - handles the majority of processing workload for asciify
asciifile - handles file and link processing for fast_asciify
trifecta - handles trifecta mode for fast_asciify webcam processing

C++ Files
---------
asciify.cpp - source code for asciify
textify.cpp - source code for textify
test_images.cpp - source code for test_images

Folders
-------
chars - image files for 94 ascii characters
internal - internal files called by user-executed files
documentation - text files mostly for project deliverables

Text Files
----------
INSTALL.txt - an installation guide
README.txt - readme for package
README.md - readme for github



