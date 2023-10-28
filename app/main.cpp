#include <iostream>
#include "Image.hpp"
#include <math.h>
using namespace std;


int main(int argc, char* argv[])
{
    Image* img = new Image(argv[1]);

    img->edge_detection(50,true);
    
    img->write_pgm("/tmp/ModifiedImage");

    delete img;
    
    cout << "Succesfully Done!" << endl;
    return 0;
}
