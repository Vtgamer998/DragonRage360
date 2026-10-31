#version 450
// GS equations: upstream port/src/gs/shaders/gs.frag, revision 3ca419c.
// v_color uses 1.0 = 128, matching the native core's vertex stream.
layout(location=0) in vec4 v_color;
layout(location=1) in vec3 v_stq;
layout(location=2) in vec2 v_depth;
layout(location=0) out vec4 o_color;
layout(location=1) out vec4 o_aux;
layout(set=0,binding=0) uniform sampler2D u_texture;
layout(set=0,binding=1) uniform sampler2D u_date;
layout(push_constant) uniform Material { vec4 mode; vec4 test; vec4 snapshot_extent; vec4 rect; } pc;
void main() {
    vec2 uv=v_stq.xy/v_stq.z;
    vec4 texel=texture(u_texture,uv);
    float ta=texel.a*(255.0/128.0);
    vec3 modulate=texel.rgb*v_color.rgb;
    vec3 highlight=modulate+vec3(v_color.a*(128.0/255.0));
    vec3 textured_rgb=pc.mode.y==0.0?modulate:pc.mode.y==1.0?texel.rgb:highlight;
    vec3 rgb=pc.mode.x==0.0?v_color.rgb*(128.0/255.0):textured_rgb;
    float textured_a=pc.mode.y==0.0?ta*v_color.a:pc.mode.y==2.0?ta+v_color.a:ta;
    float a=pc.mode.x==0.0?v_color.a:pc.mode.z==0.0?v_color.a:textured_a;
    float ag=floor(a*128.0+0.5),ref=pc.test.x,f=pc.mode.w;
    float lt=ag<ref?1.0:0.0,le=ag<=ref?1.0:0.0,eq=abs(ag-ref)<0.5?1.0:0.0;
    float ge=ag>=ref?1.0:0.0,gt=ag>ref?1.0:0.0,ne=abs(ag-ref)>=0.5?1.0:0.0;
    float pass=f==0.0?1.0:f==1.0?0.0:f==2.0?1.0:f==3.0?lt:f==4.0?le:f==5.0?eq:f==6.0?ge:f==7.0?gt:ne;
    // gs_draw.c splits AFAIL into draws with their own depth/channel masks.
    float stored=texture(u_date,gl_FragCoord.xy*pc.snapshot_extent.xy).r;
    float date_pass=pc.test.z==0.0?1.0:pc.test.z==1.0?(stored<127.5/255.0?1.0:0.0):(stored>=127.5/255.0?1.0:0.0);
    if(pass*date_pass<0.5) discard;
    rgb=pc.test.y>0.0?rgb*a:rgb;
    o_color=clamp(vec4(rgb,a),0.0,1.0);
    float ab=floor(clamp(a*128.0,0.0,255.0)+0.5);
    float with_bit=ab<128.0?ab+128.0:ab;
    o_aux=vec4((pc.test.w>0.0?with_bit:ab)/255.0,0.0,0.0,1.0);
    float depth=v_depth.x/v_depth.y;
    float steps=pc.snapshot_extent.z;
    gl_FragDepth=steps>0.0?floor(depth*steps)/steps:depth;
}
