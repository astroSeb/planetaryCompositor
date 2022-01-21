//----------------------------------------------------------------------
//
//---- masterDarkCreator.cpp : Outil de cration d'une image master
//                             dark a partir de plusieurs images
//
//----------------------------------------------------------------------

#include <iostream>

#include <opencv2/core/utility.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/features2d.hpp>


//----------------------------------------------------------------------
//---- Usage
//----------------------------------------------------------------------
void usage()
{
    std::cout << "Usage : ./masterDarkCreator <Repertoire des images DARK>" << std::endl;
    std::cout << "                            <Image master dark (sortie)>" << std::endl;
}

//----------------------------------------------------------------------
//---- Main
//----------------------------------------------------------------------
int main(int argc, char **argv)
{
    
    //---- Gestion des arguments
    if (argc !=3)
    {
       usage();
       return 1;
    }

    std::string imDarkDir = argv[1];
    std::string masterDarkPath = argv[2];
    
    
    //---- Recherche des images DARK dans le repertoire
    
    
    
    
    return 0;
}
