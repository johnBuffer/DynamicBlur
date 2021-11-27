void main()
{
	vec2 pos = gl_TexCoord[0].xy;
	gl_FragColor = vec3(pos.xy, 0.0f);
}