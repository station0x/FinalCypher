//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "LPSPAttachment.h"
#include "LPSPChildActorRef.h"
#include "LPSPRepresentation.h"
#include "LPSPWeapon.generated.h"

/**Base Weapon Class. All Weapons inherit from this.*/
UCLASS(Abstract)
class LOWPOLYSHOOTERPACK_API ALPSPWeapon final : public ALPSPRepresentation
{
	GENERATED_BODY()
	
public:
	ALPSPWeapon();

	/**Returns the value of Mesh.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Weapon")
	USkeletalMeshComponent* GetMesh() const { return Mesh; }

	/**Returns the value of LandingOffset.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Weapon")
	float GetLandingOffset() const { return LandingOffset; }

	/**Returns the texture used in UIs for this representation.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Weapon")
	UTexture* GetTexture() const { return Texture; }

	/**Returns the value of TextureTint.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Weapon")
	FLinearColor GetTextureTint() const { return TextureTint; }
	
	/**Returns the equipped attachment array. Currently does not include attachments that are only showing due to bAlwaysVisible.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Weapon")
	TArray<ALPSPAttachment*> GetAttachments() const { return Attachments; }

	/**Returns an attachment of a specific class.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Weapon")
	bool TryGetAttachment(TSubclassOf<ALPSPAttachment> Class, ALPSPAttachment*& OutAttachment);

	/**Returns true if there is a muzzle equipped. Also returns the actual equipped muzzle.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Weapon")
    bool TryGetMuzzle(class ALPSPMuzzle* &Muzzle);

	/**Returns true if there is a scope equipped. Also returns the actual equipped scope.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Weapon")
	bool TryGetScope(class ALPSPScope* &Scope);

	/**Returns true if there is a laser equipped. Also returns the actual equipped laser.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Weapon")
    bool TryGetLaser(class ALPSPLaser* &Laser);

	/**Returns true if there is a mag equipped. Also returns the actual equipped mag.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Weapon")
    bool TryGetMag(class ALPSPMagazine* &Mag);

	/**Returns true if there is a grip equipped. Also returns the actual equipped grip.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Weapon")
    bool TryGetGrip(class ALPSPGrip* &Grip);

	/**Returns the value of Instance.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Weapon")
	TSubclassOf<UAnimInstance> GetInstance() const { return Instance; }
	
private:
	UPROPERTY(EditInstanceOnly, BlueprintGetter = GetMesh, Category = "Low Poly Shooter Pack")
	USkeletalMeshComponent* Mesh;

	/**Holder of muzzle attachments.*/
	UPROPERTY(EditInstanceOnly, Category = "Low Poly Shooter Pack")
	USceneComponent* Muzzles;

	/**Holder of scope attachments.*/
	UPROPERTY(EditInstanceOnly, Category = "Low Poly Shooter Pack")
	USceneComponent* Scopes;

	/**Holder of laser attachments.*/
	UPROPERTY(EditInstanceOnly, Category = "Low Poly Shooter Pack")
	USceneComponent* Lasers;

	/**Holder of magazine attachments.*/
	UPROPERTY(EditInstanceOnly, Category = "Low Poly Shooter Pack")
	USceneComponent* Magazines;

	/**Holder of grip attachments.*/
	UPROPERTY(EditInstanceOnly, Category = "Low Poly Shooter Pack")
	USceneComponent* Grips;

	/**Holder of default attachments (meaning they're positioned at zero).*/
	UPROPERTY(EditInstanceOnly, Category = "Low Poly Shooter Pack")
	USceneComponent* Defaults;

	/**Visual representation of the ejection port placement.*/
	UPROPERTY(EditInstanceOnly, Category = "Low Poly Shooter Pack")
	class UArrowComponent* Eject;
	
	/**Name of the muzzle attachment to equip on this representation. (Name is the same as the component name)*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Weapon | Attachments")
	FLPSPChildActorRef EquippedMuzzle;

	/**Name of the scope attachment to equip on this representation. (Name is the same as the component name)*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Weapon | Attachments")
	FLPSPChildActorRef EquippedScope;

	/**Name of the laser attachment to equip on this representation. (Name is the same as the component name)*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Weapon | Attachments")
	FLPSPChildActorRef EquippedLaser;

	/**Name of the magazine attachment to equip on this representation. (Name is the same as the component name)*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Weapon | Attachments")
	FLPSPChildActorRef EquippedMagazine;

	/**Name of the grip attachment to equip on this representation. (Name is the same as the component name)*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Weapon | Attachments")
	FLPSPChildActorRef EquippedGrip;
	
	/**Texture this weapon should display on HUDs.*/
	UPROPERTY(EditAnywhere, BlueprintGetter = GetTexture, Category = "Low Poly Shooter Pack | Weapon | Settings")
	UTexture* Texture;

	/**Tint of the weapon's UI's image.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Weapon | Settings", meta = (DisplayName = "Texture Tint"))
	FLinearColor TextureTint = FLinearColor::White;

	/**Animation instance used by the Character when equipping this Weapon.*/
	UPROPERTY(EditAnywhere, BlueprintGetter = GetInstance, Category = "Low Poly Shooter Pack | Weapon | Settings")
	TSubclassOf<UAnimInstance> Instance;

	/**Montage to play when attacking melee.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Weapon | Animation", meta = (DisplayName = "Melee Montage Additive"))
	UAnimMontage* MeleeAdditiveMontage;

	/**Force applied to any character that holds this weapon when they land.*/
	UPROPERTY(EditAnywhere, BlueprintGetter = GetLandingOffset, Category = "Low Poly Shooter Pack | Weapon | Character")
	float LandingOffset = -1.0f;

	/**Array of attachments currently equipped on this representation.*/
	UPROPERTY()
	TArray<ALPSPAttachment*> Attachments;

	/**Construction.*/
	virtual void OnConstruction(const FTransform& Transform) override;
	
	/***Used to get a reference to an attachment, enable it and save it to our equipped attachments.*/
	template <typename T>
    void CacheAttachment(const FLPSPChildActorRef ActorRef)
	{
		//Get the attachment with the correct type.
		T* const Attachment = ActorRef.GetComponent<T>(this);
		if(IsValid(Attachment))
		{
			//Enable.
			Attachment->TrySetEnabled(true);

			//Save it to our attachments array.
			Attachments.AddUnique(Attachment);
		}
	}
};