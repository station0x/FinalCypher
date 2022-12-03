//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LPSPChildActorRef.generated.h"

/** 
*Struct that allows for different ways to reference a child actor. 
*/
USTRUCT()
struct LOWPOLYSHOOTERPACK_API FLPSPChildActorRef
{
	GENERATED_BODY()

public:
	/**Name of component property to use.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack")
	FName ComponentName;

	/**Get the actual component pointer from this reference.*/
	template <typename T>
	T* GetComponent(const AActor* OwningActor) const
	{
		//Check name validity.
		T* Actor = nullptr;
		if(ComponentName == NAME_None)
			return nullptr;

		//Get all of the owner's components.
		TInlineComponentArray<UChildActorComponent*> ComponentArray;
		OwningActor->GetComponents(ComponentArray);

		//Look for correct component.
		for(UChildActorComponent* ChildActorComponent : ComponentArray)
		{
			//Ignore components with incorrect names.
			if(ChildActorComponent->GetFName() != ComponentName)
				continue;
		
			//Make sure component has a valid actor.
			AActor* const ChildActor = ChildActorComponent->GetChildActor();
			if(!IsValid(ChildActor))
				continue;
			
			//Check if the actor has the correct type.
			T* const Child = Cast<T>(ChildActor);
			if(!IsValid(Child))
				continue;

			//Cache the reference.
			Actor = Child;
			break;
		}
	
		return Actor;
	}
};