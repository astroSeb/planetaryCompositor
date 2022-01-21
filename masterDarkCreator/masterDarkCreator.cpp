//----------------------------------------------------------------------
//
//---- masterDarkCreator.cpp : Outil de cration d'une image master
//                             dark a partir de plusieurs images
//
//----------------------------------------------------------------------

#include <iostream>

#include "dirent.h"

#include <opencv2/opencv.hpp>

//~ #include <opencv2/core/utility.hpp>
//~ #include <opencv2/core.hpp>
//~ #include <opencv2/imgproc.hpp>
//~ #include <opencv2/imgcodecs.hpp>
//~ #include <opencv2/highgui.hpp>
//~ #include <opencv2/photo.hpp>
//~ #include <opencv2/features2d.hpp>



//----------------------------------------------------------------------
//---- Recherche des images dans un repertoire
//----------------------------------------------------------------------
std::vector<std::string> getFileListFromDir(const std::string & dirPath, const std::string & pattern)
{
    //---- Declarations
    std::vector<std::string> imList;
    struct dirent * ent;

    //--- Ouverture du repertoire
    DIR * dir = opendir(dirPath.c_str());
    if ( dir == NULL )
    {
        std::cout << "ERREUR : Impossible d'ouvrir le repertoire " << dirPath << std::endl;
        return imList;
    }

    //--- Lecture du repertoire
    while ( (ent = readdir(dir)) != NULL )
    {
        std::string currentFilePath = ent->d_name;
        
        if ( pattern != "" )
        {
            //-- Recherche de l'extension dans le nom du fichier
            if ( currentFilePath.find(pattern) != std::string::npos )
            {
                imList.push_back(dirPath + "/" + currentFilePath);
            }
        } else {
            
            imList.push_back(dirPath + "/" + currentFilePath);
        }
    }

    //--- Fermeture du repertoire
    closedir(dir);
    
    //--- Tri par ordre alphabetique
    std::sort(imList.begin(), imList.end());

    return imList;

}




//----------------------------------------------------------------------
//---- Usage
//----------------------------------------------------------------------
void usage()
{
    std::cout << "Usage : ./masterDarkCreator <Repertoire des images DARK>" << std::endl;
    std::cout << "                            <Extension des images DARK>" << std::endl;
    std::cout << "                            <Image master dark (sortie)>" << std::endl;
}


//----------------------------------------------------------------------
//---- Main
//----------------------------------------------------------------------
int main(int argc, char **argv)
{
    
    //---- Gestion des arguments
    if (argc != 4)
    {
       usage();
       return 1;
    }

    std::string imDarkDir = argv[1];
    std::string imExt = argv[2];
    std::string masterDarkPath = argv[3];
    
    
    //---- Recherche des images DARK dans le repertoire
    std::vector<std::string> imList = getFileListFromDir(imDarkDir, imExt);
    std::cout << "INFO   : " << imList.size() << " images trouvees dans " << imDarkDir << std::endl;
    
    //---- Ouverture de la premiere image pour lire ses dimensiosns
    cv::Mat firstImage = cv::imread(imList[0]);
    if ( firstImage.empty() )
    {
        std::cout << "ERREUR : Echec lors de l'ouverture de l'image " << imList[0] << std::endl;
        return 1;
    }
    std::cout << "INFO   : Taille image " << firstImage.cols << " * " << firstImage.rows << std::endl;
    std::cout << "INFO   : Type de donnees : " << firstImage.channels() << " cannaux, " << firstImage.elemSize1()*8 << " bit par pixel" << std::endl;
    
    
    //---- Creation de la cv::Mat master DARK
    cv::Mat masterDark;
    firstImage.convertTo(masterDark, CV_32FC3);
    
    //---- Somme des DARK
    cv::Mat currentFloatIm;
    for (unsigned int index = 1; index < imList.size(); ++index)
    {
        //--- Chargement image
        cv::Mat currentImage = cv::imread(imList[index]);
        currentImage.convertTo(currentFloatIm, CV_32FC3);
        cv::add(masterDark, currentFloatIm, masterDark);
        
    }
    
    //---- Moyenne des DARK
    masterDark /= float(imList.size());
    
    //---- Conversion du master DARK en CV_8UC3
    cv::Mat masterDark_8;
    masterDark.convertTo(masterDark_8, CV_8UC3);
    
    
    //---- Export du master DARK
    if ( ! cv::imwrite(masterDarkPath, masterDark_8) )
    {
        std::cout << "ERREUR : Echec lors de l'export du master DARK " << masterDarkPath << std::endl;
        return 1;
    }
    
    
    return 0;
}
