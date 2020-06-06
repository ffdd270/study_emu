//
// Created by HaruGakkaP on 2020-05-29.
//

#ifndef GAMEBOY_TYPEDEF_H
#define GAMEBOY_TYPEDEF_H

namespace Angelica
{
	struct SimpleRect
	{
		float width, height;
	};

	struct Vec2
	{
		float x, y;
	};

	struct Vec4
	{
		float x, y, z, w;
	};

	struct Matrix4x4
	{
		Matrix4x4()
		{
			memset( &mat, 0, sizeof(mat) );
		}

		Matrix4x4( const float dest_mat[4][4] )
		{
			for ( int i = 0; i < 4; i++ )
			{
				for ( int j = 0; j < 4; j++)
				{
					mat[i][j] = dest_mat[i][j];
				}
			}
		}

		union
		{
			float mat[4][4];
			struct
			{
				float _01, _02, _03, _04;
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
			};
		};

	};
}


#endif //GAMEBOY_TYPEDEF_H
