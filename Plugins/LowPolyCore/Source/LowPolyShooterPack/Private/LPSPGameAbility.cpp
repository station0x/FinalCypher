//Copyright 2021, Infima Games. All Rights Reserved.

#include "LPSPGameAbility.h"

bool FLPSPGameAbility::CanStart(FGameplayTagContainer Container) const
{
	//Make sure the ability can start.
	if(!Container.HasAllExact(RequiredTags))
		return false;

	//Make sure none of the blocked tags are there.
	if(Container.HasAnyExact(BlockedTags))
		return false;

	//Return.
	return true;
}

bool FLPSPGameAbility::TryStart(FGameplayTagContainer& Container) const
{
	//Check if we can start the ability.
	if(CanStart(Container))
	{
		//Add tags.
		Container.AppendTags(AddedTags);
		
		//Remove tags.
		Container.RemoveTags(RemovedTags);

		//Return.
		return true;
	}

	//Return.
	return false;
}

void FLPSPGameAbility::Start(FGameplayTagContainer& Container) const
{
	//Add tags.
	Container.AppendTags(AddedTags);
		
	//Remove tags.
	Container.RemoveTags(RemovedTags);
}

void FLPSPGameAbility::Stop(FGameplayTagContainer& Container, const bool AddRemoved) const
{
	//Removed added tags.
	Container.RemoveTags(AddedTags);
	
	//Re-add removed tags.
	if(AddRemoved)
		Container.AppendTags(RemovedTags);
}
