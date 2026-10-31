#version 450
// SPDX-License-Identifier: AGPL-3.0-or-later
// Native adaptation of upstream 3ca419c port/src/gs/shaders/outline.frag.
layout(location=0) out vec4 o_color;
layout(set=0,binding=0) uniform sampler2D u_aux;
layout(push_constant) uniform Effect { vec4 params; vec4 bounds; } pc;
void main() {
    vec2 uv=gl_FragCoord.xy*pc.params.xy;
    float id_here=floor(texture(u_aux,min(max(uv,pc.bounds.xy),pc.bounds.zw)).r*255.0+0.5);
    float c_here=id_here*8.0-floor(id_here/32.0)*256.0;
    float here=id_here>254.5?0.0:c_here<0.5?128.0:c_here;
    float id_right=floor(texture(u_aux,min(max(uv+vec2(pc.params.x,0.0),pc.bounds.xy),pc.bounds.zw)).r*255.0+0.5);
    float c_right=id_right*8.0-floor(id_right/32.0)*256.0;
    float right=id_right>254.5?0.0:c_right<0.5?128.0:c_right;
    float id_left=floor(texture(u_aux,min(max(uv-vec2(pc.params.x,0.0),pc.bounds.xy),pc.bounds.zw)).r*255.0+0.5);
    float c_left=id_left*8.0-floor(id_left/32.0)*256.0;
    float left=id_left>254.5?0.0:c_left<0.5?128.0:c_left;
    float id_down=floor(texture(u_aux,min(max(uv+vec2(0.0,pc.params.y),pc.bounds.xy),pc.bounds.zw)).r*255.0+0.5);
    float c_down=id_down*8.0-floor(id_down/32.0)*256.0;
    float down=id_down>254.5?0.0:c_down<0.5?128.0:c_down;
    float id_up=floor(texture(u_aux,min(max(uv-vec2(0.0,pc.params.y),pc.bounds.xy),pc.bounds.zw)).r*255.0+0.5);
    float c_up=id_up*8.0-floor(id_up/32.0)*256.0;
    float up=id_up>254.5?0.0:c_up<0.5?128.0:c_up;
    bool edge=here>right || here>left || here>down || here>up;
    o_color=vec4(vec3(edge?pc.params.z:0.0),0.0);
}
