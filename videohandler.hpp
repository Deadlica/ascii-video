#ifndef VIDEOHANDLER_H
#define VIDEOHANDLER_H

#include <opencv2/opencv.hpp>
#include <sys/stat.h>
#include <vector>
#include <fstream>
#include <chrono>
#include <sstream>
#include <thread>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <filesystem>

class VideoHandler {
public:
    VideoHandler(std::string video_name);

    void handle();
    void play();

private:
    bool folder_exists();
    void download_frames();
    void gray_scale();
    void convert_frames();
    void to_txt();
    void resize_window(int x, int y);

    std::string video_name;
    cv::VideoCapture capture;
    int frame_rate, total_frames, frame_number = 1;
    cv::Mat frame;
    std::string path;
    std::vector<char> graphics = {'@', '#', 'S', '%', '?', '*', '+', ';', ':', ',', ' '};
    sf::RenderWindow window;
    sf::Music music;    
    const int FONT_SIZE = 12;
};

#endif
