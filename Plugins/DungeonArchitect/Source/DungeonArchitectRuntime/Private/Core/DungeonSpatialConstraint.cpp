//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/DungeonSpatialConstraint.h"


UDungeonSpatialConstraint::UDungeonSpatialConstraint(const FObjectInitializer& ObjectInitializer) : Super(
    ObjectInitializer) {
    bApplyBaseRotation = false;
    bRotateToFitConstraint = true;
}

