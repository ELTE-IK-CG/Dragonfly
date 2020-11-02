#include "ConsoleLogView.h"
void df::ConsoleLogView::LogArrived(df::LogManager::Instance* instance)
{
	printf("%s\n", instance->entry->message.c_str());
}
