uniform sampler2D texture;                                      
uniform float WIDTH;                                            
vec4 weight = vec4(0.006, 0.061, 0.242, 0.383);                 
float WIDTH_STEP = 1.0 / WIDTH;                                 
void main()                                                     
{                                                               
	vec2 pos = gl_TexCoord[0].xy;                               
	vec2 offset = vec2(WIDTH_STEP, 0.0);                        
	vec4 color = texture2D(texture, pos) * weight[3];           
	color += texture2D(texture, pos + offset * 1.5) * weight[2];
	color += texture2D(texture, pos + offset * 2.5) * weight[1];
	color += texture2D(texture, pos + offset * 3.5) * weight[0];
	color += texture2D(texture, pos - offset * 1.5) * weight[2];
	color += texture2D(texture, pos - offset * 2.5) * weight[1];
	color += texture2D(texture, pos - offset * 3.5) * weight[0];
	gl_FragColor = vec4(color.xyz, 1.0);                        
}