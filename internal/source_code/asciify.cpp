
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */
#include <sys/stat.h>
#include <string>
#include <iostream>
#include <istream>
#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include <vector>
#include <regex>
using namespace std;
//g++ -o asciify asciify.cpp -std=c++0x
string directory;
string fps;
//retrieves the current working directory
string getcwd_string() {
   char buff[PATH_MAX];
   getcwd( buff, PATH_MAX );
   std::string cwd( buff );
   return cwd;
}

//Checks if a file exists given a name
bool fexists(const char *filename)
{
  ifstream ifile(filename);
  return ifile;
}

//Checks if a folder exists given a name
bool folder_exists(const char *foldername) {
  DIR* dir = opendir(foldername);
  if (dir) {
    return true;
    closedir(dir);
  } else {
    return false;
  }
}

void create_directory(string input_file, bool output) {
  //make required directories
  if (output) {
    cout << "Creating temporary directories..." << endl;
  }

  system("mkdir \'.temp_ascii\' > /dev/null 2> /dev/null");

  //move the input file to the in directory
  string command = "cp " + input_file + " \'.temp_ascii\' > /dev/null 2> /dev/null";
  system(command.c_str());

  //cd in
  chdir(".temp_ascii");
}

void extract_audio(string input_file) {
  //grab audio from video;
  cout << "Extracting audio from video file..." << endl;
  string command = "ffmpeg -i " + input_file + " -ab 128k -ac 2"
            " -strict experimental -ar 44100 -vn sound.mp4  > /dev/null 2> /dev/null";
  system(command.c_str());
}

void detect_framerate(string input_file) {
  cout << "Detecting Frame Rate... ";
  string command = "ffmpeg -i " + input_file + " 2>&1 | grep -o '[.0-9]\\{1,5\\}\\sfps' | grep -o '[.0-9]\\{1,5\\}' > fps.txt";
  system(command.c_str());

  ifstream fpstxt("fps.txt");
  if(fpstxt.is_open()){
    getline(fpstxt, fps);
    fpstxt.close();
    cout << "Detected framerate: " <<fps.c_str()<<endl;
    if(fps.length() == 0){
      fps = "29.98";
      cout << "None detected, setting to 29.98" << endl;
    }
  }
  else{
    fps = "29.98";
    cout << "None detected, setting to 29.98" << endl;
  }
}

void split_frames(string input_file) {
    //split video into frames
  cout << "Splitting video into frames..." << endl;
  string command = "ffmpeg -i " + input_file + " -strict experimental -r " + fps + " %8d.png > /dev/null 2> /dev/null";
  system(command.c_str());
}

void split_frames(string input_file, int seconds){
  string begin_second = static_cast<ostringstream*>( &(ostringstream() << seconds) )->str();
  int end = seconds + 30;
  string end_second = static_cast<ostringstream*>( &(ostringstream() << end) )->str();
  cout << "Getting frames in range [" + begin_second + ", " + end_second + "]..." << endl;
  string command = "ffmpeg -ss " + begin_second + " -t 30 -i " + input_file + " -strict experimental -r " + fps + " %8d.png > /dev/null 2> /dev/null";
  system(command.c_str());
}

void convert_to_png(string input_file) {
    //convert to png and prepare to use gimp edge-detection
    string command = "convert " + input_file + " " + "00000001.png";
    system(command.c_str());
}

void edge_detect_directory(int threshold, bool output) {
  //7 and 255 are thresholds for gimp to use
  if (output) {
    cout << "Producing edge-detected images..." << endl;
  }
  string thresh = static_cast<ostringstream*>( &(ostringstream() << threshold) )->str();
  string command = "gimp -i -b '(edge-batch \"*.png\" " + thresh +
         " 255)' -b '(gimp-quit 0)' > /dev/null 2> /dev/null";
  system(command.c_str());
}

std::string get_selfpath() {
    char buff[1024];
    ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
    if (len != -1) {
      buff[len] = '\0';
      return std::string(buff);
    } else {
     /* handle error condition */
    }
}

void asciify_directory(bool output) {
  //convert to ascii art frames -- both edges and greyscale
  chdir("..");
  if (output) {
    cout << "Converting edge-detected images to ascii art..." << endl;
    string directory = get_selfpath();
    string command = directory.substr(0, directory.length() - 8) + "/internal/textify";
    system(command.c_str());
    cout << "\n";
  } else {
    string directory = get_selfpath();
    string command = directory + "/internal/textify > /dev/null 2> /dev/null";
    system(command.c_str());
  }
  chdir(".temp_ascii");
}

void rejoin(string filename) {
  //rejoin images
  cout << "Rejoining images..." << endl;
  string command = "mencoder mf://*.jpg -mf w=1280:h=720:fps="+ fps +":type=jpg -ovc "
         "lavc -lavcopts vcodec=mpeg4:mbd=2:trell -oac copy -o video.mp4 > /dev/null 2> /dev/null";
  system(command.c_str());

  //rejoin audio and video
  cout << "Rejoining video and audio..." << endl;
  command = "ffmpeg -y -i sound.mp4 -i video.mp4 "
         "-acodec copy -vcodec copy " + filename + "_ascii.mp4 > /dev/null 2> /dev/null";
  system(command.c_str());
}

void join_part(bool first=false){
  if(!first){
    cout << "Joining video portions..." << endl;
    string command = "mencoder mf://*.jpg -mf w=1280:h=720:fps="+ fps +":type=jpg -ovc "
           "lavc -lavcopts vcodec=mpeg4:mbd=2:trell -oac copy -o part.mpg > /dev/null 2> /dev/null";
    system(command.c_str());

    //Concatenates
    command = "cat final.mpg part.mpg > temp.mpg";
    system(command.c_str());
    command = "rm final.mpg";
    system(command.c_str());
    command = "mv temp.mpg final.mpg";
    system(command.c_str());}
  else{
    cout << "Joining video portions..." << endl;
    string command = "mencoder mf://*.jpg -mf w=1280:h=720:fps="+ fps +":type=jpg -ovc "
           "lavc -lavcopts vcodec=mpeg4:mbd=2:trell -oac copy -o final.mpg > /dev/null 2> /dev/null";
    system(command.c_str());
  }
}

void clean_up(bool output) {

  chdir("..");
  //clean up the directories
  if (output) {
    cout << "Removing temporary directories..." << endl;
  }

  system("rm -r .temp_ascii 2> /dev/null");
}

void clean_folder() {
  system("rm *.png *.jpg");
  //chdir("..");
}

void finalize_video(string filename){
  string command = "ffmpeg -i final.mpg soundless.mp4 > /dev/null 2> /dev/null";
  system(command.c_str());
  cout << "Rejoining video and audio..." << endl;
  command = "ffmpeg -y -i sound.mp4 -i soundless.mp4 "
         "-acodec copy -vcodec copy ../" + filename + "_ascii.mp4 > /dev/null 2> /dev/null";
  system(command.c_str());
}

void process_video(string input_file, int threshold) {

  int length = input_file.length();
  string filename = input_file.substr(0, length - 4);
  string ext = input_file.substr(length - 3);

  detect_framerate(input_file);
  extract_audio(input_file);
  split_frames(input_file);
  edge_detect_directory(threshold, true);
  asciify_directory(true);
  //system("mv ./in/sound.mp4 ./out > /dev/null 2> /dev/null");
  //chdir(".temp_ascii");
  rejoin(filename);

  //move file from inner directory
  string command = "mv " + filename + "_ascii.mp4 ../ > /dev/null 2> /dev/null";
  system(command.c_str());

}


void process_video_by_part(string input_file, int threshold) {

  int length = input_file.length();
  string filename = input_file.substr(0, length - 4);
  string ext = input_file.substr(length - 3);
  detect_framerate(input_file);
  extract_audio(input_file);
  int seconds = 0;
  split_frames(input_file, seconds);

  while(fexists("00000001.png")){
    edge_detect_directory(threshold, true);
    asciify_directory(true);
    join_part(seconds == 0);
    clean_folder();
    seconds+=30;
    split_frames(input_file, seconds);
    //chdir("..");
  }
  finalize_video(filename);
  //move file from inner directory
  //string command = "mv " + filename + "_ascii.mp4 ../ > /dev/null 2> /dev/null";
  //system(command.c_str());

}

void process_image(string input_file, int threshold, bool output) {

  int length = input_file.length();
  string filename = input_file.substr(0, length - 4);
  string ext = input_file.substr(length - 3);
  string jpg = "jpg";

  convert_to_png(input_file);

  edge_detect_directory(threshold, output);
  asciify_directory(output); //Brings to upper directory

  //chdir(".temp_ascii");

  //move image file to original directory
  string command = "mv 00000001.jpg ../" + filename + "_ascii.jpg > /dev/null 2> /dev/null";
  system(command.c_str());
}

vector<string> getdir () {
    DIR *dp;
    vector<string> files;
    struct dirent *dirp;
    if((dp  = opendir(".")) == NULL) {
        cout << "Error opening directory" << endl;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return files;
}

void process_directory(string input_file, int threshold) {
  //Processes an entire directory of .jpgs or .pngs

  string jpg = "jpg";
  string png = "png";

  cout << "Processing directory..." << endl;
  chdir(input_file.c_str());

  vector<string> files = getdir();
  int count = 0;

  //Individually convert each file
  for (unsigned int i = 0;i < files.size();i++) {
    try {
      string image_file = files[i];
      int length = image_file.length();
      string ext = image_file.substr(length-3);
      if ((ext == png) || (ext == jpg)) {
        create_directory(image_file, false);
        process_image(image_file, threshold, false);
        clean_up(false);
        count ++;
        cout <<"\rConverted [" << count << "] files to ascii...";
      }
    } catch (...) {}
  }
  cout << endl;

}


//wrapper for asciify
int main (int argc, char* argv[])
{

  string input_file;
  string command;
  int threshold;

  //get filename
  if (argc > 1) {
    input_file = argv[1];
  } else {
    cout << "Please provide the name of the file to process as a second argument" << endl;
    exit(0);
  }

  if (!fexists(input_file.c_str()) && !folder_exists(input_file.c_str())) {
    cout << "The given file or folder does not exist.  Please try again..." << endl;
    exit(0);
  }

  //check for optional argument of threshold, default to 7
  if (argc > 2) {
      if (atoi(argv[2]) > 0) {
        threshold = atoi(argv[2]);
      } else {
        threshold = 7;
      }
  } else {
      threshold = 7;
  }

  //used later for naming save file
  int length = input_file.length();
  string filename = input_file.substr(0, length - 4);
  string ext = input_file.substr(length - 3);
  string png = "png";
  string jpg = "jpg";
  string is_folder = input_file.substr(length-4, 1);
  string dot = ".";

  if (is_folder != dot) {
    process_directory(input_file, threshold);
  } else {
    create_directory(input_file, true);

    //set filetypes for later use
    if ((ext == png) || (ext == jpg)) {
      process_image(input_file, threshold, true);
    } else {
      process_video_by_part(input_file, threshold);
    }
  }


  clean_up(true);

  cout << "Asciification complete!" << endl;
  return 0;
}
