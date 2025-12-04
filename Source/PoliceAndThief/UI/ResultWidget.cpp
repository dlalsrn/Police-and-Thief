#include "UI/ResultWidget.h"

#include "Components/TextBlock.h"

void UResultWidget::SetResultText(const FText& NewText)
{
	ResultText->SetText(NewText);
}