/*
 * APoV project
 * By m-c/d in 2020
 */

#include "./headers/Options.hpp"

int Options::SPACE_SIZE;
int Options::ATOMIC_POV_COUNT;

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
        }
        i++;
    }
}
