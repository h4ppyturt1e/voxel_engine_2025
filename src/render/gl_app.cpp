#include "gl_app.hpp"

#ifdef VOXEL_WITH_GL
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include "../core/logging.hpp"
#include "../core/math.hpp"
#include <cmath>

namespace render {

int run_demo(const mesh::Mesh& mesh) {
	if (!glfwInit()) {
		core::log(core::LogLevel::Error, "Failed to init GLFW");
		return -1;
	}
	GLFWwindow* window = glfwCreateWindow(800, 600, "Voxel Demo", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
		int w,h; glfwGetFramebufferSize(window, &w, &h);
		glViewport(0,0,w,h);
		glClearColor(0.1f,0.15f,0.2f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Simple orbit camera using fixed pipeline
		static float t = 0.0f; t += 0.01f;
		float aspect = (h==0) ? 1.f : (float)w/(float)h;
		core::Mat4 proj = core::perspective(60.0f * 3.14159265f/180.0f, aspect, 0.1f, 100.0f);
		float radius = 20.0f;
		float eyeX = std::cos(t)*radius;
		float eyeZ = std::sin(t)*radius;
		core::Mat4 view = core::lookAt(core::Vec3{eyeX, 10.0f, eyeZ}, core::Vec3{8.0f, 0.0f, 8.0f}, core::Vec3{0.0f, 1.0f, 0.0f});
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(proj.m);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(view.m);

		// Very simple directional light effect via vertex color based on normal
		glBegin(GL_TRIANGLES);
		for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3) {
			const auto i0 = mesh.indices[i+0];
			const auto i1 = mesh.indices[i+1];
			const auto i2 = mesh.indices[i+2];
			const auto& v0 = mesh.vertices[i0];
			const auto& v1 = mesh.vertices[i1];
			const auto& v2 = mesh.vertices[i2];
			float ndotl = std::max(0.0f, v0.nx*0.577f + v0.ny*0.577f + v0.nz*0.577f);
			glColor3f(0.2f + 0.8f*ndotl, 0.2f + 0.8f*ndotl, 0.2f + 0.8f*ndotl);
			glVertex3f(v0.x, v0.y, v0.z);
			glVertex3f(v1.x, v1.y, v1.z);
			glVertex3f(v2.x, v2.y, v2.z);
		}
		glEnd();

		glfwSwapBuffers(window);
		glfwPollEvents();
    }

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

} // namespace render
#endif


