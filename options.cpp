/*
 * APoV project
 * By m-c/d in 2020
 */

#include "./headers/Options.hpp"

int Options::SPACE_SIZE = 128;
int Options::ATOMIC_POV_COUNT = 360;
int Options::RAY_STEP = 1;

float Options::MAX_PROJECTION_DEPTH = 300.0f;

void Options::init(int argc, char **argv)
{
    int i = 1;
    while(i < argc){
        const std::string name = argv[i];
        if(name.find("space-size:") == 0)
        {
           Options::SPACE_SIZE = std::stoi(name.substr(11)); 
        } else if(name.find("atomic-pov-count:") == 0)
        {
            Options::ATOMIC_POV_COUNT = std::stoi(name.substr(17));
        } else if(name.find("ray-step:") == 0)
        {
            Options::RAY_STEP = std::stoi(name.substr(9));
        } else if(name.find("projection-depth:") == 0)
        {
            Options::MAX_PROJECTION_DEPTH = (float)std::stoi(name.substr(17));
        }
        i++;
    }
}
