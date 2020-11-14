# Atomic Point of View | Raw Navigator
```
Nom de code: APoV.

Basic Atomic Point of View Raw Navigator.

### Available options
space-size ............. Size of the space on i,j,k
atomic-pov-count ....... Number of point of views (step angles) on the y axis
ray-step ............... Number of step that increase the ray from its origin
cam-hemisphere ......... Renders only the first hemisphere of view
cam-locked ............. Simulates a locked camera around the object
filter-gaps-lite ....... Lite gaps filter
filter-gaps ............ Stronger gaps filter
max-projection-depth ... Max projection depth

To disable the realtime projection, do not specify max-projection-depth or set
it to 0 in the command line.

### Mingw and linux
As an example, you can generate your space with the following:
./bin/apov space-size:256 atomic-pov-count:180 `
    ray-step:2 max-ray-depth:128 cam-locked
    
Then, run the navigator with:
./bin/apov-raw-navigator space-size:256 atomic-pov-count:180 ray-step:2 `
    max-ray-depth:128 projection-depth:800 cam-locked filter-gaps-lite
    