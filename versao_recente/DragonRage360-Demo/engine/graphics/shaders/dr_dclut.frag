#version 450
// SPDX-License-Identifier: AGPL-3.0-or-later
// Native adaptation of upstream 3ca419c port/src/gs/shaders/dclut.frag.
// Uses pixel coordinates so the frozen alpha's allocated pitch stays 1280x1024.
layout(location=0) out vec4 o_color;
layout(location=1) out vec4 o_aux;
layout(set=0,binding=0) uniform sampler2D u_depth;
layout(set=0,binding=1) uniform sampler2D u_aux;
layout(set=0,binding=2) uniform sampler2D u_clut;
layout(push_constant) uniform Effect { vec4 params; vec4 extent; } pc;
void main() {
    float z=texture(u_depth,gl_FragCoord.xy*pc.extent.xy).r*pc.params.y;
    float fog=z>=65448.0?0.0:clamp(255.0-floor(z/256.0),0.0,255.0);
    float id=floor(texture(u_aux,gl_FragCoord.xy*pc.params.zw).r*255.0+0.5);
    float index=pc.params.x<0.5?fog:id;
    vec4 c=texture(u_clut,vec2((index+0.5)/256.0,0.5));
    float a=c.a*(255.0/128.0);
    o_color=clamp(vec4(pc.extent.z>0.0?c.rgb*a:c.rgb,a),0.0,1.0);
    o_aux=vec4(c.a,0.0,0.0,1.0);
}
