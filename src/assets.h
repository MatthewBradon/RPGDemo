#pragma once

#include "EngineLibrary.h"
#include "math.h"

// #######################################################################################
//                                      Constants
// #######################################################################################


// #######################################################################################
//                                      Structs
// #######################################################################################

enum SpriteID {
    SPRITE_DICE,
    SPRITE_SKELETON,
    SPRITE_MIKE,
    SPRITE_COUNT
};

struct Sprite {
    IVec2 atlasOffset; // Offset in the texture atlas
    IVec2 atlasSize;   // Size of the texture in the atlas

};

// #######################################################################################
//                                      Functions
// #######################################################################################
Sprite getSprite(SpriteID spriteID) {

    Sprite sprite = {};

    switch (spriteID) {
        case SPRITE_DICE:
            sprite.atlasOffset = IVec2(0, 0);
            sprite.atlasSize = IVec2(16, 16);
            break;
        
        case SPRITE_SKELETON:
            sprite.atlasOffset = IVec2(16, 0);
            sprite.atlasSize = IVec2(112, 208);
            break;
        
        case SPRITE_MIKE:
            sprite.atlasOffset = IVec2(128, 0);
            sprite.atlasSize = IVec2(352, 182);
            break;
        
    }

    return sprite;
}