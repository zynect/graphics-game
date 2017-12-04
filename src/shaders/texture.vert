R"zzz(
#version 330 core
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec2 vertexUV;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

void main() {
	gl_Position = projection * view * model * vertex_position;

    // UV of the vertex. No special space for this one.
    UV = vertexUV;
}

)zzz"
