// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldObjectSettings.h"
#include "WorldObject.h"

// To be able to refresh things in the Editor - it's kinda bad to have the DataAsset class do this directly, but...
// if it works, it saves us time, where we don't have to restart Unreal :P
void UWorldObjectSettings::ApplySettingsToAllObjects()
{
    for (TObjectIterator<AWorldObject> It; It; ++It)
    {
        if (It->SettingsAsset == this)
        {
            It->UseWorldObjectAssetSettings();
#if WITH_EDITOR
            It->RerunConstructionScripts(); // To trigger an Editor update visually
#endif
        }
    }
}
