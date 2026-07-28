#include "AMSimConstructionProposalView.h"

#include "AMSimAirportSimulationSubsystem.h"
#include "AMSimPhase1Fixture.h"
#include "AMSimUITheme.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

namespace AMSimConstructionProposalPrivate
{
	UTextBlock* Text(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const TCHAR* Value,
		const int32 Size,
		const FLinearColor& Color,
		const bool bBold = false)
	{
		UTextBlock* Result =
			Tree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(Name));
		Result->SetText(FText::FromString(Value));
		Result->SetAutoWrapText(true);
		AMSim::UITheme::StyleText(Result, Size, Color, bBold, true);
		return Result;
	}

	UBorder* Surface(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const AMSim::UITheme::ESurface Kind,
		const FMargin Padding = FMargin(12.0f))
	{
		UBorder* Result =
			Tree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(Name));
		AMSim::UITheme::StyleSurface(Result, Kind, Padding, 14.0f, 1.4f);
		return Result;
	}

	UBorder* ColorSurface(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FLinearColor& Fill,
		const FLinearColor& Stroke,
		const float StrokeWidth = 2.0f,
		const float Radius = 7.0f)
	{
		UBorder* Result =
			Tree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(Name));
		Result->SetBrush(
			AMSim::UITheme::RoundedBrush(Fill, Radius, Stroke, StrokeWidth));
		Result->SetPadding(FMargin(0.0f));
		return Result;
	}

	void AddVertical(UVerticalBox* Box, UWidget* Widget, const float Padding = 4.0f)
	{
		UVerticalBoxSlot* Slot = Box->AddChildToVerticalBox(Widget);
		Slot->SetPadding(FMargin(0.0f, Padding));
		Slot->SetHorizontalAlignment(HAlign_Fill);
	}

	void SetFill(UHorizontalBoxSlot* Slot, const float Value)
	{
		FSlateChildSize Size(ESlateSizeRule::Fill);
		Size.Value = Value;
		Slot->SetSize(Size);
	}

	UCanvasPanelSlot* Place(
		UCanvasPanel* Canvas,
		UWidget* Widget,
		const FAnchors& Anchors,
		const FMargin& Offsets = FMargin(),
		const int32 ZOrder = 0)
	{
		UCanvasPanelSlot* Slot = Canvas->AddChildToCanvas(Widget);
		Slot->SetAnchors(Anchors);
		Slot->SetOffsets(Offsets);
		Slot->SetZOrder(ZOrder);
		return Slot;
	}

	void PlaceCentered(
		UCanvasPanel* Canvas,
		UWidget* Widget,
		const FVector2D& Anchor,
		const int32 ZOrder = 2)
	{
		UCanvasPanelSlot* Slot = Place(
			Canvas,
			Widget,
			FAnchors(Anchor.X, Anchor.Y, Anchor.X, Anchor.Y),
			FMargin(),
			ZOrder);
		Slot->SetAlignment(FVector2D(0.5f, 0.5f));
		Slot->SetAutoSize(true);
	}

	UBorder* LabeledCard(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const TCHAR* Label,
		const TCHAR* Body,
		const AMSim::UITheme::ESurface Kind = AMSim::UITheme::ESurface::Card,
		const FLinearColor& LabelColor = AMSim::UITheme::Cyan())
	{
		UBorder* Card = Surface(Tree, Name, Kind, FMargin(10.0f, 7.0f));
		UVerticalBox* Column = Tree->ConstructWidget<UVerticalBox>();
		AddVertical(
			Column,
			Text(
				Tree,
				*(FString(Name) + TEXT("Label")),
				Label,
				10,
				LabelColor,
				true),
			0.0f);
		AddVertical(
			Column,
			Text(
				Tree,
				*(FString(Name) + TEXT("Body")),
				Body,
				12,
				AMSim::UITheme::White(),
				true),
			1.0f);
		Card->SetContent(Column);
		return Card;
	}

	UCanvasPanel* PatternedRegion(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FLinearColor& Fill,
		const FLinearColor& Stroke,
		const TCHAR* Label,
		const int32 StripeCount)
	{
		UCanvasPanel* Region =
			Tree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), FName(Name));
		Place(
			Region,
			ColorSurface(
				Tree,
				*(FString(Name) + TEXT("Base")),
				Fill,
				Stroke),
			FAnchors(0.0f, 0.0f, 1.0f, 1.0f));

		for (int32 Index = 0; Index < StripeCount; ++Index)
		{
			const float Left = 0.04f + Index * (0.90f / StripeCount);
			Place(
				Region,
				ColorSurface(
					Tree,
					*FString::Printf(TEXT("%sStripe%d"), Name, Index),
					FLinearColor(1.0f, 1.0f, 1.0f, 0.16f),
					FLinearColor::Transparent,
					0.0f,
					0.0f),
				FAnchors(Left, 0.08f, Left + 0.025f, 0.92f),
				FMargin(),
				1);
		}

		UTextBlock* RegionLabel = Text(
			Tree,
			*(FString(Name) + TEXT("Label")),
			Label,
			11,
			AMSim::UITheme::White(),
			true);
		RegionLabel->SetJustification(ETextJustify::Center);
		PlaceCentered(Region, RegionLabel, FVector2D(0.5f, 0.5f), 3);
		return Region;
	}

	void AddSelectionHandles(
		UWidgetTree* Tree,
		UCanvasPanel* Region,
		const TCHAR* Prefix,
		const FLinearColor& Color)
	{
		const FVector2D Anchors[] = {
			FVector2D(0.0f, 0.0f),
			FVector2D(1.0f, 0.0f),
			FVector2D(0.0f, 1.0f),
			FVector2D(1.0f, 1.0f)};
		for (int32 Index = 0; Index < UE_ARRAY_COUNT(Anchors); ++Index)
		{
			UBorder* Handle = ColorSurface(
				Tree,
				*FString::Printf(TEXT("%sHandle%d"), Prefix, Index),
				AMSim::UITheme::White(),
				Color,
				3.0f,
				5.0f);
			UCanvasPanelSlot* Slot = Place(
				Region,
				Handle,
				FAnchors(
					Anchors[Index].X,
					Anchors[Index].Y,
					Anchors[Index].X,
					Anchors[Index].Y),
				FMargin(-7.0f, -7.0f, 14.0f, 14.0f),
				4);
			Slot->SetAlignment(FVector2D(0.0f, 0.0f));
		}
	}
}

bool UAMSimConstructionProposalView::IsProposalOpen() const
{
	return GetVisibility() != ESlateVisibility::Collapsed;
}

void UAMSimConstructionProposalView::OpenProposal()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UAMSimConstructionProposalView::CloseProposal()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

AMSim::FStarterPlanProposal
UAMSimConstructionProposalView::MakePresentationProposal(
	const bool bConflictVariant)
{
	AMSim::FStarterPlanProposal Proposal = AMSim::CreateDefaultStarterPlan();
	if (bConflictVariant)
	{
		// The proof uses the real Phase 1 validator: 180 m of the runway starts
		// outside the owned parcel while every other authored dimension remains
		// lifecycle-valid.
		Proposal.RunwayStart.X = -18000;
	}
	return Proposal;
}

TSharedRef<SWidget> UAMSimConstructionProposalView::RebuildWidget()
{
	using namespace AMSimConstructionProposalPrivate;
	const bool bConflictProof =
		FParse::Param(FCommandLine::Get(), TEXT("AMSimPhase45ConstructionProof"));
	CurrentProposal = MakePresentationProposal(bConflictProof);
	CurrentValidation = AMSim::ValidateStarterPlan(CurrentProposal);
	WidgetTree = NewObject<UWidgetTree>(this, TEXT("ConstructionProposalTree"));
	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(
		UCanvasPanel::StaticClass(),
		TEXT("ConstructionProposalRoot"));
	WidgetTree->RootWidget = Root;

	UBorder* ModeHeader = Surface(
		WidgetTree,
		TEXT("BuildModeHeader"),
		AMSim::UITheme::ESurface::Chrome,
		FMargin(14.0f, 9.0f));
	UHorizontalBox* HeaderRow = WidgetTree->ConstructWidget<UHorizontalBox>();
	ModeHeader->SetContent(HeaderRow);
	UTextBlock* HeaderTitle = Text(
		WidgetTree,
		TEXT("BuildModeTitle"),
		TEXT("BUILD MODE  |  STARTER AIRFIELD"),
		14,
		AMSim::UITheme::White(),
		true);
	UHorizontalBoxSlot* HeaderTitleSlot =
		HeaderRow->AddChildToHorizontalBox(HeaderTitle);
	SetFill(HeaderTitleSlot, 0.64f);
	UTextBlock* HeaderState = Text(
		WidgetTree,
		TEXT("BuildModeState"),
		bConflictProof
			? TEXT("INVALID  |  NOT COMMITTED")
			: TEXT("VALID PROPOSAL  |  NOT COMMITTED"),
		11,
		bConflictProof ? AMSim::UITheme::Coral() : AMSim::UITheme::Green(),
		true);
	HeaderState->SetJustification(ETextJustify::Right);
	UHorizontalBoxSlot* HeaderStateSlot =
		HeaderRow->AddChildToHorizontalBox(HeaderState);
	SetFill(HeaderStateSlot, 0.36f);
	Place(Root, ModeHeader, FAnchors(0.25f, 0.01f, 0.75f, 0.085f), FMargin(), 8);

	UBorder* Palette = Surface(
		WidgetTree,
		TEXT("BuildPalette"),
		AMSim::UITheme::ESurface::Panel,
		FMargin(12.0f, 10.0f));
	UVerticalBox* PaletteColumn = WidgetTree->ConstructWidget<UVerticalBox>();
	Palette->SetContent(PaletteColumn);
	AddVertical(
		PaletteColumn,
		Text(
			WidgetTree,
			TEXT("PaletteTitle"),
			TEXT("CONSTRUCTION PALETTE"),
			14,
			AMSim::UITheme::White(),
			true),
		0.0f);
	AddVertical(
		PaletteColumn,
		Text(
			WidgetTree,
			TEXT("PaletteHint"),
			TEXT("LIFECYCLE-VALID STARTER KIT"),
			10,
			AMSim::UITheme::Muted(),
			true),
		1.0f);
	for (const TPair<const TCHAR*, const TCHAR*>& Item : {
		TPair<const TCHAR*, const TCHAR*>(TEXT("PaletteRunway"), TEXT("RUNWAY|09/27  -  GRASS  -  800 M")),
		TPair<const TCHAR*, const TCHAR*>(TEXT("PaletteTaxiway"), TEXT("TAXIWAY|TAXI A  -  90 M LINK")),
		TPair<const TCHAR*, const TCHAR*>(TEXT("PaletteStand"), TEXT("STAND|A1  -  LIGHT AIRCRAFT")),
		TPair<const TCHAR*, const TCHAR*>(TEXT("PaletteRoute"), TEXT("SERVICE ROUTE|ACCESS + SAFE APPROACH")),
		TPair<const TCHAR*, const TCHAR*>(TEXT("PaletteMarking"), TEXT("MARKINGS|THRESHOLD + HOLD LINE")),
		TPair<const TCHAR*, const TCHAR*>(TEXT("PaletteFacility"), TEXT("FACILITY|OPERATIONS HUT"))})
	{
		TArray<FString> Parts;
		FString(Item.Value).ParseIntoArray(Parts, TEXT("|"));
		AddVertical(
			PaletteColumn,
			LabeledCard(
				WidgetTree,
				Item.Key,
				*Parts[0],
				*Parts[1],
				FString(Item.Key) == TEXT("PaletteRunway")
					? AMSim::UITheme::ESurface::RaisedCard
					: AMSim::UITheme::ESurface::Card),
			2.0f);
	}
	Place(Root, Palette, FAnchors(0.01f, 0.03f, 0.225f, 0.91f), FMargin(), 8);

	UBorder* Summary = Surface(
		WidgetTree,
		TEXT("ProposalSummary"),
		AMSim::UITheme::ESurface::Panel,
		FMargin(12.0f, 10.0f));
	UVerticalBox* SummaryColumn = WidgetTree->ConstructWidget<UVerticalBox>();
	Summary->SetContent(SummaryColumn);
	AddVertical(
		SummaryColumn,
		Text(
			WidgetTree,
			TEXT("ProposalSummaryTitle"),
			TEXT("RUNWAY 09/27 PROPOSAL"),
			14,
			AMSim::UITheme::White(),
			true),
		0.0f);
	AddVertical(
		SummaryColumn,
		LabeledCard(
			WidgetTree,
			TEXT("ValidationCard"),
			CurrentValidation.bValid
				? TEXT("VALID PLACEMENT")
				: TEXT("INVALID PLACEMENT"),
			CurrentValidation.bValid
				? TEXT("PARCEL FIT  /  TAXI CLEAR  /  STAND CLEAR")
				: TEXT("OWNED PARCEL  /  180 M OVERLAP"),
			CurrentValidation.bValid
				? AMSim::UITheme::ESurface::Positive
				: AMSim::UITheme::ESurface::Danger,
			CurrentValidation.bValid
				? AMSim::UITheme::Green()
				: AMSim::UITheme::Coral()),
		3.0f);
	if (!CurrentValidation.bValid)
	{
		AddVertical(
			SummaryColumn,
			LabeledCard(
				WidgetTree,
				TEXT("ValidationCauseCard"),
				TEXT("VALIDATOR RESULT"),
				*CurrentValidation.Cause,
				AMSim::UITheme::ESurface::Danger,
				AMSim::UITheme::Coral()),
			3.0f);
	}
	AddVertical(
		SummaryColumn,
		LabeledCard(
			WidgetTree,
			TEXT("ConflictCard"),
			CurrentValidation.bValid
				? TEXT("0 CONFLICTS")
				: TEXT("1 LOCALIZED CONFLICT"),
			CurrentValidation.bValid
				? TEXT("CLEAR OF BOUNDARY + SERVICE ROUTE")
				: TEXT("OUTSIDE OWNED LAND  /  CORAL STRIPE"),
			CurrentValidation.bValid
				? AMSim::UITheme::ESurface::Positive
				: AMSim::UITheme::ESurface::Danger,
			CurrentValidation.bValid
				? AMSim::UITheme::Green()
				: AMSim::UITheme::Coral()),
		3.0f);
	AddVertical(
		SummaryColumn,
		LabeledCard(
			WidgetTree,
			TEXT("ClosureCard"),
			TEXT("OPERATIONAL CLOSURE"),
			TEXT("NONE  /  STARTER SITE IS NOT OPEN"),
			AMSim::UITheme::ESurface::Card,
			AMSim::UITheme::Cyan()),
		3.0f);
	AddVertical(
		SummaryColumn,
		LabeledCard(
			WidgetTree,
			TEXT("ResourcesCard"),
			TEXT("COST + RESOURCES"),
			TEXT("3,400 CR  |  STARTER KIT  |  FIELD CREW")),
		3.0f);
	AddVertical(
		SummaryColumn,
		LabeledCard(
			WidgetTree,
			TEXT("CompatibilityCard"),
			TEXT("COMPATIBILITY"),
			TEXT("STARTER FLEET  100%  |  WIND 09/27")),
		3.0f);
	AddVertical(
		SummaryColumn,
		LabeledCard(
			WidgetTree,
			TEXT("CapabilityCard"),
			TEXT("CAPABILITY GAINED"),
			TEXT("LIGHT AIRCRAFT ARRIVAL + DEPARTURE")),
		3.0f);
	Place(Root, Summary, FAnchors(0.775f, 0.03f, 0.99f, 0.91f), FMargin(), 8);

	UCanvasPanel* ValidGeometry = PatternedRegion(
		WidgetTree,
		TEXT("ValidProposalGeometry"),
		FLinearColor(0.08f, 0.45f, 0.62f, 0.46f),
		AMSim::UITheme::Cyan(),
		bConflictProof
			? TEXT("PROPOSED RUNWAY  /  CLEAR 620 M")
			: TEXT("VALID PROPOSED RUNWAY  /  800 M"),
		12);
	AddSelectionHandles(
		WidgetTree,
		ValidGeometry,
		TEXT("ValidProposal"),
		AMSim::UITheme::Cyan());
	Place(
		Root,
		ValidGeometry,
		bConflictProof
			? FAnchors(0.43f, 0.225f, 0.75f, 0.315f)
			: FAnchors(0.29f, 0.225f, 0.75f, 0.315f),
		FMargin(),
		3);

	if (bConflictProof)
	{
		UCanvasPanel* ConflictSegment = PatternedRegion(
			WidgetTree,
			TEXT("LocalizedConflictGeometry"),
			FLinearColor(0.52f, 0.07f, 0.05f, 0.62f),
			AMSim::UITheme::Coral(),
			TEXT("OUTSIDE PARCEL  /  180 M"),
			7);
		AddSelectionHandles(
			WidgetTree,
			ConflictSegment,
			TEXT("Conflict"),
			AMSim::UITheme::Coral());
		Place(
			Root,
			ConflictSegment,
			FAnchors(0.29f, 0.225f, 0.43f, 0.315f),
			FMargin(),
			5);
	}

	UCanvasPanel* TaxiGeometry = PatternedRegion(
		WidgetTree,
		TEXT("ValidTaxiGeometry"),
		FLinearColor(0.08f, 0.45f, 0.62f, 0.40f),
		AMSim::UITheme::Cyan(),
		TEXT("TAXI A"),
		5);
	Place(
		Root,
		TaxiGeometry,
		FAnchors(0.47f, 0.315f, 0.56f, 0.59f),
		FMargin(),
		3);

	UCanvasPanel* WorkZoneGeometry = PatternedRegion(
		WidgetTree,
		TEXT("ProtectedWorkZoneGeometry"),
		FLinearColor(0.58f, 0.33f, 0.04f, 0.68f),
		AMSim::UITheme::Amber(),
		TEXT("PROTECTED WORK ZONE"),
		8);
	Place(
		Root,
		WorkZoneGeometry,
		FAnchors(0.54f, 0.50f, 0.72f, 0.60f),
		FMargin(),
		6);

	UBorder* ConsequenceLabel = Surface(
		WidgetTree,
		TEXT("ProposalConsequenceLabel"),
		bConflictProof
			? AMSim::UITheme::ESurface::Danger
			: AMSim::UITheme::ESurface::Positive,
		FMargin(10.0f, 6.0f));
	ConsequenceLabel->SetContent(
		Text(
			WidgetTree,
			TEXT("ProposalConsequenceText"),
			bConflictProof
				? TEXT("PLACEMENT BLOCKED  |  MOVE INSIDE THE OWNED PARCEL")
				: TEXT("NO OPERATIONAL CLOSURE  |  STARTER SITE IS UNOPENED"),
			10,
			AMSim::UITheme::White(),
			true));
	Place(
		Root,
		ConsequenceLabel,
		FAnchors(0.31f, 0.125f, 0.69f, 0.185f),
		FMargin(),
		7);

	UBorder* ActionBar = Surface(
		WidgetTree,
		TEXT("ProposalActionBar"),
		AMSim::UITheme::ESurface::Chrome,
		FMargin(10.0f, 8.0f));
	UHorizontalBox* Actions = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("ProposalActions"));
	ActionBar->SetContent(Actions);
	ConfirmButton = WidgetTree->ConstructWidget<UButton>(
		UButton::StaticClass(),
		TEXT("ConfirmProposal"));
	ConfirmButton->SetStyle(
		AMSim::UITheme::ButtonStyle(AMSim::UITheme::EButton::Positive));
	ConfirmButton->SetContent(
		Text(
			WidgetTree,
			TEXT("ConfirmLabel"),
			bConflictProof
				? TEXT("RESOLVE CONFLICT TO CONFIRM")
				: TEXT("CONFIRM + FUND  |  3,400 CR"),
			13,
			AMSim::UITheme::White(),
			true));
	ConfirmButton->SetIsEnabled(CurrentValidation.bValid);
	ConfirmButton->OnClicked.AddDynamic(
		this,
		&UAMSimConstructionProposalView::ConfirmProposal);
	UHorizontalBoxSlot* ConfirmSlot =
		Actions->AddChildToHorizontalBox(ConfirmButton);
	SetFill(ConfirmSlot, 0.58f);
	ConfirmSlot->SetPadding(FMargin(0.0f, 0.0f, 6.0f, 0.0f));

	UButton* EditButton = WidgetTree->ConstructWidget<UButton>(
		UButton::StaticClass(),
		TEXT("KeepEditing"));
	EditButton->SetStyle(
		AMSim::UITheme::ButtonStyle(AMSim::UITheme::EButton::Secondary));
	EditButton->SetContent(
		Text(
			WidgetTree,
			TEXT("EditLabel"),
			TEXT("KEEP PLANNING"),
			12,
			AMSim::UITheme::White(),
			true));
	EditButton->OnClicked.AddDynamic(
		this,
		&UAMSimConstructionProposalView::KeepEditing);
	UHorizontalBoxSlot* EditSlot = Actions->AddChildToHorizontalBox(EditButton);
	SetFill(EditSlot, 0.42f);
	Place(
		Root,
		ActionBar,
		FAnchors(0.30f, 0.86f, 0.70f, 0.975f),
		FMargin(),
		9);

	CloseProposal();
	return Root->TakeWidget();
}

void UAMSimConstructionProposalView::ConfirmProposal()
{
	if (UWorld* World = GetWorld())
	{
		if (UAMSimAirportSimulationSubsystem* Subsystem =
			World->GetSubsystem<UAMSimAirportSimulationSubsystem>())
		{
			AMSim::FPhase1Command Command;
			Command.Type = AMSim::EPhase1CommandType::CommitStarterPlan;
			Command.Proposal = CurrentProposal;
			if (Subsystem->SubmitPhase1Command(MoveTemp(Command)) ==
				AMSim::EPhase1CommandResult::Accepted)
			{
				CloseProposal();
			}
		}
	}
}

void UAMSimConstructionProposalView::KeepEditing()
{
	CloseProposal();
}
