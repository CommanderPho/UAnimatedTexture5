/**
 * Copyright 2019 Neil Fang. All Rights Reserved.
 *
 * Animated Texture from GIF file
 *
 * Created by Neil Fang
 * GitHub: https://github.com/neil3d/UAnimatedTexture5
 *
*/

#include "AnimatedTextureResource.h"
#include "AnimatedTexture2D.h"

#include "DeviceProfiles/DeviceProfile.h"	// Engine
#include "DeviceProfiles/DeviceProfileManager.h"	// Engine

FAnimatedTextureResource::FAnimatedTextureResource(UAnimatedTexture2D * InOwner) :Owner(InOwner)
{
}

uint32 FAnimatedTextureResource::GetSizeX() const
{
	if (Owner) return Owner->GetSurfaceWidth();
	return 2;
}

uint32 FAnimatedTextureResource::GetSizeY() const
{
	if (Owner)
		return Owner->GetSurfaceHeight();

	return 2;
}

void FAnimatedTextureResource::InitRHI()
{
	//-- create FSamplerStateRHIRef FTexture::SamplerStateRHI
	CreateSamplerStates(
		GetDefaultMipMapBias()
	);

	//-- create FTextureRHIRef FTexture::TextureRHI
	const ETextureCreateFlags CreateFlags = TexCreate_Dynamic | TexCreate_ShaderResource | TexCreate_CPUWritable | TexCreate_DisableDCC;
	uint32 NumMips = 1;

	FRHIResourceCreateInfo CreateInfo(TEXT("AnimatedTexture2D"));
	TextureRHI = RHICreateTexture2D(GetSizeX(), GetSizeY(), PF_B8G8R8A8, NumMips, 1, CreateFlags, CreateInfo);
	TextureRHI->SetName(Owner->GetFName());

	RHIUpdateTextureReference(Owner->TextureReference.TextureReferenceRHI, TextureRHI);

	if (Owner)
		Owner->UpdateFirstFrame();
}

void FAnimatedTextureResource::ReleaseRHI()
{
	FTextureResource::ReleaseRHI();
}

int32 FAnimatedTextureResource::GetDefaultMipMapBias() const
{
	return 0;
}

void FAnimatedTextureResource::CreateSamplerStates(float MipMapBias)
{
	FSamplerStateInitializerRHI SamplerStateInitializer
	(
		SF_Bilinear,
		Owner->AddressX == TA_Wrap ? AM_Wrap : (Owner->AddressX == TA_Clamp ? AM_Clamp : AM_Mirror),
		Owner->AddressY == TA_Wrap ? AM_Wrap : (Owner->AddressY == TA_Clamp ? AM_Clamp : AM_Mirror),
		AM_Wrap,
		MipMapBias
	);
	SamplerStateRHI = RHICreateSamplerState(SamplerStateInitializer);

	FSamplerStateInitializerRHI DeferredPassSamplerStateInitializer
	(
		SF_Bilinear,
		Owner->AddressX == TA_Wrap ? AM_Wrap : (Owner->AddressX == TA_Clamp ? AM_Clamp : AM_Mirror),
		Owner->AddressY == TA_Wrap ? AM_Wrap : (Owner->AddressY == TA_Clamp ? AM_Clamp : AM_Mirror),
		AM_Wrap,
		MipMapBias,
		1,
		0,
		2
	);

	DeferredPassSamplerStateRHI = RHICreateSamplerState(DeferredPassSamplerStateInitializer);
}