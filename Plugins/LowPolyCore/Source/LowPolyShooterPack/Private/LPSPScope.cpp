//Copyright 2021, Infima Games. All Rights Reserved.

#include "LPSPScope.h"
#include "Materials/MaterialInstanceDynamic.h"

void ALPSPScope::BeginPlay()
{
	//Base.
	Super::BeginPlay();

	//Make sure we want to override the sight material. Otherwise it's really annoying that sights don't just use the material.
	if(bOverrideSightMaterial)
	{
		//Create dynamic material.
		UMaterialInstanceDynamic* Material = Mesh->CreateDynamicMaterialInstance(Mesh->GetMaterialIndex("Sight"));
		if(IsValid(Material))
		{
			//Update parameters.
			Material->SetVectorParameterValue("Tint", SightTint);
			Material->SetTextureParameterValue("Texture", SightTexture);
			Material->SetScalarParameterValue("Scale", SightScale);
			Material->SetScalarParameterValue("Parallax Depth", SightDepth);
			Material->SetScalarParameterValue("Emission", SightEmission);
		}
	}
}