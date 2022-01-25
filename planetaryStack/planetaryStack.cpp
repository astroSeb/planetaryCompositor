//----------------------------------------------------------------------
//
//---- planetaryStack.cpp : Outil de stacking d'images planetaires
//
//----------------------------------------------------------------------

#include <iostream>

#include "dirent.h"

#include <opencv2/opencv.hpp>


struct ObjetRoi
{
    int minX;
    int maxX;
    int minY;
    int maxY;
    int surface;
};


//----------------------------------------------------------------------
//---- Affichage image
//----------------------------------------------------------------------
void dispIm(const cv::Mat & im)
{
    cv::namedWindow("Display", cv::WINDOW_NORMAL);
    imshow("Display", im);
    cv::waitKey(0);
    
}


//----------------------------------------------------------------------
//---- Flood fill
//----------------------------------------------------------------------
void floodFill(cv::Mat & imBin, int col0, int lig0, ObjetRoi & obj)
{
    //~ std::cout << "DEBUG  : floddFill " << col0 << ", " << lig0 << std::endl;
    //---- Mise a jour de l'objet
    if ( col0 < obj.minX ) { obj.minX = col0; }
    if ( col0 > obj.maxX ) { obj.maxX = col0; }
    if ( lig0 < obj.minY ) { obj.minY = lig0; }
    if ( lig0 > obj.maxY ) { obj.maxY = lig0; }
    obj.surface += 1;
    
    //---- Mise a jour de l'image
    imBin.at<uint8_t>(lig0, col0) = 0;
    
    //---- Parcours des voisins
    uint8_t currentVal;
    for (int lig = lig0-1; lig <= lig0+1; ++lig)
    {
        if ( (lig >= 0) && (lig < imBin.rows) )
        {
            for (int col = col0-1; col <= col0+1; ++col)
            {
                if ( (col >= 0) && (col < imBin.cols) )
                {
                    currentVal = imBin.at<uint8_t>(lig, col);
                    if ( currentVal == 255 )
                    {
                        floodFill(imBin, col, lig, obj);
                    }
                }
            }
        }
    }
    
    
}


//----------------------------------------------------------------------
//---- Detection de l'objet le plus gros
//----------------------------------------------------------------------
cv::Rect detectPlanet(cv::Mat & imBin)
{
    std::vector<ObjetRoi> vecObj;
    cv::Rect maxObjRoi;
    
    //---- Parcours de l'image binaire
    uint8_t currentVal;
    ObjetRoi currentObj;
    for ( int lig = 0; lig < imBin.rows; ++lig)
    {
        for ( int col = 0; col < imBin.cols; ++col)
        {
            currentVal = imBin.at<uint8_t>(lig, col);
            if ( currentVal == 255 )
            {
                //~ std::cout << "DEBUG  : obj " << col << ", lig " << lig << std::endl;
                //--- Initialisation objet
                currentObj.minX = col;
                currentObj.maxX = col;
                currentObj.minY = lig;
                currentObj.maxY = lig;
                currentObj.surface = 0;
                
                floodFill(imBin, col, lig, currentObj);
                vecObj.push_back(currentObj);
            }
        }
    }
    std::cout << "DEBUG  : Nb obj " << vecObj.size() << std::endl;
    
    //---- Recherche de l'objet detecte le plus gros
    int surfaceMax = 0;
    for (std::vector<ObjetRoi>::iterator iObj = vecObj.begin(); iObj != vecObj.end(); ++iObj)
    {
        //--- Test plus grande surface
        if ( iObj->surface > surfaceMax )
        {
            surfaceMax = iObj->surface;
            maxObjRoi.x = iObj->minX;
            maxObjRoi.y = iObj->minY;
            maxObjRoi.width = iObj->maxX - iObj->minX + 1;
            maxObjRoi.height = iObj->maxY - iObj->minY + 1;
        }
    }
    
    return maxObjRoi;
}


//----------------------------------------------------------------------
//---- Calcul statistiques du fond
//----------------------------------------------------------------------
void computeBgStat(const cv::Mat & im, cv::Scalar & bgMean, cv::Scalar & bgSigma)
{   
    //---- Taille de la zone de calcuk
    int winSize = 71;
    
    //---- Nombre de zone de calcul
    int nbRoiLig = im.rows / winSize;
    int nbRoiCol = im.cols / winSize;
    
    std::cout << "DEBUG  : nb roi " << nbRoiLig << ", " << nbRoiCol << std::endl;
    
    //---- Parcours des zones de calcul
    cv::Mat currentRoi;
    cv::Scalar currentMean, currentSigma;
    double nbRoi = 0.0;
    for (int lig = 0; lig < nbRoiLig; ++lig)
    {
        for (int col = 0; col < nbRoiCol; ++col)
        {
            //--- Extraction Roi
            cv::Rect roi(col*winSize, lig*winSize, winSize, winSize);
            currentRoi = im(roi);
            
            //--- Calcul moyenne, ecart-type
            cv::meanStdDev(currentRoi, currentMean, currentSigma);
            
            bgMean += currentMean;
            bgSigma += currentSigma;
            
            nbRoi += 1.0;
        }
    }
    
    bgMean /= nbRoi;
    bgSigma /= nbRoi;
    
}


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
    std::cout << "Usage : ./planetaryStack <Repertoire des images>" << std::endl;
    std::cout << "                         <Extension des images>" << std::endl;
    std::cout << "                         <Image stack (sortie)>" << std::endl;
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

    std::string imDir = argv[1];
    std::string imExt = argv[2];
    std::string finalImPath = argv[3];
    
    
    //---- Recherche des images dans le repertoire
    std::vector<std::string> imList = getFileListFromDir(imDir, imExt);
    std::cout << "INFO   : " << imList.size() << " images trouvees dans " << imDir << std::endl;
    
    //---- Ouverture de la premiere image pour lire ses dimensiosns
    cv::Mat firstImage = cv::imread(imList[0]);
    if ( firstImage.empty() )
    {
        std::cout << "ERREUR : Echec lors de l'ouverture de l'image " << imList[0] << std::endl;
        return 1;
    }
    std::cout << "INFO   : Taille image " << firstImage.cols << " * " << firstImage.rows << std::endl;
    std::cout << "INFO   : Type de donnees : " << firstImage.channels() << " cannaux, " << firstImage.elemSize1()*8 << " bit par pixel" << std::endl;
    dispIm(firstImage);
    
    //---- Creation de l'image pour le stacking
    cv::Mat imStack;
    firstImage.convertTo(imStack, CV_32FC3);
    
    //---- Recherche de la planete
    //--- Calcul statistiques du fond du ciel
    cv::Scalar bgMean, bgSigma;
    computeBgStat(firstImage, bgMean, bgSigma);
    std::cout << "DEBUG  : Statistiques fond du ciel " << bgMean << " +- " << bgSigma << std::endl;
    
    //--- Binarisation
    cv::Mat imBin;
    double seuil = bgMean[0] + 15.0 * bgSigma[0];
    cv::threshold(firstImage, imBin, seuil, 255, cv::THRESH_BINARY);
    cv::cvtColor(imBin, imBin, cv::COLOR_BGR2GRAY);
    std::cout << "INFO   : Type de donnees imBin : " << imBin.channels() << " cannaux, " << imBin.elemSize1()*8 << " bit par pixel" << std::endl;
    dispIm(imBin);
    
    //--- Detection de l'objet le plus gros
    cv::Rect planetRoi = detectPlanet(imBin);
    std::cout << "DEBUG  : Coord planet " << planetRoi.x << ", " << planetRoi.y << ", " << planetRoi.width << ", " << planetRoi.height << std::endl;
    cv::rectangle(firstImage, planetRoi, cv::Scalar(255, 0, 0));
    dispIm(firstImage);
    
    cv::Mat planetTemplate = firstImage(planetRoi);
    dispIm(planetTemplate);
    
    
    //---- Parcours des autres images pour recherche de la planete
    cv::Mat currentImage, currentResult, currentTranslated, currentFloatTrans;
    float nbIm = 0.0;
    for (unsigned int indexIm = 1; indexIm < imList.size(); ++indexIm)
    {
        //--- Chargement de l'image
        currentImage = cv::imread(imList[indexIm]);
        if ( currentImage.empty() )
        {
            std::cout << "ERREUR : Echec lors de l'ouverture de l'image " << imList[indexIm] << std::endl;
            return 1;
        }
        
        //--- Recherche de la planete
        cv::matchTemplate(currentImage, planetTemplate, currentResult, cv::TM_CCORR_NORMED);
        double minVal, maxVal;
        cv::Point minLoc, maxLoc;
        minMaxLoc(currentResult, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
        //~ std::cout << "DEBUG  : Match " << maxLoc.x << ", " << maxLoc.y << ", score " << maxVal << std::endl;
        
        //--- TODO verifier si la planete n'est pas coupee !!!
        
        //~ cv::Rect test;
        //~ test.x = maxLoc.x;
        //~ test.y = maxLoc.y;
        //~ test.width = 50;
        //~ test.height = 50;
        //~ cv::rectangle(currentImage, test, cv::Scalar(255, 0, 0));
        //~ dispIm(currentImage);
        
        //--- Calcul de la translation
        float dx = -(maxLoc.x - planetRoi.x);
        float dy = -(maxLoc.y - planetRoi.y);
        //~ std::cout << "DEBUG  : Trans " << dx << ", " << dy << std::endl;
        
        //--- Translation de l'image courante
        float warpValues[] = { 1.0, 0.0, dx, 0.0, 1.0, dy };
        cv::Mat translationMat = cv::Mat(2, 3, CV_32F, warpValues);
        cv::warpAffine(currentImage, currentTranslated, translationMat, currentImage.size());
        
        //~ dispIm(currentImage);
        //~ dispIm(currentTranslated);
        
        //--- Somme des stacks
        currentTranslated.convertTo(currentFloatTrans, CV_32FC3);
        cv::add(imStack, currentFloatTrans, imStack);
        nbIm += 1.0;
    }
    
    //---- Moyenne des stacks
    imStack /= nbIm;
    
    //---- Conversion du stack en CV_8UC3
    cv::Mat imStack_8;
    imStack.convertTo(imStack_8, CV_8UC3);
    
    
    //---- Export du master DARK
    if ( ! cv::imwrite(finalImPath, imStack_8) )
    {
        std::cout << "ERREUR : Echec lors de l'export du master DARK " << finalImPath << std::endl;
        return 1;
    }
    
    
    return 0;
}
