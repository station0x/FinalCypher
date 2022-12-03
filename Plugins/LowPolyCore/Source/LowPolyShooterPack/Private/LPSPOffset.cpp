//Copyright 2021, Infima Games. All Rights Reserved.

#include "LPSPOffset.h"

FLPSPOffset::FLPSPOffset(const FVector Loc, const FRotator Rot, const FVector Scale): Location(Loc), Rotation(Rot), Scale(Scale)
{
}

FLPSPOffset FLPSPOffset::Add(const FLPSPOffset& Other) const
{
	//Return added offsets.
	return FLPSPOffset(this->Location + Other.Location, this->Rotation + Other.Rotation);
}