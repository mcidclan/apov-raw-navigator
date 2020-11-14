/*
 * APoV project
 * By m-c/d in 2020
 */

#include "./headers/Options.hpp"

bool Options::CAM_HEMISPHERE = false;
bool Options::CAM_LOCKED = false;
bool Options::SMOOTH_PIXELS = false;
bool Options::FILTER_GAPS = false;
bool Options::FILTER_GAPS_LITE = false;
float Options::MAX_PROJECTION_DEPTH = 0.0f;

u8 Options::HEADER_SIZE = 0;
u8 Options::SPACE_BLOCK_COUNT = 1;
u8 Options::WIDTH_BLOCK_COUNT = 1;
u8 Options::DEPTH_BLOCK_COUNT = 1;
u16 Options::SPACE_BLOCK_SIZE = 256;
u16 Options::RAY_STEP = 1;
u16 Options::HORIZONTAL_POV_COUNT = 1;
u16 Options::VERTICAL_POV_COUNT = 1;

void Options::init(int argc, char **argv) {
    int i = 1;
    while(i < argc){
        const std::string name = argv[i];
        if(name.find("space-block-size:") == 0) {
           Options::SPACE_BLOCK_SIZE = std::stoi(name.substr(17)); 
        } else if(name.find("horizontal-pov-count:") == 0) {
            Options::HORIZONTAL_POV_COUNT = std::stoi(name.substr(21));
        } else if(name.find("vertical-pov-count:") == 0) {
            Options::VERTICAL_POV_COUNT = std::stoi(name.substr(19));
        } else if(name.find("ray-step:") == 0) {
            Options::RAY_STEP = std::stoi(name.substr(9));
        } else if(name.find("projection-depth:") == 0) {
            Options::MAX_PROJECTION_DEPTH = (float)std::stoi(name.substr(17));
        } else if(name.find("space-block-count:") == 0) {
            Options::SPACE_BLOCK_COUNT = std::stoi(name.substr(18));
        } else if(name.find("cam-hemisphere") == 0) {
            Options::CAM_HEMISPHERE = true;
        } else if(name.find("cam-locked") == 0) {
            Options::CAM_LOCKED = true;
        } else if(name.find("smooth-pixels") == 0) {
            Options::SMOOTH_PIXELS = true;
        } else if(name.find("filter-gaps-lite") == 0) {
            Options::FILTER_GAPS = true;
            Options::FILTER_GAPS_LITE = true;
        } else if(name.find("filter-gaps") == 0) {
            Options::FILTER_GAPS = true;
        }
        i++;
    }
    if(Options::CAM_HEMISPHERE) {
        Options::HORIZONTAL_POV_COUNT /= 2;
        Options::VERTICAL_POV_COUNT /= 2;
    }
}
