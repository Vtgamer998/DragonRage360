#version 450
// D24S8 resolve's CPU-order bytes are Dhi,Dmid,Dlo,S (xsdk gpu_resolve.c).
// A borrowed RGBA8 view therefore exposes stencil in alpha; no numeric depth decode.
layout(location=0) in vec2 v_uv;
layout(location=0) out vec4 o_color;
layout(set=0,binding=0) uniform sampler2D u_depth_bytes;
layout(push_constant) uniform Stencil { vec4 params; } pc;
void main() {
    float stencil=floor(texture(u_depth_bytes,v_uv).a*255.0+0.5);
    float shifted=floor(stencil/max(pc.params.x,1.0));
    float bit=shifted-2.0*floor(shifted*0.5);
    // x=0 clears all stencil bits; other passes restore one saved bit.
    if(pc.params.x>0.0 && bit<0.5) discard;
    o_color=vec4(0.0);
}
