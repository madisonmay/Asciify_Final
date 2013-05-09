from os import system

dependencies = 'imagemagick ffmpeg gimp mencoder konsole '
dependencies += 'python-opencv python-numpy libavcodec-extra-53 mpg123;'
system('sudo apt-get install ' + dependencies + ' youtube-dl -y;')
system('chmod 755 fast_asciify')
system('chmod 755 internal/trifecta')
system('chmod 755 internal/asciifile')
