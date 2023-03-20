/**
 COMP557 Assignment1
 Hanzhi Zhang
 260908416
 */
#pragma once
#ifndef MOTION_H
#define MOTION_H

#include <string>
#include <vector>
#include <memory>
#include "GLSL.h"

class DAGNode;

/**
 * This class manages (loads) a character skeleton specification along with motion data from a bvh file.
 * NOTE: You should not need to modify this class, but you should understand the public member data and 
 * feel free to make changes you deem necessary.
 */
class Motion
{
public:
	Motion();
	virtual ~Motion();
	bool loadBVH(const std::string& bvhName);

	// root node of the skeleton
	DAGNode* root;
	// total number of channels (degrees of freedom) in the skeleton
	int numChannels;
	// total number of motion capture frames loaded
	int numFrames;
	// frame step size in seconds (could be used if we wanted to play back at normal speed)
	float frameTime;
	// motion data, channel j of frame i can be accessed with data[i*numChannels+j]
	float* data;
};

#endif
