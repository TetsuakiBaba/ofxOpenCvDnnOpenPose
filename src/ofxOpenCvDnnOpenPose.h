#pragma once

#include "ofMain.h"
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/ocl.hpp>
#include <iostream>


using namespace cv;
using namespace cv::dnn;
using namespace std;



class ofxOpenCvDnnOpenPose{
public:
    ofxOpenCvDnnOpenPose();
    ~ofxOpenCvDnnOpenPose();
    void setup(string _path_to_cfg, string _path_to_weights);
    void update(ofPixels &op);
    void setNetworkImageSize(int _w, int _h);
    void draw();
    
    cv::Mat toCV(ofPixels &pix);
    dnn::Net net;
    int network_width = 368;
    int network_height = 368;
    int input_width;
    int input_height;
    float threshold;
    vector<ofPoint>p;
    Mat color;
};
