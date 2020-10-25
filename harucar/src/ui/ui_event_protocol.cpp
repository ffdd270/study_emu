//
// Created by nhy20 on 2020-10-25.
//

#include <ui/ui_event_protocol.h>

void UIEventProtocol::AddEvent(std::string_view event_name)
{
	auto iter = mEvents.find( event_name );

	if( iter == mEvents.end() )
	{
		mEvents.insert( std::make_pair(  event_name, true  ) );
		return;
	}

	iter->second = true;
}

bool UIEventProtocol::CheckEvent(std::string_view event_name) const
{
	auto iter = mEvents.find( event_name );
	if( iter == mEvents.end() ) { return false; }
	return iter->second;
}

void UIEventProtocol::RemoveEvent(std::string_view event_name)
{
	auto iter = mEvents.find( event_name );
	if( iter == mEvents.end() ) { return; }
	iter->second = false;
}



