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

    // simple camera state
    double lastX = 0.0, lastY = 0.0; bool dragging = false;
    float yaw = 0.0f, pitch = -0.5f;
    float camX = 8.0f, camY = 10.0f, camZ = 28.0f; // eye position

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetCursorPos(window, 0, 0);

    while (!glfwWindowShouldClose(window)) {
        // input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            if (!dragging) { dragging = true; glfwGetCursorPos(window, &lastX, &lastY); }
            double x, y; glfwGetCursorPos(window, &x, &y);
            double dx = x - lastX, dy = y - lastY; lastX = x; lastY = y;
            yaw   += static_cast<float>(dx) * 0.01f;
            pitch -= static_cast<float>(dy) * 0.01f; // inverted mouse Y
            if (pitch < -1.5f) pitch = -1.5f;
            if (pitch > 1.5f)  pitch = 1.5f;
        } else {
            dragging = false;
        }
        float moveSpeed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? 0.6f : 0.2f;
        // compute facing vectors from yaw/pitch
        float cp = std::cos(pitch), sp = std::sin(pitch);
        float cy = std::cos(yaw),   sy = std::sin(yaw);
        // forward includes pitch (free-fly)
        float fwdX = cp * cy;
        float fwdY = sp;
        float fwdZ = cp * sy;
        // right is horizontal strafe
        float rightX = -sy;
        float rightZ =  cy;
        // WASD free-fly movement in facing direction
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { camX += fwdX * moveSpeed; camY += fwdY * moveSpeed; camZ += fwdZ * moveSpeed; }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { camX -= fwdX * moveSpeed; camY -= fwdY * moveSpeed; camZ -= fwdZ * moveSpeed; }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { camX -= rightX * moveSpeed;                         camZ -= rightZ * moveSpeed; }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { camX += rightX * moveSpeed;                         camZ += rightZ * moveSpeed; }
        // Q/E dolly along forward (closer/farther)
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { camX += fwdX * moveSpeed; camY += fwdY * moveSpeed; camZ += fwdZ * moveSpeed; }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { camX -= fwdX * moveSpeed; camY -= fwdY * moveSpeed; camZ -= fwdZ * moveSpeed; }
		int w,h; glfwGetFramebufferSize(window, &w, &h);
		glViewport(0,0,w,h);
		glClearColor(0.1f,0.15f,0.2f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Simple orbit camera using fixed pipeline
		static float t = 0.0f; t += 0.01f;
        float aspect = (h==0) ? 1.f : (float)w/(float)h;
        core::Mat4 proj = core::perspective(60.0f * 3.14159265f/180.0f, aspect, 0.1f, 500.0f);
        // look towards forward direction from camera
        float centerX = camX + fwdX;
        float centerY = camY + fwdY;
        float centerZ = camZ + fwdZ;
        core::Mat4 view = core::lookAt(core::Vec3{camX, camY, camZ}, core::Vec3{centerX, centerY, centerZ}, core::Vec3{0.0f, 1.0f, 0.0f});
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


