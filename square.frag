
/*************************************************************
Name: Kenny Moser	krm104@gmail.com
Course: ECE8990
File:	ground.frag

This is the fragment shader for transitioning the ground
between seasons
*************************************************************/
#version 120

uniform sampler2D Texture;

void main( )
{
	vec4 temp_color = texture2D(Texture, gl_TexCoord[0].st);
	gl_FragColor =  temp_color;//vec4( 0.0, 0.0, 1.0, 1.0);
}

