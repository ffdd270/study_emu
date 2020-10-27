//
// Created by nhy20 on 2020-10-26.
//

#ifndef HARUCAR_UTIL_STRING_H
#define HARUCAR_UTIL_STRING_H

#include <vector>
#include <string>
#include <string_view>

namespace HaruCar::Util
{
	inline std::vector<std::string> string_split( std::string_view view, char spliter = '\n')
	{
		std::vector<int> spliter_pos_vector;
		std::vector<std::string> split_string;

		for ( int i = 0; i < view.size(); i++ )
		{
			if ( view[i] == spliter )
			{
				spliter_pos_vector.emplace_back( i );
			}
		}

		spliter_pos_vector.emplace_back(view.size()); // 마지막 스플리터.

		for (int i = 0; i < spliter_pos_vector.size(); i++ )
		{
			int prv_pos = ( i == 0 ) ? 0 : ( spliter_pos_vector[ i - 1 ] + 1 ) ;
			int pos = spliter_pos_vector[ i ];

			split_string.emplace_back( view.substr( prv_pos, pos - prv_pos ) );
		}


		return split_string;
	}
}


#endif //HARUCAR_UTIL_STRING_H
