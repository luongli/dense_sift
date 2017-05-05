// C headers
#include <string.h>
#include <stdlib.h>
// C++ headers
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


// opencv headers
#include <opencv/cv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/xfeatures2d.hpp"

#define N_IMAGES 6
#define N_CLUSTERS 256

using namespace cv;
using namespace std;

void createDenseFeature(vector<KeyPoint> &keypoints, Mat image, float initFeatureScale, int featureScaleLevels,
                        float featureScaleMul,
                        int initXyStep, int initImgBound,
                        bool varyXyStepWithScale,
                        bool varyImgBoundWithScale);


Mat extractDenseSift(Mat img, vector<KeyPoint> &keypoint);

/**
 * write mat object of opencv to csv file
 */
void writeCSV(string filename, Mat m)
{
    ofstream myfile;
    myfile.open(filename.c_str());
    myfile<< cv::format(m, cv::Formatter::FMT_CSV) << std::endl;
    myfile.close();
}

/**
 * Check if a given path is a regular file or not
 * Return true/false
 */
int isRegularFile(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

/** @function main */
int main(int argc, char **argv) {

    if (argc < 2) {
        printf("Error: missing path to training images\n");
        exit(1);
    }

    int kmean_iterator = 0;
    if (argc >= 3) {
        kmean_iterator = atoi(argv[2]);
    }

    if (kmean_iterator == 0) {
        printf("Warning: You do not specify the number of iterators for kmean, so run kmean with default 100 iterators\n");
        kmean_iterator = 100;
    }

    //namedWindow("show image", WINDOW_AUTOSIZE);
    Mat img;
    Mat dsift;
    Mat centers;
    Mat labels;
    char filename[200];
    DIR *dpdf;
    struct dirent *epdf;
    int i = 0;

    // open directory of training images
    dpdf = opendir(argv[1]);
    if (dpdf == NULL) {
        printf("Cannot open directory %s\n", argv[1]);
        exit(1);
    }
    
    while(epdf = readdir(dpdf)) {
        // compose the path to the image
        sprintf(filename, "%s/%s", argv[1], epdf->d_name);
        if (isRegularFile(filename)) {
            printf("Loading and computing sift of image %s\n", filename);
            /// Load an image
            img = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
            if (!img.data) {
                cout << "Cannot open the image " << filename << endl;
                exit(1);
            }
            //  dense sift
            vector<KeyPoint> keypoints;
            Mat result = extractDenseSift(img, keypoints);
            
            // add the dense sift result of each image to the array dsift
            // for k mean clustering
            if (i == 0) {
                // first loop
                // init dsift to result
                dsift = result.clone();
            } else {
                // concat result to dsift
                vconcat(result, dsift, dsift);
            }
            i++;
        }
    }

    closedir(dpdf);
    
    cout << "Loaded " << i << " images" << endl;
    printf("The feature matrix has %d rows and %d cols\n", dsift.rows, dsift.cols);
    printf("Now run Kmean with %d iterators\n", kmean_iterator);

    // now clustering using kmean
    kmeans(dsift, N_CLUSTERS, labels,
            TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 10, 1.0),
            kmean_iterator, KMEANS_PP_CENTERS, centers);
    writeCSV("dict.csv", centers);

    printf("Done\nThe centers matrix has %d rows and %d cols\n", centers.rows, centers.cols);

    printf("The result was writen to dict.csv\n");

    // waitKey(0);

    return 0;
}

void createDenseFeature(vector<KeyPoint> &keypoints, Mat image,
                        int initXyStep = 6, int initImgBound = 0,
                        float initFeatureScale = 1.f, int featureScaleLevels = 1,
                        float featureScaleMul = 0.1f,
                        bool varyXyStepWithScale = true,
                        bool varyImgBoundWithScale = false) {
    float curScale = static_cast<float>(initFeatureScale);
    int curStep = initXyStep;
    int curBound = initImgBound;

    for (int curLevel = 0; curLevel < featureScaleLevels; curLevel++) {
        for (int x = curBound; x < image.cols - curBound; x += curStep) {
            for (int y = curBound; y < image.rows - curBound; y += curStep) {
                keypoints.push_back(KeyPoint(static_cast<float>(x), static_cast<float>(y), curScale));
            }
        }

        curScale = static_cast<float>(curScale * featureScaleMul);
        if (varyXyStepWithScale) curStep = static_cast<int>( curStep * featureScaleMul + 0.5f );
        if (varyImgBoundWithScale) curBound = static_cast<int>( curBound * featureScaleMul + 0.5f );
    }

}

Mat extractDenseSift(Mat img, vector<KeyPoint> &keypoints) {
    Mat descriptors;

    Ptr<Feature2D> sift = xfeatures2d::SIFT::create();
    createDenseFeature(keypoints, img, 1, 5, 4);
    sift->compute(img, keypoints, descriptors);

    return descriptors;
}
