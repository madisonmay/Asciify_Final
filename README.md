Asciify
=============
A Software Design Project by Bill, Jacob, Madison, and Mitchell

Installation Guide
------------------

### Step 1 - Run setup.py

Running setup.py from within the asciify directory will automatically
install and set-up many dependencies and prepare parts of asciify to run.

Additionally, it is expected that the user is running python 2.7.

```
python setup.py
```

#### Step 1.1 - Install the required dependencies

NOTE: THIS STEP IS NOT REQUIRED IF YOU SUCCESSFULLY RUN SETUP.PY
Imagemagick, ffmpeg, mencoder, opencv, and gimp are required to run this suite.
To view ascii art video with sound, libavcodec-extra-53 and mpg123 are required.
Konsole is recommended for viewing ascii art video in your terminal.
Execute the following in your terminal to install these dependencies:

```
sudo apt-get install imagemagick ffmpeg gimp mencoder konsole python-opencv python-numpy libavcodec-extra-53 mpg123;
```

This suite also supports processing of youtube videos, which requires the youtube-dl package.

```
sudo apt-get install youtube-dl;
```

#### Step 1.2 - Prepare the python-based files to be executable

NOTE: THIS STEP IS NOT REQUIRED IF YOU SUCCESSFULLY RUN SETUP.PY

```
chmod 755 fast_asciify internal/trifecta internal/asciifile
```
	
### Step 2 - Insert a custom gimp script for batch edge detection

Place edge-detect.scm in /home/{username}/.gimp-{version}/scripts/


### Step 3 - Add this directory to your .bashrc

Open ~/.bashrc with the text editor of your choice and edit your
path to include this directory.

```
export PATH="<path>:$PATH"
```


### User-executed Files

- setup.py - used to install dependencies and grant permission to executables
- asciify - main wrapper for conversion of videos, images, and folders to an edge-detected -ascii art file
- test_images - used to determine the ideal white threshold value for a video
- fast_asciify - wrapper for printing gradient ascii in the terminal

### Internal Files

- textify - handles the majority of processing workload for asciify
- asciifile - handles file and link processing for fast_asciify
- trifecta - handles trifecta mode for fast_asciify webcam processing

### C++ Files

- asciify.cpp - source code for asciify
- textify.cpp - source code for textify
- test_images.cpp - source code for test_images

### Folders

- chars - image files for 94 ascii characters
- internal - internal files called by user-executed files
- documentation - text files mostly for project deliverables

### Text Files

- INSTALL.txt - an installation guide
- README.txt - readme for package
- README.md - readme for github


### The following file types are currently supported:

- .mpeg
- .mp4
- .avi
- .jpg
- .png

Please note that only Linux (Ubuntu) and python 2.7 are currently supported.

### Typical Usage (asciify)

To asciify a image, video, or folder, you need only provide a filename.

```
$ asciify <filename>
```

### Optional Threshold Argument

As part of the edge-detection process, edges of varying brightnesses are produced.
In order to prevent excess noise and in order to produce a black and white image,
all pixels with brightness greater than 7 (a value we have found typically produces
decent results) are converted to white pixels.  You can alter this sensitivity value
by providing an integer as an optional argument via the command line.  For example:

```
$ asciify <filename> [int]
```

To find the ideal threshold value, you can produce a set of 30 test images from a video.
Simply use the following syntax:

```
$ test_images <filename>
```

The resultant images are saved in the folder Test_Images.  Each is saved as it's
threshold value with a .png extension.  You can browse through these images and
select the appropriate threshold for your video.

### Typical Usage (fast_asciify)

To view an asciified webcam feed in the terminal, simply run fast_asciify with 
the desired mode provided (-g, gradient; -l, laplacian; -c, combination (default).

```
$ fast_asciify [mode]
```

In addition, all three modes can be seen simultaniously by giving a mode argument
of -t.

To view an asciified photo or video on file, provide the -f argument and filename.

```
$ fast_asciify -f <filename>
```

To invert the ascii library (default is intended for light text on dark background),
use the -i argument.

```
$ fast_asciify -i ...
```

Use the -h or --help arguments to view available arguments.


### Design Choices

In general, one of two paths is followed during program execution.

The first program path handles pre-existing files. If a video file is fed to our program,
it is first unpacked into image files by ffmpeg and placed into a temporary directory.
Audio is extracted from the original video file, to be joined later with the final video.
Each image is opened, and a series of operations are applied that link a ascii character
to a similarly sized portion of the input image.  In the gradient (fast_asciify) version of
our code, this means the saturation of the original image is matched to a character.
In the slower (asciify) version, gimp applies laplacian edge detection to each image
and produces a second image for each input image.  These edge-detection images are then
fed to our algorithm to be matched with ascii characters.  We handled the asciify version
in C++ to make the processing time required a bit more bearable.

At this point, the text is either displayed in the terminal (as is the case with
fast_asciify, which also uses a less effective, quicker edge detection) or compiled into 
an image (as is done with asciify).  With asciify, these files are then joined into a video 
and the original audio is reapplied.

If an image file or folder of images is fed to our program, this same basic path is
followed -- however, audio extraction is not required.

In the second major execution path, opencv loads images from the webcam and these
are processed without saving intermediate files.  By eliminating file I/O and gimp
edge detection from the execution path, we see a drastic speed increase.



