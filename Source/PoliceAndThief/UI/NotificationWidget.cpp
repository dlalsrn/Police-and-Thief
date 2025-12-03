#include "UI/NotificationWidget.h"
#include "Components/TextBlock.h"

void UNotificationWidget::SetNotificationText(const FText& NewText)
{
	NotificationText->SetText(NewText);
}
