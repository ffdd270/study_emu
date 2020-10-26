//
// Created by nhy20 on 2020-10-25.
//

#include <ui/ui_event_protocol_structure.h>

using namespace HaruCar::UI::Structure;

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


bool UIEventHelperFunction::FireEvent(UIEventProtocol &ref_event_protocol, std::string_view event_name)
{
	bool rtn_flag = ref_event_protocol.CheckEvent( event_name );
	ref_event_protocol.RemoveEvent( event_name );

	return rtn_flag;
}
