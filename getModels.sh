#!/usr/bin/env bash
set -euxo pipefail
path_to_cfg="https://raw.githubusercontent.com/opencv/opencv_extra/master/testdata/dnn/openpose_pose_coco.prototxt"
path_to_weights="http://posefs1.perception.cs.cmu.edu/OpenPose/models/pose/coco/pose_iter_440000.caffemodel"
if wget ${path_to_weights}; then
	echo "downloading caffemodel"
else
	echo "wget is not installed. Falling back to curl"
	curl -O "${path_to_weights}"
fi
cp ./pose_iter_440000.caffemodel ./Examples/single_image/bin/data/dnn/
if wget ${path_to_cfg}; then
	echo "downloading prototxt"
else
	echo "wget is not installed. Falling back to curl"
	curl -O "${path_to_cfg}"
fi
cp openpose_pose_coco.prototxt ./Examples/single_image/bin/data/dnn/
rm -f ./pose_iter_440000.caffemodel
rm -f ./openpose_pose_coco.prototxt
echo "done"
