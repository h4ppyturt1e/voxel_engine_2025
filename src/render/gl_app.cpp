#include "gl_app.hpp"

#ifdef VOXEL_WITH_GL
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include "../core/logging.hpp"
#include "../core/math.hpp"
#include <cmath>
#include "../voxel/world.hpp"
#include "../mesh/greedy_mesher.hpp"

namespace render {

struct RayHit { int x, y, z; int nx, ny, nz; bool hit; };

static RayHit raycastVoxel(const voxel::Chunk& chunk, float ox, float oy, float oz, float dx, float dy, float dz, float maxDist) {
    RayHit r{0,0,0,0,0,0,false};
    const int sx = chunk.sizeX();
    const int sy = chunk.sizeY();
    const int sz = chunk.sizeZ();

    int x = (int)std::floor(ox);
    int y = (int)std::floor(oy);
    int z = (int)std::floor(oz);

    float t = 0.0f;
    int stepX = (dx > 0) ? 1 : (dx < 0 ? -1 : 0);
    int stepY = (dy > 0) ? 1 : (dy < 0 ? -1 : 0);
    int stepZ = (dz > 0) ? 1 : (dz < 0 ? -1 : 0);

    float tDeltaX = (dx != 0.0f) ? std::abs(1.0f / dx) : 1e30f;
    float tDeltaY = (dy != 0.0f) ? std::abs(1.0f / dy) : 1e30f;
    float tDeltaZ = (dz != 0.0f) ? std::abs(1.0f / dz) : 1e30f;

    float nextVoxelBoundaryX = (stepX > 0) ? (std::floor(ox) + 1.0f) : std::floor(ox);
    float nextVoxelBoundaryY = (stepY > 0) ? (std::floor(oy) + 1.0f) : std::floor(oy);
    float nextVoxelBoundaryZ = (stepZ > 0) ? (std::floor(oz) + 1.0f) : std::floor(oz);

    float tMaxX = (dx != 0.0f) ? (nextVoxelBoundaryX - ox) / dx : 1e30f; if (tMaxX < 0) tMaxX = 0;
    float tMaxY = (dy != 0.0f) ? (nextVoxelBoundaryY - oy) / dy : 1e30f; if (tMaxY < 0) tMaxY = 0;
    float tMaxZ = (dz != 0.0f) ? (nextVoxelBoundaryZ - oz) / dz : 1e30f; if (tMaxZ < 0) tMaxZ = 0;

    int lastNx=0,lastNy=0,lastNz=0;

    while (t <= maxDist) {
        if (x>=0 && y>=0 && z>=0 && x<sx && y<sy && z<sz) {
            if (chunk.at(x,y,z).type != voxel::BlockType::Air) {
                r = {x,y,z,lastNx,lastNy,lastNz,true};
                return r;
            }
        }
        if (tMaxX < tMaxY) {
            if (tMaxX < tMaxZ) { x += stepX; t = tMaxX; tMaxX += tDeltaX; lastNx = -stepX; lastNy = 0; lastNz = 0; }
            else { z += stepZ; t = tMaxZ; tMaxZ += tDeltaZ; lastNx = 0; lastNy = 0; lastNz = -stepZ; }
        } else {
            if (tMaxY < tMaxZ) { y += stepY; t = tMaxY; tMaxY += tDeltaY; lastNx = 0; lastNy = -stepY; lastNz = 0; }
            else { z += stepZ; t = tMaxZ; tMaxZ += tDeltaZ; lastNx = 0; lastNy = 0; lastNz = -stepZ; }
        }
    }
    return r;
}

static void drawWireCube(int x, int y, int z, float r, float g, float b) {
    const float x0=x, y0=y, z0=z, x1=x+1, y1=y+1, z1=z+1;
    glDisable(GL_DEPTH_TEST);
    glColor3f(r,g,b);
    glBegin(GL_LINES);
    // bottom rectangle
    glVertex3f(x0,y0,z0); glVertex3f(x1,y0,z0);
    glVertex3f(x1,y0,z0); glVertex3f(x1,y0,z1);
    glVertex3f(x1,y0,z1); glVertex3f(x0,y0,z1);
    glVertex3f(x0,y0,z1); glVertex3f(x0,y0,z0);
    // top rectangle
    glVertex3f(x0,y1,z0); glVertex3f(x1,y1,z0);
    glVertex3f(x1,y1,z0); glVertex3f(x1,y1,z1);
    glVertex3f(x1,y1,z1); glVertex3f(x0,y1,z1);
    glVertex3f(x0,y1,z1); glVertex3f(x0,y1,z0);
    // verticals
    glVertex3f(x0,y0,z0); glVertex3f(x0,y1,z0);
    glVertex3f(x1,y0,z0); glVertex3f(x1,y1,z0);
    glVertex3f(x1,y0,z1); glVertex3f(x1,y1,z1);
    glVertex3f(x0,y0,z1); glVertex3f(x0,y1,z1);
    glEnd();
    glEnable(GL_DEPTH_TEST);
}

static void drawCrosshair(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, w, h, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	glBegin(GL_LINES);
	glColor3f(1,1,1);
	int cx = w/2, cy = h/2; int s = 8;
	glVertex2i(cx - s, cy); glVertex2i(cx + s, cy);
	glVertex2i(cx, cy - s); glVertex2i(cx, cy + s);
	glEnd();
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

int run_demo(voxel::World& world, mesh::GreedyMesher& mesher) {
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
    double lastX = 0.0, lastY = 0.0; bool haveLast = false;
    float yaw = 0.0f, pitch = -0.5f;
    float camX = 8.0f, camY = 10.0f, camZ = 28.0f; // eye position
    // Initialize yaw/pitch to face cube center (8,8,8)
    {
        float toX = 8.0f - camX;
        float toY = 8.0f - camY;
        float toZ = 8.0f - camZ;
        float len = std::sqrt(toX*toX + toY*toY + toZ*toZ);
        if (len > 0.0001f) {
            toX /= len; toY /= len; toZ /= len;
            yaw = std::atan2(toZ, toX);
            pitch = std::asin(toY);
        }
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Build initial mesh from chunk (0,0)
    voxel::Chunk& chunk = world.getOrCreateChunk(0,0);
    mesh::Mesh mesh = mesher.buildMesh(chunk);

    // state for toggles

    while (!glfwWindowShouldClose(window)) {
        // input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
        // Mouse look: always track mouse delta
        {
            double x, y; glfwGetCursorPos(window, &x, &y);
            if (!haveLast) { lastX = x; lastY = y; haveLast = true; }
            double dx = x - lastX, dy = y - lastY; lastX = x; lastY = y;
            yaw   += static_cast<float>(dx) * 0.01f;
            pitch -= static_cast<float>(dy) * 0.01f; // inverted mouse Y
            if (pitch < -1.5f) pitch = -1.5f;
            if (pitch > 1.5f)  pitch = 1.5f;
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
        // Note: Q/E no longer dolly; they are used for edit actions below
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

        // Keyboard: recenter (R) to world origin view
        static bool prevR = false, prevF = false, prevQ = false, prevE = false; 
        bool curR = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
        bool curF = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
        bool curQ = glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS;
        bool curE = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;
        if (curR && !prevR) {
            camX = 8.0f; camY = 10.0f; camZ = 28.0f;
            float toX = 8.0f - camX, toY = 8.0f - camY, toZ = 8.0f - camZ;
            float len = std::sqrt(toX*toX + toY*toY + toZ*toZ);
            if (len > 0.0001f) { toX/=len; toY/=len; toZ/=len; yaw = std::atan2(toZ,toX); pitch = std::asin(toY); }
        }
        static bool wireframe = false;
        if (curF && !prevF) {
            wireframe = !wireframe;
            glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        }
        prevR = curR; prevF = curF;

        // Raycast and edit
        // Q = remove, E = add (edge-triggered)
        bool pressQ = curQ && !prevQ;
        bool pressE = curE && !prevE;
        prevQ = curQ; prevE = curE;
        RayHit hit = raycastVoxel(chunk, camX, camY, camZ, fwdX, fwdY, fwdZ, 100.0f);
        if (pressQ || pressE) {
            if (pressQ && hit.hit) {
                int nonAir = 0;
                for (int z=0; z<chunk.sizeZ(); ++z) for (int y=0; y<chunk.sizeY(); ++y) for (int x=0; x<chunk.sizeX(); ++x) if (chunk.at(x,y,z).type!=voxel::BlockType::Air) ++nonAir;
                // Protect world origin block (0,0,0) from deletion
                if (nonAir > 1 && !(hit.x==0 && hit.y==0 && hit.z==0)) { chunk.at(hit.x,hit.y,hit.z).type = voxel::BlockType::Air; mesh = mesher.buildMesh(chunk); }
            } else if (pressE) {
                int px,py,pz;
                if (hit.hit) { px = hit.x + hit.nx; py = hit.y + hit.ny; pz = hit.z + hit.nz; }
                else { float t = 4.0f; px = (int)std::floor(camX + fwdX*t); py = (int)std::floor(camY + fwdY*t); pz = (int)std::floor(camZ + fwdZ*t); }
                if (px>=0&&py>=0&&pz>=0&&px<chunk.sizeX()&&py<chunk.sizeY()&&pz<chunk.sizeZ()) { chunk.at(px,py,pz).type = voxel::BlockType::Dirt; mesh = mesher.buildMesh(chunk); }
            }
        }

        // Highlight selection and placement preview
        if (hit.hit) {
            drawWireCube(hit.x, hit.y, hit.z, 1.0f, 0.2f, 0.2f);
            int px = hit.x + hit.nx, py = hit.y + hit.ny, pz = hit.z + hit.nz;
            if (px>=0&&py>=0&&pz>=0&&px<chunk.sizeX()&&py<chunk.sizeY()&&pz<chunk.sizeZ()) {
                drawWireCube(px, py, pz, 0.2f, 1.0f, 0.2f);
            }
        }

        // Draw HUD crosshair and present
        drawCrosshair(w,h);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

} // namespace render
#endif


