R"zzz(#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;
void main() {
	int n = 0;
	for (n = 0; n < gl_in.length(); n++) {
		gl_Position = gl_in[n].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}
)zzz"
