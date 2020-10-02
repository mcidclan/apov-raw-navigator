# Atomic Point of View | Raw Navigator
```
Nom de code: APoV.

Basic Atomic Point of View Raw Navigator.

### Available options
space-size ............. Size of the space on i,j,k
atomic-pov-count ....... Number of point of views (step angles) on the y axis
ray-step ............... Number of step that increase the ray from its origin
max-ray-depth .......... Maximun length the ray can while raytracing
cam-hemisphere ......... Renders only the first hemisphere of view
cam-locked ............. Simulates a locked camera around the object
filter-gaps-lite ....... Lite gaps filter
filter-gaps ............ Stronger gaps filter
max-projection-depth ... Max projection depth


### Mingw and linux
As an example, you can generate your space with the following:
./bin/apov space-size:256 atomic-pov-count:180 `
    ray-step:2 max-ray-depth:128 cam-locked
    
Then, run the navigator with:
./bin/apov-raw-navigator space-size:256 atomic-pov-count:180 ray-step:2 `
    max-ray-depth:128 projection-depth:700 cam-locked filter-gaps-lite

### PSP
You need to generate your space with:
./bin/apov space-size:256 atomic-pov-count:36 \
    ray-step:8 max-ray-depth:256 cam-locked
    
For other configuration you can change the options via the options file.
