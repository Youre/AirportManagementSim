#include "AMSimRootScreen.h"

#include "AMSimPhase1HudPresentation.h"
#include "AMSimUITheme.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Engine/UserInterfaceSettings.h"

int32 UAMSimRootScreen::GetLoadedNavigationIconCount() const
{
	int32 Count = 0;
	for (const UTexture2D* Icon : NavigationIcons)
	{
		Count += Icon ? 1 : 0;
	}
	return Count;
}

void UAMSimRootScreen::RefreshPhase1ContextPanel(
	const AMSim::FPhase1QuerySnapshot& Query,
	const AMSim::FPhase1State& State)
{
	if (!ContextPanel)
	{
		return;
	}

	const bool bOfferContext =
		Query.OfferState == AMSim::EOfferState::Available ||
		Query.OfferState == AMSim::EOfferState::Accepted;
	const bool bConstructionContext =
		Query.ConstructionStage >= AMSim::EConstructionStage::Funded &&
		Query.ConstructionStage < AMSim::EConstructionStage::ReadyToOpen;
	if (!bOfferContext && !bConstructionContext)
	{
		ContextPanel->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	FString Header;
	FString Body;
	FString Status;
	if (bConstructionContext)
	{
		const AMSim::FPhase1ConstructionActivityCard Card =
			AMSim::FPhase1HudPresentation::MakeConstructionActivityCard(
				Query.ConstructionStage,
				State.Project.Proposal);
		Header = Card.Header;
		Body = Card.Detail;
	}
	else
	{
		Header = TEXT("FIRST-FLIGHT OFFER  •  RIVERBEND");
		Body = TEXT("RIVERBEND 21  •  TRAINER  •  STAND A1");
		Status = TEXT("D1 00:10  •  45 MIN  •  600 CR + 5 AP");
	}

	ContextPanel->SetVisibility(ESlateVisibility::Visible);
	ContextHeaderText->SetText(FText::FromString(Header));
	ContextHeaderText->SetColorAndOpacity(FSlateColor(AMSim::UITheme::Cyan()));
	ContextBodyText->SetText(FText::FromString(Body));
	ContextStatusText->SetText(FText::FromString(Status));
	ContextCompactText->SetText(FText::FromString(Body));
	ContextCompactText->SetVisibility(
		bConstructionContext
			? ESlateVisibility::HitTestInvisible
			: ESlateVisibility::Collapsed);
	ContextDetailRowWidget->SetVisibility(
		bConstructionContext
			? ESlateVisibility::Collapsed
			: ESlateVisibility::SelfHitTestInvisible);

	const UUserInterfaceSettings* InterfaceSettings =
		GetDefault<UUserInterfaceSettings>();
	const float ContextScale = InterfaceSettings
		? InterfaceSettings->GetDPIScaleBasedOnSize(FIntPoint(1920, 1080))
		: 1.0f;
	const bool bCompactContext = ContextScale >= 1.75f;
	const bool bMediumContext = ContextScale >= 1.20f && !bCompactContext;
	ContextPanel->SetPadding(
		bConstructionContext
			? (bCompactContext ? FMargin(11.0f, 8.0f) : FMargin(13.0f, 10.0f))
			: (bCompactContext ? FMargin(12.0f, 9.0f) : FMargin(18.0f, 15.0f)));
	if (UCanvasPanelSlot* ContextSlot = Cast<UCanvasPanelSlot>(ContextPanel->Slot))
	{
		if (bConstructionContext)
		{
			ContextSlot->SetAnchors(
				bCompactContext
					? FAnchors(0.36f, 0.02f, 0.96f, 0.20f)
					: bMediumContext
						? FAnchors(0.58f, 0.02f, 0.98f, 0.15f)
						: FAnchors(0.72f, 0.02f, 0.985f, 0.12f));
		}
		else
		{
			ContextSlot->SetAnchors(
				bCompactContext
					? FAnchors(0.05f, 0.28f, 0.95f, 0.90f)
					: bMediumContext
						? FAnchors(0.08f, 0.45f, 0.92f, 0.89f)
						: FAnchors(0.10f, 0.57f, 0.90f, 0.82f));
		}
	}
}
