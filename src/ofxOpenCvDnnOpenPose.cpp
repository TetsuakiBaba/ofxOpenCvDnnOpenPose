#include "ofxOpenCvDnnOpenPose.h"

/// connection table, in the format [model_id][pair_id][from/to]
// please look at the nice explanation at the bottom of:
// https://github.com/CMU-Perceptual-Computing-Lab/openpose/blob/master/doc/output.md
//
const int POSE_PAIRS[3][20][2] = {
    {   // COCO body
        {1,2}, {1,5}, {2,3},
        {3,4}, {5,6}, {6,7},
        {1,8}, {8,9}, {9,10},
        {1,11}, {11,12}, {12,13},
        {1,0}, {0,14},
        {14,16}, {0,15}, {15,17}
    },
    {   // MPI body
        {0,1}, {1,2}, {2,3},
        {3,4}, {1,5}, {5,6},
        {6,7}, {1,14}, {14,8}, {8,9},
        {9,10}, {14,11}, {11,12}, {12,13}
    },
    {   // hand
        {0,1}, {1,2}, {2,3}, {3,4},         // thumb
        {0,5}, {5,6}, {6,7}, {7,8},         // pinkie
        {0,9}, {9,10}, {10,11}, {11,12},    // middle
        {0,13}, {13,14}, {14,15}, {15,16},  // ring
        {0,17}, {17,18}, {18,19}, {19,20}   // small
    }
    
};


ofxOpenCvDnnOpenPose::ofxOpenCvDnnOpenPose()
{
    
}



ofxOpenCvDnnOpenPose::~ofxOpenCvDnnOpenPose()
{
    
}

template <typename T>
static cv::Mat toCv(ofPixels_<T>& pix)
{
    int depth;
    switch(pix.getBytesPerChannel())
    {
        case 4: depth = CV_32F; break;
        case 2: depth = CV_16U; break;
        case 1: default: depth = CV_8U; break;
    }
    return cv::Mat(pix.getHeight(), pix.getWidth(), CV_MAKETYPE(depth, pix.getNumChannels()), pix.getData(), 0);
}

cv::Mat ofxOpenCvDnnOpenPose::toCV(ofPixels &pix)
{
    return cv::Mat(pix.getHeight(), pix.getWidth(), CV_MAKETYPE(CV_8U, pix.getNumChannels()), pix.getData(), 0);
}

void ofxOpenCvDnnOpenPose::update(ofPixels &op)
{
    
    Mat img = toCv(op);
    input_width = (int)op.getWidth();
    input_height = (int)op.getHeight();
    
    // send it through the network
    Mat inputBlob = blobFromImage(img, 1.0 / 255, cv::Size(network_width, network_height),
                                  Scalar(0, 0, 0), false, false);
    net.setInput(inputBlob);
    Mat result = net.forward();
    // the result is an array of "heatmaps", the probability of a body part being in location x,y
    int midx, npairs;
    int nparts = result.size[1];
    int H = result.size[2];
    int W = result.size[3];
    // find out, which model we have
    if (nparts == 19)
    {   // COCO body
        midx   = 0;
        npairs = 17;
        nparts = 18; // skip background
    }
    else if (nparts == 16)
    {   // MPI body
        midx   = 1;
        npairs = 14;
    }
    else if (nparts == 22)
    {   // hand
        midx   = 2;
        npairs = 20;
    }
    else
    {
        cerr << "there should be 19 parts for the COCO model, 16 for MPI, or 22 for the hand one, but this model has " << nparts << " parts." << endl;
        return (0);
    }
    // find the position of the body parts
    vector<cv::Point> points(22);
    for (int n=0; n<nparts; n++)
    {
        // Slice heatmap of corresponding body's part.
        Mat heatMap(H, W, CV_32F, result.ptr(0,n));
        // 1 maximum per heatmap
        cv::Point p(-1,-1),pm;
        double conf;
        minMaxLoc(heatMap, 0, &conf, 0, &pm);
        if (conf > threshold )
            p = pm;
        points[n] = p;
    }
    // connect body parts and draw it !
    float SX = float(img.cols) / W;
    float SY = float(img.rows) / H;
    p.clear();
    for (int n=0; n<npairs; n++)
    {
        // lookup 2 connected body/hand parts
        Point2f a = points[POSE_PAIRS[midx][n][0]];
        Point2f b = points[POSE_PAIRS[midx][n][1]];
        // we did not find enough confidence before
        if (a.x<=0 || a.y<=0 || b.x<=0 || b.y<=0)
            continue;
        // scale to image size
        a.x*=SX; a.y*=SY;
        b.x*=SX; b.y*=SY;
        p.push_back(ofPoint(a.x, a.y));
        p.push_back(ofPoint(b.x, b.y));
    }
}


void ofxOpenCvDnnOpenPose::draw()
{
    
    ofSetLineWidth(3);
    for( int i = 0; i < p.size(); i=i+2 ){
        ofSetColor(ofColor::gray);
        ofDrawLine(p[i],p[i+1]);
    }
    
    ofSetColor(ofColor::darkRed);
    for( int i = 0; i < p.size(); i++ ){
        ofFill();
        ofDrawCircle(p[i], 7);
    }
  
}


void ofxOpenCvDnnOpenPose::setup(string _path_to_cfg, string _path_to_weights)
{
    // read the network model
    String modelConfiguration = _path_to_cfg;
    String modelBinary = _path_to_weights;
    net = readNetFromCaffe(modelConfiguration, modelBinary);
    
    std::vector<String> lname = net.getLayerNames();
    for (int i = 0; i < lname.size();i++) {
        std::cout << i+1 << " " << lname[i] << std::endl;
    }
    
    ocl::setUseOpenCL( true );
    net.setPreferableTarget(DNN_TARGET_CPU);

    if (net.empty())
    {
        cout << "Can't load network by using the following files: " << endl;
        cout << "txt-file:     " << modelConfiguration << endl;
        cout << "model-file: " << modelBinary << endl;
    }
    
}

void ofxOpenCvDnnOpenPose::setNetworkImageSize(int _w, int _h)
{
    network_width = _w;
    network_height = _h;
}


