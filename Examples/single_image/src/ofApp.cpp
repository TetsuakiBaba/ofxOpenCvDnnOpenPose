#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    img.load(ofToDataPath("bolt.jpg"));
    ofSetWindowShape(img.getWidth(), img.getHeight());
    openpose.setNetworkImageSize(386, 386);
    openpose.setup(ofToDataPath("dnn/openpose_pose_coco.prototxt"),
                   ofToDataPath("dnn/pose_iter_440000.caffemodel"));
    openpose.update(img.getPixels());
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    ofSetColor(255);
    img.draw(0,0);
    openpose.draw();
    ofDrawBitmapStringHighlight("Drag and Drop a image file",
                                 ofGetWidth()/2-100,
                                 ofGetHeight()-20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}


//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    if( dragInfo.files.size() > 0 ){
        img.load(dragInfo.files[0]);
        img.setImageType(OF_IMAGE_COLOR);
    }
    ofSetWindowShape(img.getWidth(), img.getHeight());
    openpose.update(img.getPixels());
}
