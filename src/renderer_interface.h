#pragma once

#include "EngineLibrary.h"
#include "math.h"
#include "assets.h"

// #######################################################################################
//                                      Constants
// #######################################################################################

constexpr int MAX_TRANSFORMS = 1000; // Maximum number of transformations

// #######################################################################################
//                                      Structs
// #######################################################################################

struct Transform {
    IVec2 atlasOffset; // Offset in the texture atlas
    IVec2 atlasSize;   // Size of the texture in the atlas
    Vec2 position; // Position in world space
    Vec2 size;    // Scale factor
};


struct RenderData {
    Transform transforms[MAX_TRANSFORMS]; // Array of transformations
    int transformCount; // Number of transformations
};

// #######################################################################################
//                                      Globals
// #######################################################################################

static RenderData renderData;

// #######################################################################################
//                                      Functions
// #######################################################################################

void drawSprite(SpriteID spriteID, Vec2 position, Vec2 size) {

    Sprite sprite = getSprite(spriteID);

    Transform transform = {};
    transform.position = position;
    transform.size = size;
    transform.atlasOffset = sprite.atlasOffset;
    transform.atlasSize = sprite.atlasSize;


    renderData.transforms[renderData.transformCount] = transform;
    renderData.transformCount++;
}