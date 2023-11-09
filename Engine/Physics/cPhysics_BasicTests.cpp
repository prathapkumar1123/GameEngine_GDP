#include "PhysicsEngine.h"

// Returns true if sphere s intersects triangle ABC, false otherwise.
bool PhysicsEngine::mTestSphereTriangle(float sphereRadius, glm::vec3 vert0, glm::vec3 vert1, glm::vec3 vert2, glm::vec3 sphereCentre) {
	
	// Find point P on triangle ABC closest to sphere center
	glm::vec3 closestPoint = this->mClosestPtPointTriangle(sphereCentre, vert0, vert1, vert2);

	// Sphere and triangle intersect if the (squared) distance from sphere
	// center to point p is less than the (squared) sphere radius
	glm::vec3 v = closestPoint - sphereCentre;

	bool isItIntersecting = false;

	if (glm::dot(v, v) <= (sphereRadius * sphereRadius)) {
		isItIntersecting = true;
	}

	return isItIntersecting;
}

glm::vec3 PhysicsEngine::closestTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& p)
{
	glm::vec3 v10 = v1 - v0;
	glm::vec3 p0 = p - v0;
	glm::vec3 v21 = v2 - v1;
	glm::vec3 p1 = p - v1;
	glm::vec3 v02 = v0 - v2;
	glm::vec3 p2 = p - v2;
	glm::vec3 nor = glm::cross(v10, v02);

	// method 1, in 3D space
	if (glm::dot(glm::cross(v10, nor), p0) < 0.0)
		return v0 + v10 * glm::clamp(glm::dot(p0, v10) / glm::dot(v10, v10), 0.0f, 1.0f);
	if (glm::dot(glm::cross(v21, nor), p1) < 0.0)
		return v1 + v21 * glm::clamp(glm::dot(p1, v21) / glm::dot(v21, v21), 0.0f, 1.0f);
	if (glm::dot(glm::cross(v02, nor), p2) < 0.0)
		return v2 + v02 * glm::clamp(glm::dot(p2, v02) / glm::dot(v02, v02), 0.0f, 1.0f);
	return p - nor * glm::dot(nor, p0) / glm::dot(nor, nor);

	// method 2, in barycentric space
	glm::vec3 q = glm::cross(nor, p0);
	float d = 1.0f / glm::dot(nor, nor);
	float u = d * glm::dot(q, v02);
	float v = d * glm::dot(q, v10);
	float w = 1.0f - u - v;

	if (u < 0.0f) {
		w = glm::clamp(glm::dot(p2, v02) / glm::dot(v02, v02), 0.0f, 1.0f);
		u = 0.0f;
		v = 1.0f - w;
	}
	else if (v < 0.0f) {
		u = glm::clamp(glm::dot(p0, v10) / glm::dot(v10, v10), 0.0f, 1.0f);
		v = 0.0f;
		w = 1.0f - u;
	}
	else if (w < 0.0f) {
		v = glm::clamp(glm::dot(p1, v21) / glm::dot(v21, v21), 0.0f, 1.0f);
		w = 0.0f;
		u = 1.0f - v;
	}

	return u * v1 + v * v2 + w * v0;
}


glm::vec3 PhysicsEngine::mClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 bc = c - b;

	// Compute parametric position s for projection P' of P on AB,
	// P' = A + s*AB, s = snom/(snom+sdenom)
	float snom = glm::dot(p - a, ab), sdenom = glm::dot(p - b, a - b);

	// Compute parametric position t for projection P' of P on AC,
	// P' = A + t*AC, s = tnom/(tnom+tdenom)
	float tnom = glm::dot(p - a, ac), tdenom = glm::dot(p - c, a - c);

	if (snom <= 0.0f && tnom <= 0.0f) return a; // Vertex region early out

	// Compute parametric position u for projection P' of P on BC,
	// P' = B + u*BC, u = unom/(unom+udenom)
	float unom = glm::dot(p - b, bc), udenom = glm::dot(p - c, b - c);

	if (sdenom <= 0.0f && unom <= 0.0f) return b; // Vertex region early out
	if (tdenom <= 0.0f && udenom <= 0.0f) return c; // Vertex region early out


	// P is outside (or on) AB if the triple scalar product [N PA PB] <= 0
	glm::vec3 n = glm::cross(b - a, c - a);
	float vc = glm::dot(n, glm::cross(a - p, b - p));
	// If P outside AB and within feature region of AB,
	// return projection of P onto AB
	if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
		return a + snom / (snom + sdenom) * ab;

	// P is outside (or on) BC if the triple scalar product [N PB PC] <= 0
	float va = glm::dot(n, glm::cross(b - p, c - p));
	// If P outside BC and within feature region of BC,
	// return projection of P onto BC
	if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
		return b + unom / (unom + udenom) * bc;

	// P is outside (or on) CA if the triple scalar product [N PC PA] <= 0
	float vb = glm::dot(n, glm::cross(c - p, a - p));
	// If P outside CA and within feature region of CA,
	// return projection of P onto CA
	if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
		return a + tnom / (tnom + tdenom) * ac;

	// P must project inside face region. Compute Q using barycentric coordinates
	float u = va / (va + vb + vc);
	float v = vb / (va + vb + vc);
	float w = 1.0f - u - v; // = vc / (va + vb + vc)
	return u * a + v * b + w * c;
}

// The OG code from the book:
//Point ClosestPtPointTriangle(Point p, Point a, Point b, Point c)
//{
//	Vector ab = b - a;
//	Vector ac = c - a;
//	Vector bc = c - b;
//
//	// Compute parametric position s for projection P' of P on AB,
//	// P' = A + s*AB, s = snom/(snom+sdenom)
//	float snom = Dot(p - a, ab), sdenom = Dot(p - b, a - b);
//
//	// Compute parametric position t for projection P' of P on AC,
//	// P' = A + t*AC, s = tnom/(tnom+tdenom)
//	float tnom = Dot(p - a, ac), tdenom = Dot(p - c, a - c);
//
//	if (snom <= 0.0f && tnom <= 0.0f) return a; // Vertex region early out
//
//	// Compute parametric position u for projection P' of P on BC,
//	// P' = B + u*BC, u = unom/(unom+udenom)
//	float unom = Dot(p - b, bc), udenom = Dot(p - c, b - c);
//
//	if (sdenom <= 0.0f && unom <= 0.0f) return b; // Vertex region early out
//	if (tdenom <= 0.0f && udenom <= 0.0f) return c; // Vertex region early out
//
//
//	// P is outside (or on) AB if the triple scalar product [N PA PB] <= 0
//	Vector n = Cross(b - a, c - a);
//	float vc = Dot(n, Cross(a - p, b - p));
//	// If P outside AB and within feature region of AB,
//	// return projection of P onto AB
//	if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
//		return a + snom / (snom + sdenom) * ab;
//
//	// P is outside (or on) BC if the triple scalar product [N PB PC] <= 0
//	float va = Dot(n, Cross(b - p, c - p));
//	// If P outside BC and within feature region of BC,
//	// return projection of P onto BC
//	if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
//		return b + unom / (unom + udenom) * bc;
//
//	// P is outside (or on) CA if the triple scalar product [N PC PA] <= 0
//	float vb = Dot(n, Cross(c - p, a - p));
//	// If P outside CA and within feature region of CA,
//	// return projection of P onto CA
//	if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
//		return a + tnom / (tnom + tdenom) * ac;
//
//	// P must project inside face region. Compute Q using barycentric coordinates
//	float u = va / (va + vb + vc);
//	float v = vb / (va + vb + vc);
//	float w = 1.0f - u - v; // = vc / (va + vb + vc)
//	return u * a + v * b + w * c;
//}

