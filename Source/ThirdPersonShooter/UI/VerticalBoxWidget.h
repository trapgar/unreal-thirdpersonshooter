#pragma once

#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

#include "VerticalBoxWidget.generated.h"

UCLASS()
class THIRDPERSONSHOOTER_API UVerticalBoxWidget : public UVerticalBox
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Panel", meta=(DisplayName="Insert Child At"))
	UVerticalBoxSlot* K2_InsertChildAt(int32 Index, UWidget* Content)
	{
		TArray<UWidget*> AllTheWidgetsThatCameBefore = GetAllChildren();

		for (UWidget* Widget : AllTheWidgetsThatCameBefore)
		{
			RemoveChild(Widget);
		}

		UVerticalBoxSlot* NewPanelSlot = nullptr;

		for (int32 i = 0; i < AllTheWidgetsThatCameBefore.Num(); i++)
		{
			if (i == Index)
			{
				NewPanelSlot = Cast<UVerticalBoxSlot>(AddChild(Content));
			}

			AddChild(AllTheWidgetsThatCameBefore[i]);
		}

		if (NewPanelSlot == nullptr)
		{
			NewPanelSlot = Cast<UVerticalBoxSlot>(AddChild(Content));
		}

		return NewPanelSlot;
	};
};
