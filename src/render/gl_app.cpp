#include "gl_app.hpp"

#ifdef VOXEL_WITH_GL
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include "../core/logging.hpp"
#include "../core/math.hpp"
#include "../config/config.hpp"
#include "../input/input_manager.hpp"
#include "../config/config_manager.hpp"
#include "raycast.hpp"
#include <cmath>
#include <cstring>
#include <iostream>
#include "../voxel/world.hpp"
#include "../mesh/greedy_mesher.hpp"
#include <filesystem>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace render {


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

static std::string nowTimestamp() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto t = system_clock::to_time_t(now);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

static void writeRunError(const std::string& message) {
    try {
        std::filesystem::create_directories("logs");
        std::ofstream f("logs/run_errors.log", std::ios::app);
        if (f) {
            static bool first = true;
            if (first) {
                f << "\n============ " << nowTimestamp() << " - run errors session ============\n";
                first = false;
            }
            f << '[' << nowTimestamp() << "] " << message << '\n';
            f.flush();
        }
    } catch (...) {}
    std::fprintf(stderr, "%s\n", message.c_str());
}

int run_demo(voxel::World& world, mesh::GreedyMesher& mesher) {
	if (!glfwInit()) {
		core::log(core::LogLevel::Error, "Failed to init GLFW");
		const char* disp = std::getenv("DISPLAY");
		std::string d = disp ? disp : "(null)";
		writeRunError(std::string("GLFW init failed. DISPLAY=") + d + ", ensure X server is running.");
		return -1;
	}
	
	GLFWwindow* window = glfwCreateWindow(800, 600, "Voxel Demo", nullptr, nullptr);
	if (!window) {
		const char* disp = std::getenv("DISPLAY");
		std::string d = disp ? disp : "(null)";
		writeRunError(std::string("Failed to create GLFW window. DISPLAY=") + d + ", windowing not available.");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
    
    // Initialize ConfigManager
    config::ConfigManager& configManager = config::ConfigManager::instance();
    if (!configManager.initialize()) {
        std::cerr << "Failed to initialize ConfigManager" << std::endl;
        return -1;
    }
    
    // Ensure input config exists
    if (!configManager.ensureConfigExists("input.ini")) {
        std::cerr << "Failed to ensure input.ini exists" << std::endl;
        return -1;
    }
    
    // Initialize InputManager
    input::InputManager& inputManager = input::InputManager::instance();
    inputManager.initialize();
    inputManager.loadConfig(configManager.getConfigPath("input.ini"));
    
    // Set up GLFW callbacks
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        input::InputManager::instance().setKeyState(key, action == GLFW_PRESS || action == GLFW_REPEAT);
    });
    
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        input::InputManager::instance().setKeyState(button, action == GLFW_PRESS);
    });
    
    // Load VSync setting from config
    bool vsyncEnabled = config::Config::instance().graphics().vsync;
    glfwSwapInterval(vsyncEnabled ? 1 : 0);

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

    bool showDebug = false;
    // FPS tracking
    auto startTime = std::chrono::steady_clock::now();
    auto lastFpsTime = startTime;
    auto lastTitleTime = startTime;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;
    double fps = 0.0;

    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;
        
        // Update InputManager
        inputManager.update();
        
        // Mouse look: always track mouse delta
        {
            double x, y; glfwGetCursorPos(window, &x, &y);
            if (!haveLast) { lastX = x; lastY = y; haveLast = true; }
            double dx = x - lastX, dy = y - lastY; lastX = x; lastY = y;
            inputManager.setMouseDelta(static_cast<float>(dx), static_cast<float>(dy));
            
            float mouseDeltaX, mouseDeltaY;
            inputManager.getMouseDelta(mouseDeltaX, mouseDeltaY);
            yaw   += mouseDeltaX;
            pitch -= mouseDeltaY; // inverted mouse Y
            if (pitch < -1.5f) pitch = -1.5f;
            if (pitch > 1.5f)  pitch = 1.5f;
        }
        float moveSpeed = inputManager.isActionPressed(input::Action::FastMovement) ? 6.0f : 2.0f;
        moveSpeed *= deltaTime; // Apply delta time
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
        if (inputManager.isActionPressed(input::Action::MoveForward)) { camX += fwdX * moveSpeed; camY += fwdY * moveSpeed; camZ += fwdZ * moveSpeed; }
        if (inputManager.isActionPressed(input::Action::MoveBackward)) { camX -= fwdX * moveSpeed; camY -= fwdY * moveSpeed; camZ -= fwdZ * moveSpeed; }
        if (inputManager.isActionPressed(input::Action::MoveLeft)) { camX -= rightX * moveSpeed;                         camZ -= rightZ * moveSpeed; }
        if (inputManager.isActionPressed(input::Action::MoveRight)) { camX += rightX * moveSpeed;                         camZ += rightZ * moveSpeed; }
        if (inputManager.isActionPressed(input::Action::MoveUp)) { camY += moveSpeed; }
        if (inputManager.isActionPressed(input::Action::MoveDown)) { camY -= moveSpeed; }
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
        static bool prevR = false, prevF = false, prevQ = false, prevE = false, prevF3 = false, prevF4 = false, prevF5 = false, prevML=false, prevMR=false;
        bool curR = inputManager.isActionPressed(input::Action::RecenterCamera);
        bool curF = inputManager.isActionPressed(input::Action::ToggleWireframe);
        bool curQ = glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS; // Keep Q/E for now
        bool curE = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;
        bool curF3 = inputManager.isActionPressed(input::Action::ToggleDebug);
        bool curF4 = inputManager.isActionPressed(input::Action::ToggleMouseLock);
        bool curF5 = inputManager.isActionPressed(input::Action::ToggleVSync);
        bool curML = inputManager.isActionPressed(input::Action::BreakBlock);
        bool curMR = inputManager.isActionPressed(input::Action::PlaceBlock);
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
        if (curF3 && !prevF3) {
            showDebug = !showDebug;
        }
        static bool mouseLocked = false;
        if (curF4 && !prevF4) {
            mouseLocked = !mouseLocked;
            glfwSetInputMode(window, GLFW_CURSOR, mouseLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        }
        static bool currentVsync = config::Config::instance().graphics().vsync;
        if (curF5 && !prevF5) {
            currentVsync = !currentVsync;
            glfwSwapInterval(currentVsync ? 1 : 0);
            core::log(core::LogLevel::Info, currentVsync ? "VSync enabled" : "VSync disabled");
        }
        prevR = curR; prevF = curF; prevF3 = curF3; prevF4 = curF4; prevF5 = curF5;

        // Raycast and edit (mouse buttons)
        bool pressL = curML && !prevML;
        bool pressR = curMR && !prevMR;
        prevML = curML; prevMR = curMR; prevQ = curQ; prevE = curE;
        RayHit hit = raycastVoxel(chunk, camX, camY, camZ, fwdX, fwdY, fwdZ, 100.0f);
        if (pressL || pressR) {
            if (pressL && hit.hit) {
                int nonAir = 0;
                for (int z=0; z<chunk.sizeZ(); ++z) for (int y=0; y<chunk.sizeY(); ++y) for (int x=0; x<chunk.sizeX(); ++x) if (chunk.at(x,y,z).type!=voxel::BlockType::Air) ++nonAir;
                // Protect world origin block (0,0,0) from deletion
                if (nonAir > 1 && !(hit.x==0 && hit.y==0 && hit.z==0)) {
                    chunk.at(hit.x,hit.y,hit.z).type = voxel::BlockType::Air;
                    mesh = mesher.buildMesh(chunk);
                    int cx = 0, cz = 0;
                    core::log(core::LogLevel::Info, "Break block at (" + std::to_string(hit.x) + "," + std::to_string(hit.y) + "," + std::to_string(hit.z) + ") in chunk (" + std::to_string(cx) + "," + std::to_string(cz) + ")");
                }
            } else if (pressR) {
                int px,py,pz;
                if (hit.hit) { px = hit.x + hit.nx; py = hit.y + hit.ny; pz = hit.z + hit.nz; }
                else { float t = 4.0f; px = (int)std::floor(camX + fwdX*t); py = (int)std::floor(camY + fwdY*t); pz = (int)std::floor(camZ + fwdZ*t); }
                if (px>=0&&py>=0&&pz>=0&&px<chunk.sizeX()&&py<chunk.sizeY()&&pz<chunk.sizeZ()) {
                    chunk.at(px,py,pz).type = voxel::BlockType::Dirt;
                    mesh = mesher.buildMesh(chunk);
                    int cx = 0, cz = 0;
                    core::log(core::LogLevel::Info, "Place block at (" + std::to_string(px) + "," + std::to_string(py) + "," + std::to_string(pz) + ") in chunk (" + std::to_string(cx) + "," + std::to_string(cz) + ")");
                } else {
                    core::log(core::LogLevel::Warn, "Placement out of chunk bounds");
                }
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

        // FPS update accounting
        frameCount++;
        auto now = std::chrono::steady_clock::now();
        double dtFps = std::chrono::duration<double>(now - lastFpsTime).count();
        if (dtFps >= 1.0) {
            fps = frameCount / dtFps;
            frameCount = 0;
            lastFpsTime = now;
        }
        // Update title at most 4x/sec to reduce border flicker in some X servers
        if (std::chrono::duration<double>(now - lastTitleTime).count() >= 0.25) {
            char title[256];
            if (showDebug) {
                std::snprintf(title, sizeof(title), "Voxel Demo | FPS: %.1f | cam(%.2f,%.2f,%.2f) look(%.2f,%.2f,%.2f)",
                              fps, camX, camY, camZ, fwdX, fwdY, fwdZ);
                if (hit.hit) {
                    char buf2[96];
                    std::snprintf(buf2, sizeof(buf2), " | hit(%d,%d,%d)", hit.x, hit.y, hit.z);
                    size_t len = std::strlen(title);
                    size_t avail = sizeof(title) - 1 - len;
                    if (avail > 0) {
                        std::strncpy(title + len, buf2, avail);
                        title[len + avail <= sizeof(title)-1 ? len + avail : sizeof(title)-1] = '\0';
                    }
                }
            } else {
                std::snprintf(title, sizeof(title), "Voxel Demo | FPS: %.1f", fps);
            }
            glfwSetWindowTitle(window, title);
            lastTitleTime = now;
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


