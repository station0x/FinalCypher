//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "Runtime/Core/Public/CoreMinimal.h"
#include "Runtime/Engine/Public/Engine.h"
#include "LPSPOffset.h"
#include "LPSPRepresentation.h"
#include "LPSPAttachment.generated.h"

/**Base Attachment class. All Attachments inherit from this class.*/
UCLASS(Abstract)
class LOWPOLYSHOOTERPACK_API ALPSPAttachment : public ALPSPRepresentation
{
	GENERATED_BODY()

public:
	/**Constructor.*/
	ALPSPAttachment();

	/**Mesh used for this attachment.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	UStaticMeshComponent* Mesh;

	/**Returns the value of bAlwaysVisible.*/
	UFUNCTION(BlueprintCallable, Category = "Low Poly Shooter Pack | Attachment")
	bool IsAlwaysVisible() const { return bAlwaysVisible; }

	/**Returns the value of Texture.*/
	UFUNCTION(BlueprintCallable, Category = "Low Poly Shooter Pack | Attachment")
    UTexture* GetTexture() const { return Texture; }

	/**Returns the value of TextureTint.*/
	UFUNCTION(BlueprintCallable, Category = "Low Poly Shooter Pack | Attachment")
	FLinearColor GetTextureTint() const { return TextureTint; }

	/**Returns the value of Offset.*/
	UFUNCTION(BlueprintCallable, Category = "Low Poly Shooter Pack | Attachment")
    FLPSPOffset GetOffset() const { return Offset; }	

protected:
	/**Construction.*/
	virtual void OnConstruction(const FTransform& Transform) override;

	/**On Enable.*/
	virtual void OnEnable_Implementation() override;

	/**On Disable.*/
	virtual void OnDisable_Implementation() override;

private:
	/**Is this attachment always visible, no matter whether it's in-use or not?*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Attachment")
	bool bAlwaysVisible;

	/**Map of materials to apply to this attachment. These map every Material Slot to a Material, and allow us to easily override.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Attachment")
	TMap<FName, UMaterialInterface*> Materials;
	
	/**Offset applied to the weapon while using this attachment.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Attachment")
	FLPSPOffset Offset;

	/**Tint of the attachment UI's image.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Attachment", meta = (DisplayName = "Texture Tint"))
	FLinearColor TextureTint = FLinearColor::White;

	/**Element to display on the player's HUD when this attachment is equipped on a view weapon.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Attachment", meta = (DisplayName = "Texture"))
	UTexture* Texture;
};