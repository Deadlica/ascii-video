#include "videohandler.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <cstdlib>
#include <iostream>

VideoHandler::VideoHandler(std::string video_name)
:video_name(video_name), capture(video_name),
frame_rate(capture.get(cv::CAP_PROP_FPS)), total_frames(capture.get(cv::CAP_PROP_FRAME_COUNT)) {
    if(!capture.isOpened()) {
        std::cerr << "Couldn't open " << video_name << std::endl;
        abort();
    }

    this->video_name.resize(video_name.size() - 4);
    music.openFromFile(this->video_name + ".ogg");
}

void VideoHandler::handle() {
    if(folder_exists()) {
        system("sleep 2");
        system("clear");
        std::cout << "It seems like you already have the frames for this video, would you still like to download them again? [Y/n] ";
        std::string input;
        std::getline(std::cin, input);
        if(input != "Y" && input != "y") {
            return;
        }
    }
        //Convert frames as grayscale images
        download_frames();
        capture.release();

        //Converts the images to .txt files
        convert_frames();
}

bool VideoHandler::folder_exists() {
    struct stat buffer;
    if(!stat(video_name.c_str(), &buffer)) {
        return true;
    }
    return false;
}

void VideoHandler::play() {
    std::string image;
    std::vector<std::string> frames;
    sf::Font monospace;
    monospace.loadFromFile("NotoSansMono-Bold.ttf");
    sf::Text display_frame("", monospace, FONT_SIZE);
    display_frame.setString("@");
    double char_width_ratio = display_frame.getGlobalBounds().height / display_frame.getGlobalBounds().width;
    display_frame.setScale(char_width_ratio, 1);
    display_frame.setFillColor(sf::Color::Black);
    display_frame.setLineSpacing(0.75);
    display_frame.setLetterSpacing(0.75);
    std::cout << display_frame.getGlobalBounds().width << ", " << display_frame.getGlobalBounds().height << std::endl;


    for(int i = 1; i <= total_frames; i++) {
        std::ifstream txt_frame(video_name + "/txt_frames/frame" + std::to_string(i) + ".txt");
        std::stringstream stream;
        stream << txt_frame.rdbuf();
        txt_frame.close();
        frames.push_back(stream.str());
    }

    window.create(sf::VideoMode(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height), video_name, sf::Style::Close);
    //window.setVisible(false);
    window.setFramerateLimit(frame_rate);
    window.setPosition(sf::Vector2i(0, 0));
    
    music.play();
    //window.setVisible(true);
    for(int i = 0; i < frames.size(); i++) {
        window.clear(sf::Color::White);

/*        printf("\r");                             For printing in terminal
        printf("%s", dispay_frame.c_str());         instead of sfml window
        printf("\n");
*/
        sf::Event event;                            //This is to avoid SFML giving the
        while(window.pollEvent(event)) {            //"window is not responding" error message
            if(event.type == sf::Event::Closed) {
                window.close();
            }
        }


        display_frame.setString(frames[i]);
        std::cout << display_frame.getGlobalBounds().height << "x" << display_frame.getGlobalBounds().width << std::endl;
        window.draw(display_frame);
        window.display();
    }
    window.close();
}

void VideoHandler::download_frames() {
    capture >> frame;
    while(!frame.empty()) {

        gray_scale();

        if(frame_number == 1) {
            system("clear");
        }

        std::cout << "\rDownloading frame " << frame_number << "/" << total_frames;

        capture >> frame;
        frame_number++;
    }
    std::cout << std::endl;
}

void VideoHandler::gray_scale() {
    if(frame_number == 1) {
        mkdir(video_name.c_str(), 0777);
        std::string child_folder = video_name + "/frames";
        mkdir(child_folder.c_str(), 0777);
        path = child_folder + "/";
    }

    cv::Mat gray_frame;

    cv::cvtColor(frame, gray_frame, cv::COLOR_RGB2GRAY);
    std::string filename = path + "frame" + std::to_string(frame_number) + ".png";
    cv::imwrite(filename, gray_frame);
}

void VideoHandler::convert_frames() {
    frame_number = 1;
    mkdir((video_name + "/txt_frames").c_str(), 0777);
    while(frame_number <= total_frames) {
        frame = cv::imread(path + "frame" + std::to_string(frame_number) + ".png");
        
        if(frame.empty())
            std::cerr << "Failed to load image!" << std::endl;
        else
            to_txt();
        
        std::cout << "\rConverting frame " << frame_number << "/" << total_frames;
        frame_number++;
    }
}

void VideoHandler::to_txt() {
    using pixel = cv::Vec3b;
    std::string str_frame = "";
    sf::Text dummy_text;

    cv::Mat resized_frame;
    double y = (double)sf::VideoMode::getDesktopMode().height / frame.size().height / (FONT_SIZE + 0.75);
    double x = (double)sf::VideoMode::getDesktopMode().width / frame.size().width / (FONT_SIZE - 1.93);

    cv::resize(frame, resized_frame, cv::Size(), x, y);
    

    for(int y = 0; y < resized_frame.rows; y++) {
        for(int x = 0; x < resized_frame.cols; x++) {
            pixel color = resized_frame.at<pixel>(cv::Point(x, y));

            double color_val = color[0];

            //Accentuate the color values to give better contrast
            if(color_val >= 200) {
                color_val += 35;
                if(color_val > 255)
                    color_val = 255;
            }
            else if(color_val <= 120) {
                color_val -= 40;
                if(color_val < 0)
                    color_val = 0;
            }
            
            str_frame += graphics[std::floor(color_val / 25)];
        }
        str_frame += "\n";
    }
    
    str_frame.resize(str_frame.size() - 1);
    std::string filename = video_name + "/txt_frames/frame" + std::to_string(frame_number) + ".txt";
    std::ofstream file(filename);
    file << str_frame;
    file.close();
}
