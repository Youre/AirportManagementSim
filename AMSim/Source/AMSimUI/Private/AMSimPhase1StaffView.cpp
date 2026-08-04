#include "AMSimPhase1StaffView.h"

#include "AMSimPhase1StaffPresentation.h"
#include "AMSimUISoundSubsystem.h"
#include "AMSimUITheme.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

namespace
{
	UTextBlock* MakeStaffText(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const TCHAR* Value,
		const int32 Size,
		const FLinearColor& Color,
		const bool bBold = false)
	{
		UTextBlock* Text = Tree->ConstructWidget<UTextBlock>(
			UTextBlock::StaticClass(), Name);
		Text->SetText(FText::FromString(Value));
		Text->SetAutoWrapText(true);
		AMSim::UITheme::StyleText(Text, Size, Color, bBold, true);
		return Text;
	}
}

TSharedRef<SWidget> UAMSimPhase1StaffView::RebuildWidget()
{
	WidgetTree = NewObject<UWidgetTree>(this, TEXT("Phase1StaffTree"));
	UBorder* Panel = WidgetTree->ConstructWidget<UBorder>(
		UBorder::StaticClass(), TEXT("Phase1StaffPanel"));
	WidgetTree->RootWidget = Panel;
	AMSim::UITheme::StyleSurface(
		Panel, AMSim::UITheme::ESurface::Panel, FMargin(18.0f), 18.0f, 2.0f);

	UVerticalBox* Column = WidgetTree->ConstructWidget<UVerticalBox>();
	Panel->SetContent(Column);
	const auto Add = [Column](UWidget* Child, const float Top)
	{
		UVerticalBoxSlot* Slot = Column->AddChildToVerticalBox(Child);
		Slot->SetPadding(FMargin(0.0f, Top, 0.0f, 0.0f));
		Slot->SetHorizontalAlignment(HAlign_Fill);
	};

	Add(MakeStaffText(
		WidgetTree, TEXT("StaffTitle"), TEXT("STAFF"), 14,
		AMSim::UITheme::Cyan(), true), 0.0f);
	Add(MakeStaffText(
		WidgetTree, TEXT("StaffSection"), TEXT("CONSTRUCTION CREW"), 12,
		AMSim::UITheme::Muted(), true), 7.0f);
	CrewCountText = MakeStaffText(
		WidgetTree, TEXT("StaffCrewCount"), TEXT("4 WORKERS"), 26,
		AMSim::UITheme::White(), true);
	Add(CrewCountText, 2.0f);
	AvailabilityText = MakeStaffText(
		WidgetTree, TEXT("StaffAvailability"), TEXT("AVAILABLE 4  |  ASSIGNED 0"), 13,
		AMSim::UITheme::Amber(), true);
	Add(AvailabilityText, 5.0f);
	StatusText = MakeStaffText(
		WidgetTree, TEXT("StaffStatus"), TEXT("CREW AVAILABLE"), 15,
		AMSim::UITheme::Cyan(), true);
	Add(StatusText, 13.0f);
	DetailText = MakeStaffText(
		WidgetTree, TEXT("StaffDetail"),
		TEXT("Construction dispatch is automatic when a project is funded."), 13,
		AMSim::UITheme::Muted());
	Add(DetailText, 4.0f);

	UButton* CloseButton = WidgetTree->ConstructWidget<UButton>(
		UButton::StaticClass(), TEXT("CloseStaff"));
	CloseButton->SetStyle(AMSim::UITheme::ButtonStyle(
		AMSim::UITheme::EButton::Quiet));
	CloseButton->SetContent(MakeStaffText(
		WidgetTree, TEXT("CloseStaffLabel"), TEXT("CLOSE"), 12,
		AMSim::UITheme::White(), true));
	CloseButton->OnClicked.AddDynamic(this, &UAMSimPhase1StaffView::Close);
	Add(CloseButton, 14.0f);
	SetVisibility(ESlateVisibility::Collapsed);
	return Panel->TakeWidget();
}

void UAMSimPhase1StaffView::TogglePanel()
{
	const bool bOpen = !IsPanelOpen();
	SetVisibility(bOpen ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	AMSim::UIAudio::Play(
		this, bOpen ? EAMSimUISound::PanelOpen : EAMSimUISound::Back);
}

void UAMSimPhase1StaffView::ClosePanel()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

bool UAMSimPhase1StaffView::IsPanelOpen() const
{
	return GetVisibility() != ESlateVisibility::Collapsed;
}

void UAMSimPhase1StaffView::RefreshFromSnapshot(
	const AMSim::FPhase1QuerySnapshot& Query,
	const AMSim::FPhase1State& State)
{
	const AMSim::FPhase1StaffPanelState View =
		AMSim::FPhase1StaffPresentation::Derive(Query, State);
	if (CrewCountText)
	{
		CrewCountText->SetText(FText::FromString(FString::Printf(
			TEXT("%d WORKERS"), View.TotalConstructionWorkers)));
	}
	if (AvailabilityText)
	{
		AvailabilityText->SetText(FText::FromString(FString::Printf(
			TEXT("AVAILABLE %d  |  ASSIGNED %d"),
			View.AvailableConstructionWorkers,
			View.AssignedConstructionWorkers)));
	}
	if (StatusText) StatusText->SetText(FText::FromString(View.Status));
	if (DetailText) DetailText->SetText(FText::FromString(View.Detail));
}

void UAMSimPhase1StaffView::Close()
{
	AMSim::UIAudio::Play(this, EAMSimUISound::Back);
	ClosePanel();
}
