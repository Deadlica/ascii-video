#include <iostream>
#include "videohandler.hpp"

bool fix_input(std::string &video_name);

int main() {

    std::string video_name;
    
    system("clear"); //Will make a function to replace this later

    std::cout << "Video name: ";
    std::getline(std::cin, video_name);
    fix_input(video_name);

    VideoHandler video(video_name);

    video.handle();
    std::cout << std::endl << "Are you ready for the " << video_name.substr(0, video_name.size() - 4) << "? [Y/n] ";
    std::cin.get();
    video.play();
}

bool fix_input(std::string &video_name) {
    if(video_name.size() > 4) {
        if(video_name.substr(video_name.size() - 4, 4) == ".mp4")
            return false;

        video_name += ".mp4";
        return true;
    }
    return false;
}
