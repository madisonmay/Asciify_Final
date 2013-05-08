
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */
#include <string>
#include <iostream>
#include <istream>
#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include <vector>
#include <regex>
using namespace std;


int main (int argc, char* argv[])
{
    string video;
    string command;

    //get filename
    if (argc > 1) {
        video = argv[1];
    } else {
        cout << "Please provide the name of the file to process as a second argument";
    }

    command = "ffmpeg -ss 00:00:05 -i " + video + " test.png > /dev/null 2> /dev/null";
    system(command.c_str());

    system("mkdir Test_Images");
    system("mv test.png ./Test_Images");

    chdir("Test_Images");
    for (int i=1; i<=30; i=i++){
        command = "cp test.png " + to_string(i) + ".png";
        system(command.c_str());
        command = "gimp -i -b '(edge-batch \"" + to_string(i) + ".png\" " +
               to_string(i) + " 255)' -b '(gimp-quit 0)' > /dev/null 2> /dev/null";
        system(command.c_str());
    }

    cout << "Test images generated..." << endl;
}
