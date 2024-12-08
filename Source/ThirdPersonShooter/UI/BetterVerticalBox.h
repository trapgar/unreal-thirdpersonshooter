#pragma once

#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

#include "BetterVerticalBox.generated.h"

UCLASS()
class THIRDPERSONSHOOTER_API UBetterVerticalBox : public UVerticalBox
{
	GENERATED_BODY()

public:
	/** The list of positions/orientations to draw the markers at. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Panel")
	TEnumAsByte<EHorizontalAlignment> DefaultChildHorizontalAlignment;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Panel")
	TEnumAsByte<EVerticalAlignment> DefaultChildVerticalAlignment;

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

protected:
	virtual void OnSlotAdded(UPanelSlot* NewSlot) override
	{
		Super::OnSlotAdded(NewSlot);

		if (UVerticalBoxSlot* VBoxSlot = Cast<UVerticalBoxSlot>(NewSlot))
		{
			VBoxSlot->SetHorizontalAlignment(DefaultChildHorizontalAlignment);
			VBoxSlot->SetVerticalAlignment(DefaultChildVerticalAlignment);
		}
	};
};
