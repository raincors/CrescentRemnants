// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldObjectSettings.h"
#include "WorldObject.h"

// To be able to refresh things in the Editor - it's kinda bad to have the DataAsset class do this directly.
// It does save us time, where we don't have to restart the Unreal Engine.
// However, this is... somewhat unreliable and can cause crashes from time to time until a more robust solution is implemented.
void UWorldObjectSettings::ApplySettingsToAllObjects()
{
	// The below Editor-only code granted to me generously by AI to help indicate a warning of a possible crash when trying
	// to apply settings to all objects within the Editor / Level.
#if WITH_EDITOR
	// Show a dialogue in the editor
	FText DialogTitle = FText::FromString("Apply Settings");
	FText DialogMessage = FText::FromString("Applying settings to all objects may cause the editor to become unresponsive or crash. Make sure you have saved your work.\n\nDo you want to continue?");
    
	// Create a modal dialogue
	FModuleManager::LoadModuleChecked<IModuleInterface>("MainFrame");
	const bool bResult = FMessageDialog::Open(EAppMsgType::YesNo, DialogMessage, DialogTitle) == EAppReturnType::Yes;
    
	if (!bResult)
	{
		UE_LOG(LogTemp, Log, TEXT("WorldObjectSettings: User canceled applying settings"));
		return;
	}
#endif

	// Expensive Unreal-specific iterator that loops through ALL objects of type AWorldObject with subclasses.
    for (TObjectIterator<AWorldObject> It; It; ++It)
    {
    	// If the object is not valid / returns null, move to the next element in the loop.
		if (!IsValid(*It)) continue;
    	
    	// If the WorldObject does not have a valid SettingsAsset, and if the specific SettingsAsset is not this instance.
        if (!It->SettingsAsset.IsValid() || It->SettingsAsset != this) continue;

    	// Run the function that applies the WorldObjectAssetSettings.
    	It->UseWorldObjectAssetSettings();
#if WITH_EDITOR
    	// Forces the object to update its meshes, materials, transforms immediately inside the Editor.
    	It->RerunConstructionScripts(); 
#endif
    }
}
