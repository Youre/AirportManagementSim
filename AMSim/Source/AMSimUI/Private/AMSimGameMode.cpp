#include "AMSimGameMode.h"
#include "AMSimCameraPawn.h"
#include "AMSimHUD.h"

AAMSimGameMode::AAMSimGameMode()
{
	DefaultPawnClass = AAMSimCameraPawn::StaticClass();
	HUDClass = AAMSimHUD::StaticClass();
}
