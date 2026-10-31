#version 450
layout(location=0) in vec2 v_uv;
layout(location=0) out vec4 o_color;
layout(location=1) out vec4 o_aux;
layout(set=0,binding=0) uniform sampler2D u_color;
layout(set=0,binding=1) uniform sampler2D u_depth;
layout(set=0,binding=2) uniform sampler2D u_aux;
void main() {
    o_color=texture(u_color,v_uv);
    o_aux=texture(u_aux,v_uv);
    gl_FragDepth=texture(u_depth,v_uv).r;
}
