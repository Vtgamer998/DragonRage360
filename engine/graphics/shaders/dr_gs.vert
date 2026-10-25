#version 450
layout(location=0) in vec3 a_position;
layout(location=1) in vec4 a_color;
layout(location=2) in vec3 a_stq;
layout(location=0) out vec4 v_color;
layout(location=1) out vec3 v_stq;
layout(push_constant) uniform Push { vec4 extent; } pc;
layout(location=2) out vec2 v_depth;
void main() {
    gl_Position=vec4(a_position.x*pc.extent.x-1.0,1.0-a_position.y*pc.extent.y,a_position.z,1.0);
    v_color=a_color;
    v_stq=a_stq;
    v_depth=vec2(a_position.z,1.0);
}
