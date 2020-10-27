//
// Created by nhy20 on 2020-10-25.
//

#ifndef HARUCAR_UI_EVENT_PROTOCOL_STRUCTURE_H
#define HARUCAR_UI_EVENT_PROTOCOL_STRUCTURE_H


#include <string>
#include <string_view>
#include <unordered_map>

//이벤트 켜면 언젠간 가져가서 언젠간 Remove하는 굉장히 Lazy한 프로토콜. Viewer 와 Eventer가 교환할 때 쓴다.
//콜백 방식.. 써야 하나.. :thinking_face:. 넣는 건 안 어려운데 고민중.

namespace HaruCar::UI::Structure
{
	class UIEventProtocol
	{
	public:
		// if not add.
		void AddEvent( std::string_view event_name );

		// Get, Not Set To False
		[[nodiscard]] bool CheckEvent( std::string_view event_name ) const;

		// set to false.
		void RemoveEvent( std::string_view event_name );
	private:
		std::unordered_map<std::string_view, bool> mEvents;
	};

	namespace UIEventHelperFunction
	{
		// 버튼 같은 단발성 이벤트용.
		bool FireEvent( UIEventProtocol & ref_event_protocol, std::string_view event_name );
	}

}

#endif //HARUCAR_UI_EVENT_PROTOCOL_STRUCTURE_H
