#pragma once

#include "CoreMinimal.h"

namespace AMSim
{
	struct AMSIMUI_API FTimetableCardGeometry
	{
		float Left = 0.0f;
		float Top = 0.0f;
		float Right = 0.0f;
		float Bottom = 0.0f;
	};

	struct AMSIMUI_API FTimetableGeometry
	{
		static constexpr int32 FirstVisibleMinute = 6 * 60;
		static constexpr int32 LastVisibleMinute = 22 * 60;
		static constexpr float HeaderBottom = 0.205f;
		static constexpr float GridBottom = 0.895f;

		static float MinuteToAnchor(int32 Minute);
		static float DayColumnLeft(int32 DayIndex);
		static float DayColumnRight(int32 DayIndex);
		static FTimetableCardGeometry MakeCard(
			int32 DayIndex,
			int32 ArrivalMinute,
			int32 DepartureMinute,
			bool bCompact);
	};
}
