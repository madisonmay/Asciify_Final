
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */
#include <string>
#include <iostream>
#include <istream>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <unistd.h>
#include <vector>
#include <regex>
using namespace std;
//g++ -o asciify wrapper_functioned_framebased.cpp -std=c++0x
string directory;
string fps;
double framerate;
string width;
string height;
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

void create_directory(string input_file, bool output) {
  //make required directories
  cout << "Creating temporary directories..." << endl;
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
  string command = "ffmpeg -i " + input_file + " 2>&1 | grep -o '[.0-9]\\{1,5\\}\\stbr' | grep -o '[.0-9]\\{1,5\\}' > fps.txt"; 
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
  framerate = atof(fps.c_str());
  //cout<<framerate<<endl;
}

void detect_resolution(string input_file){
  cout<< "Detecting Video Resolution... ";
  string command = "ffmpeg -i " + input_file + " 2>&1 | grep -o '\\s[0-9]\\{3,4\\}x[0-9]\\{3\\},' > res.txt";
  system(command.c_str());
  ifstream restxt("res.txt");
  if(restxt.is_open()){
    string res;
    getline(restxt,res);
    restxt.close();
    if(res.length() == 0){
      res = " 1280x640,";
    }
    //res = res.substr(1, res.length() - 1);
    int l = res.length();
    cout << "Detected resolution: " << res.c_str()<<endl;
    width = res.substr(1, l - 6);
    height = res.substr(l - 4, 3);
    cout<<res<<" "<<width.c_str()<<" "<<height.c_str()<<endl;
  }
}

void split_frames(string input_file) {
    //split video into frames
  cout << "Splitting video into frames..." << endl;
  string command = "ffmpeg -i " + input_file + " -strict experimental -r " + fps + " %8d.png > /dev/null 2> /dev/null";
  system(command.c_str());
}

void split_frames(string input_file, double seconds, double sec_step){
  string begin_second = static_cast<ostringstream*>( &(ostringstream() << seconds) )->str();
  double end = seconds + sec_step;
  string end_second = static_cast<ostringstream*>( &(ostringstream() << end) )->str();
  string step = static_cast<ostringstream*>( &(ostringstream() << sec_step) )->str();
  cout << "Getting frames in range [" + begin_second + ", " + end_second + "]..." << endl;
  string command = "ffmpeg -ss " + begin_second + " -t " +step+ " -i " + input_file + " -strict experimental -r " + fps + " %8d.png > /dev/null 2> /dev/null";
  system(command.c_str());
}

void convert_to_png(string input_file) {
    //convert to png and prepare to use gimp edge-detection
    string command = "convert " + input_file + " " + "00000001.png";
    system(command.c_str());
}

void edge_detect_directory(int threshold) {
  //7 and 255 are thresholds for gimp to use
  cout << "Producing edge-detected images..." << endl;
  string thresh = static_cast<ostringstream*>( &(ostringstream() << threshold) )->str();
  string command = "gimp -i -b '(edge-batch \"*.png\" " + thresh +
         " 255)' -b '(gimp-quit 0)' > /dev/null 2> /dev/null";
  system(command.c_str());
}

void asciify_directory() {
  //convert to ascii art frames -- both edges and greyscale
  chdir("..");
  cout << "Converting edge-detected images to ascii art..." << endl;
  system("textify");
  cout << "\n";
  chdir(".temp_ascii");
}

void rejoin(string filename) {
  //rejoin images
  cout << "Rejoining images..." << endl;
  string command = "mencoder mf://*.jpg -mf w="+width+":h="+height+":fps="+ fps +":type=jpg -ovc "
         "lavc -lavcopts vcodec=mpeg4:mbd=2:trell -oac copy -o video.mp4 > /dev/null 2> /dev/null";
  system(command.c_str());

  //rejoin audio and video
  cout << "Rejoining video and audio..." << endl;
  command = "ffmpeg -i sound.mp4 -i video.mp4 "
         "-acodec copy -vcodec copy " + filename + "_ascii.mp4 > /dev/null 2> /dev/null";
  system(command.c_str());
}

void join_part(){
  //If the file exists, appends the new portion to the end of it
  if(fexists("final.mpg")){
    cout << "Joining video portions..." << endl;
    string command = "mencoder mf://*.jpg -mf w="+width+":h="+height+":fps="+ fps +":type=jpg -ovc "
           "lavc -lavcopts vcodec=mpeg4:mbd=2:trell -oac copy -o part.mpg > /dev/null 2> /dev/null";
    system(command.c_str());
  
    //Concatenates 
    command = "cat final.mpg part.mpg > temp.mpg";
    system(command.c_str());
    command = "rm final.mpg";
    system(command.c_str());
    command = "mv temp.mpg final.mpg";
    system(command.c_str());}
  //Otherwise makes the first file
  else{
    cout << "Initializing Video File..." << endl;
    string command = "mencoder mf://*.jpg -mf w="+width+":h="+height+":fps="+ fps +":type=jpg -ovc "
           "lavc -lavcopts vcodec=mpeg4:mbd=2:trell -oac copy -o final.mpg > /dev/null 2> /dev/null";
    system(command.c_str());
  }
}

void clean_up() {
  chdir("..");
  //clean up the directories
  cout << "Removing temporary directories..." << endl;
  system("rm -r .temp_ascii");
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
  edge_detect_directory(threshold);
  asciify_directory();
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
  detect_resolution(input_file);
  double sec_step = 1000.0 / framerate;
  cout<<"The video will be processed in steps of "<<sec_step<<"s"<<endl;
  if(!fexists(".temp_ascii/sound.mp4")){
    extract_audio(input_file);}
//  if(fexists(".temp_ascii/final.mpg")){
//    seconds = 
//  }
  double seconds = 0;
  split_frames(input_file, seconds, sec_step);

  while(fexists("00000001.png")){
    edge_detect_directory(threshold);
    asciify_directory();
    join_part();
    clean_folder();
    seconds+=sec_step;
    split_frames(input_file, seconds, sec_step);
    //chdir("..");
  }
  finalize_video(filename);
  //move file from inner directory
  //string command = "mv " + filename + "_ascii.mp4 ../ > /dev/null 2> /dev/null";
  //system(command.c_str());

}

void process_image(string input_file, int threshold) {

  int length = input_file.length();
  string filename = input_file.substr(0, length - 4);
  string ext = input_file.substr(length - 3);
  string jpg = "jpg";

  if (ext == jpg) {
    convert_to_png(input_file);
  };

  edge_detect_directory(threshold);
  asciify_directory(); //Brings to upper directory

  //chdir(".temp_ascii");

  //move image file to original directory
  string command = "mv 00000001.jpg ../" + filename + "_ascii." + ext + " > /dev/null 2> /dev/null";
  system(command.c_str());
}

//wrapper for asciify
int main (int argc, char* argv[])
{

  string input_file;
  string command;
  int threshold;

  //get current working directory
  directory = getcwd_string();

  //get filename
  if (argc > 1) {
    input_file = argv[1];
  } else {
    cout << "Please provide the name of the file to process as a second argument";
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

  create_directory(input_file);

  //set filetypes for later use
  if ((ext == png) || (ext == jpg)) {
    process_image(input_file, threshold);
  } else {
    process_video_by_part(input_file, threshold);
  }

  clean_up();

  cout << "Asciification complete!" << endl;
  return 0;
}
