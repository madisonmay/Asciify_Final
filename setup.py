from os import system

dependencies = 'imagemagick ffmpeg gimp mencoder konsole '
dependencies += 'python-opencv python-numpy libavcodec-extra-53 mpg123;'
system('sudo apt-get install '+dependencies)
system('sudo apt-get install youtube-dl;')
system('chmod 755 fast_asciify WebCam/trifecta')