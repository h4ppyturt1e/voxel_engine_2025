#include "mesh.hpp"
#include "greedy_mesher.hpp"
#include "../voxel/chunk.hpp"
#include "../voxel/voxel.hpp"

namespace mesh {

static inline bool isSolid(const voxel::Voxel& v) {
	return v.type != voxel::BlockType::Air;
}

// Emit a single quad into the mesh (two triangles)
static void emitQuad(Mesh& out,
	float x0, float y0, float z0,
	float x1, float y1, float z1,
	float x2, float y2, float z2,
	float x3, float y3, float z3,
	float nx, float ny, float nz)
{
	std::uint32_t base = static_cast<std::uint32_t>(out.vertices.size());
	out.vertices.push_back(Vertex{ x0, y0, z0, nx, ny, nz, 0.0f, 0.0f });
	out.vertices.push_back(Vertex{ x1, y1, z1, nx, ny, nz, 1.0f, 0.0f });
	out.vertices.push_back(Vertex{ x2, y2, z2, nx, ny, nz, 1.0f, 1.0f });
	out.vertices.push_back(Vertex{ x3, y3, z3, nx, ny, nz, 0.0f, 1.0f });
	// Winding: counter-clockwise as seen from normal direction
	out.indices.push_back(base + 0);
	out.indices.push_back(base + 1);
	out.indices.push_back(base + 2);
	out.indices.push_back(base + 0);
	out.indices.push_back(base + 2);
	out.indices.push_back(base + 3);
}

Mesh GreedyMesher::buildMesh(const voxel::Chunk& chunk) {
	Mesh out;
	const int dims[3] = { chunk.sizeX(), chunk.sizeY(), chunk.sizeZ() };

	// Sweep across three principal axes: 0=x,1=y,2=z
	for (int d = 0; d < 3; ++d) {
		const int u = (d + 1) % 3;
		const int v = (d + 2) % 3;

		int x[3] = {0,0,0};
		int q[3] = {0,0,0};
		q[d] = 1; // slice offset in the sweep direction

		// 2D mask for current slice
		std::vector<voxel::Voxel> mask(static_cast<size_t>(dims[u]) * dims[v]);

		for (x[d] = -1; x[d] < dims[d];) {
			// Build the mask comparing slice x vs x+1 along axis d
			int n = 0;
			for (x[v] = 0; x[v] < dims[v]; ++x[v]) {
				for (x[u] = 0; x[u] < dims[u]; ++x[u]) {
					voxel::Voxel a{}; // default air
					voxel::Voxel b{};
					bool aValid = (x[d] >= 0);
					bool bValid = (x[d] + 1 < dims[d]);
					if (aValid) {
						int ax = x[0], ay = x[1], az = x[2];
						a = chunk.at(ax, ay, az);
					}
					if (bValid) {
						int bx = x[0] + q[0], by = x[1] + q[1], bz = x[2] + q[2];
						b = chunk.at(bx, by, bz);
					}
					// Store face where a solid differs from b solid
					// We keep in mask the visible face voxel (solid side)
					if (isSolid(a) != isSolid(b)) {
						mask[n++] = isSolid(a) ? a : b;
					} else {
						mask[n++].type = voxel::BlockType::Air;
					}
				}
			}

			++x[d];

			// Greedy merge on mask
			int i = 0;
			for (int j = 0; j < dims[v]; ++j) {
				for (int i0 = 0; i0 < dims[u];) {
					// Find width
					voxel::Voxel cur = mask[i + i0];
					if (!isSolid(cur)) { ++i0; continue; }
					int w;
					for (w = 1; i0 + w < dims[u] && mask[i + i0 + w].type == cur.type; ++w) {}
					// Find height
					int h;
					for (h = 1; j + h < dims[v]; ++h) {
						bool same = true;
						for (int k = 0; k < w; ++k) {
							if (mask[i + i0 + k + h * dims[u]].type != cur.type) { same = false; break; }
						}
						if (!same) break;
					}
					// Emit quad for this rectangle
					// Compute quad corners in world space based on axis d and orientation
					int du[3] = {0,0,0}; du[u] = w;
					int dv[3] = {0,0,0}; dv[v] = h;

					// Position of the rectangle origin in the slice
					int x0[3] = { x[0], x[1], x[2] };
					x0[u] = i0; x0[v] = j;

					// Determine which side the face is on (a solid vs b solid)
					bool backFace = false;
					{
						// If the current mask voxel came from b (x+q), it means the visible face normal points -q
						// We infer by checking the adjacent original voxels again
						int ax = x0[0], ay = x0[1], az = x0[2];
						int bx = ax + q[0], by = ay + q[1], bz = az + q[2];
						voxel::Voxel va = (x[d]-1 >= 0) ? chunk.at(ax, ay, az) : voxel::Voxel{};
						voxel::Voxel vb = (x[d]-1 + 1 < dims[d]) ? chunk.at(bx, by, bz) : voxel::Voxel{};
						backFace = (!isSolid(va) && isSolid(vb));
					}

					// Build corners as floats
					float p0[3] = { (float)x0[0], (float)x0[1], (float)x0[2] };
					float p1[3] = { (float)(x0[0] + du[0]), (float)(x0[1] + du[1]), (float)(x0[2] + du[2]) };
					float p2[3] = { (float)(x0[0] + du[0] + dv[0]), (float)(x0[1] + du[1] + dv[1]), (float)(x0[2] + du[2] + dv[2]) };
					float p3[3] = { (float)(x0[0] + dv[0]), (float)(x0[1] + dv[1]), (float)(x0[2] + dv[2]) };

					// Advance on the slice plane along d to actual face position
					p0[d] = (float)x[d]; p1[d] = (float)x[d]; p2[d] = (float)x[d]; p3[d] = (float)x[d];
					if (backFace) {
						p0[d] -= 1.0f; p1[d] -= 1.0f; p2[d] -= 1.0f; p3[d] -= 1.0f;
					}

					float nx = 0.f, ny = 0.f, nz = 0.f;
					(backFace ? nx : nx) = 0.f; // noop to silence unused warning
					if (d == 0) nx = backFace ? -1.f : 1.f;
					if (d == 1) ny = backFace ? -1.f : 1.f;
					if (d == 2) nz = backFace ? -1.f : 1.f;

					if (!backFace) {
						emitQuad(out,
							p0[0], p0[1], p0[2],
							p1[0], p1[1], p1[2],
							p2[0], p2[1], p2[2],
							p3[0], p3[1], p3[2],
							nx, ny, nz);
					} else {
						// flip winding for back face
						emitQuad(out,
							p3[0], p3[1], p3[2],
							p2[0], p2[1], p2[2],
							p1[0], p1[1], p1[2],
							p0[0], p0[1], p0[2],
							nx, ny, nz);
					}

					// Zero-out mask we consumed
					for (int y = 0; y < h; ++y) {
						for (int xw = 0; xw < w; ++xw) {
							mask[i + i0 + xw + y * dims[u]].type = voxel::BlockType::Air;
						}
					}

					i0 += w;
				}
				i += dims[u];
			}
		}
	}

	return out;
}

} // namespace mesh


