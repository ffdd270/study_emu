#version 420 core

in vec2 position;
uniform mat4 proj_ortho;

void main( void )
{
    gl_Position = proj_ortho * vec4( position.xy, 0.0, 1.0 );
}