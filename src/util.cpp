#include "util.h"

void print_vec3(glm::vec3 vec, std::ostream& os) {
	os << "( " << vec.x << ", " << vec.y << ", " << vec.z << ")\n";
}