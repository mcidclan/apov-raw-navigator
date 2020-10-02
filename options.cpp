/*
 * APoV project
 * By m-c/d in 2020
 */

#include "./headers/Options.hpp"

int Options::SPACE_SIZE = 128;
int Options::ATOMIC_POV_COUNT = 360;
int Options::RAY_STEP = 1;
u32 Options::MAX_RAY_DEPTH = 64;
bool Options::CAM_HEMISPHERE = false;
bool Options::CAM_LOCKED = false;
bool Options::SMOOTH_PIXELS = false;
bool Options::FILTER_GAPS = false;
bool Options::FILTER_GAPS_LITE = false;
float Options::MAX_PROJECTION_DEPTH = 300.0f;

void Options::init(int argc, char **argv) {
    #ifndef PSP
        int i = 1;
        while(i < argc){
            const std::string name = argv[i];
            if(name.find("space-size:") == 0) {
               Options::SPACE_SIZE = std::stoi(name.substr(11)); 
            } else if(name.find("atomic-pov-count:") == 0) {
                Options::ATOMIC_POV_COUNT = std::stoi(name.substr(17));
            } else if(name.find("ray-step:") == 0) {
                Options::RAY_STEP = std::stoi(name.substr(9));
            } else if(name.find("max-ray-depth:") == 0) {
                Options::MAX_RAY_DEPTH = std::stoi(name.substr(14));
            } else if(name.find("projection-depth:") == 0) {
                Options::MAX_PROJECTION_DEPTH = (float)std::stoi(name.substr(17));
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
    #else
        // ToDo: To be loaded from atom.bin header
        Options::SPACE_SIZE = 256;
        Options::ATOMIC_POV_COUNT = 90;
        Options::RAY_STEP = 4;
        Options::MAX_RAY_DEPTH = 192;
        Options::CAM_HEMISPHERE = false;
        Options::CAM_LOCKED = true;
        Options::SMOOTH_PIXELS = false;
        Options::FILTER_GAPS_LITE = true;
        Options::FILTER_GAPS = true;
        Options::MAX_PROJECTION_DEPTH = 700.0f;
    #endif

    if(Options::CAM_HEMISPHERE) {
        Options::ATOMIC_POV_COUNT /= 2;
    }
}
