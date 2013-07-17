#include "Background.h"
#include "Camera.h"
#include "ConstantBackground.h"
#include "Color.h"
#include "Image.h"
#include "LambertianMaterial.h"
#include "Light.h"
#include "Material.h"
#include "Object.h"
#include "PinholeCamera.h"
#include "Plane.h"
#include "PointLight.h"
#include "Ray.h"
#include "Scene.h"
#include "Sphere.h"
#include "Time.h"
#include "Parser.h"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

int main(int argc, char** argv)
{
    double t1 = Time::currentSeconds();

    if ( argc < 2 ) {
        cerr << "Usage: specter scene" << endl;
        return 1;
    }
    ifstream scene_file( argv[ 1 ] );
    if ( !scene_file.good() ) {
        cerr << "Unable to read scene file: " << argv[ 1 ] << endl;
        return 1;
    }
    Parser reader( scene_file );
    string filename;
    Scene *scene = reader.parseScene( filename );
    scene->setMaxRayDepth(10);

    cerr << "rendering " << reader.maxTime()+1 << " frames with a max reflection depth of " << scene->getMaxRayDepth() << endl;
    scene->preprocess(reader.maxTime());

    double t2 = Time::currentSeconds();
    for(int i = 0; i <= reader.maxTime(); i++)
    {
        double before = Time::currentSeconds();

        scene->render(i);

        if(Time::currentSeconds() - before > 1)
            cerr << endl << "rendered frame " << setprecision(2)
                << std::to_string(i) << "/" << std::to_string(reader.maxTime())
                << " in " << setprecision(3) << Time::currentSeconds() - before
                << " seconds" << endl;
        else
        {
            cerr << std::to_string(i) << " ";
            if(i % 10 == 0 && (i > 0))
                cerr << endl;
        }

        scene->getImage()->write(std::to_string(i) + "_" + filename);
    }

    cerr << endl;

    double t3 = Time::currentSeconds();
    cerr << "Setup/load time:\t\t" << setprecision(3) << t2-t1 << " seconds\n";
    cerr << "Render/Write to disk time:\t" << setprecision(3) << t3-t2 << " seconds\n";
    return 0;
}
