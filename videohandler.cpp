#include "videohandler.hpp"

VideoHandler::VideoHandler(std::string video_name)
:video_name(video_name), capture(video_name),
frame_rate(capture.get(cv::CAP_PROP_FPS)), total_frames(capture.get(cv::CAP_PROP_FRAME_COUNT)),
window(sf::VideoMode(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height), video_name, sf::Style::Close) {
    if(!capture.isOpened()) {
        std::cerr << "Couldn't open " << video_name << std::endl;
        abort();
    }
    window.setFramerateLimit(frame_rate);
    window.setPosition(sf::Vector2i(0, 0));
    window.setVisible(false);

    this->video_name.resize(video_name.size() - 4);
    music.openFromFile(this->video_name + ".ogg");
}

void VideoHandler::handle() {
    //Convert frames as grayscale images
    download_frames();
    capture.release();

    //Converts the images to .txt files
    convert_frames();
}

void VideoHandler::play() {
    std::string image;
    std::vector<std::string> frames;
    sf::Font monospace;
    monospace.loadFromFile("NotoSansMono-Bold.ttf");
    sf::Text display_frame("", monospace, FONT_SIZE);
    display_frame.setFillColor(sf::Color::Black);
    display_frame.setLineSpacing(0.75);

    for(int i = 1; i <= total_frames; i++) {
        std::ifstream txt_frame(video_name + "/txt_frames/frame" + std::to_string(i) + ".txt");
        std::stringstream stream;
        stream << txt_frame.rdbuf();
        txt_frame.close();
        frames.push_back(stream.str());
    }
    
    music.play();
    window.setVisible(true);
    for(int i = 0; i < frames.size(); i++) {
        window.clear(sf::Color::White);

/*        printf("\r");                             For printing in terminal
        printf("%s", dispay_frame.c_str());         instead of sfml window
        printf("\n");
*/

        display_frame.setString(frames[i]);
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

    cv::Mat resized_frame;
    auto screen = window.getSize();
    double x = (double)screen.x / frame.size().width / FONT_SIZE;
    double y = (double)screen.y / frame.size().height / FONT_SIZE;
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
