#!/bin/bash
BIN_PATH=../bin/1.1-image/obpmark-1.1-image

IMAGE_WIDTH=1024
IMAGE_HEIGHT=1024
IMAGE_FRAMES=10

# FIXME add file reading

${BIN_PATH}_cpu -f $IMAGE_FRAMES -w $IMAGE_WIDTH -h $IMAGE_HEIGHT -r -t
${BIN_PATH}_openmp -f $IMAGE_FRAMES -w $IMAGE_WIDTH -h $IMAGE_HEIGHT -r -t
${BIN_PATH}_opencl -f $IMAGE_FRAMES -w $IMAGE_WIDTH -h $IMAGE_HEIGHT -r -t
${BIN_PATH}_cuda -f $IMAGE_FRAMES -w $IMAGE_WIDTH -h $IMAGE_HEIGHT -r -t

IMAGE_WIDTH=2048
IMAGE_HEIGHT=2048

${BIN_PATH}_cpu -f $IMAGE_FRAMES -w $IMAGE_WIDTH -h $IMAGE_HEIGHT -r -t
${BIN_PATH}_openmp -f $IMAGE_FRAMES -w $IMAGE_WIDTH -h $IMAGE_HEIGHT -r -t
${BIN_PATH}_opencl -f $IMAGE_FRAMES -w $IMAGE_WIDTH -h $IMAGE_HEIGHT -r -t
${BIN_PATH}_cuda -f $IMAGE_FRAMES -w $IMAGE_WIDTH -h $IMAGE_HEIGHT -r -t

IMAGE_WIDTH=4096
IMAGE_HEIGHT=4096

${BIN_PATH}_cpu -f $IMAGE_FRAMES -w $IMAGE_WIDTH -h $IMAGE_HEIGHT -r -t
${BIN_PATH}_openmp -f $IMAGE_FRAMES -w $IMAGE_WIDTH -h $IMAGE_HEIGHT -r -t
${BIN_PATH}_opencl -f $IMAGE_FRAMES -w $IMAGE_WIDTH -h $IMAGE_HEIGHT -r -t
${BIN_PATH}_cuda -f $IMAGE_FRAMES -w $IMAGE_WIDTH -h $IMAGE_HEIGHT -r -t
