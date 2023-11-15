#ifndef PRINT_DEBUG_HELPERS_H
#define PRINT_DEBUG_HELPERS_H

#include <stdio.h>
#include <glm/glm.hpp>

void PrintMat4(const glm::mat4& mat) {
	printf("| %3.3f %3.3f %3.3f %3.3f |\n| %3.3f %3.3f %3.3f %3.3f |\n| %3.3f %3.3f %3.3f %3.3f |\n| %3.3f %3.3f %3.3f %3.3f |\n",
		mat[0][0], mat[0][1], mat[0][2], mat[0][3],
		mat[1][0], mat[1][1], mat[1][2], mat[1][3],
		mat[2][0], mat[2][1], mat[2][2], mat[2][3],
		mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
}

void PrintVec3(const glm::vec3& vec) {
	printf("[ %3.3f %3.3f %3.3f ]\n", vec.x, vec.y, vec.z);
}

#endif // !PRINT_DEBUG_HELPERS_H
