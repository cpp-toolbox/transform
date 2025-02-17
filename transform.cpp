#include "transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <format>

Transform::Transform() : position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f) {}

#include <iostream> // Include iostream for std::cout
//
constexpr float two_pi = glm::two_pi<float>();

glm::mat4 Transform::get_transform_matrix() const {
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), position);

    // Convert rotations from turns to radians.
    float radX = rotation.x * two_pi;
    float radY = rotation.y * two_pi;
    float radZ = rotation.z * two_pi;

    // Create the rotation matrices for each axis.
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), radX, glm::vec3(1.0f, 0.0f, 0.0f));
    rotate = glm::rotate(rotate, radY, glm::vec3(0.0f, 1.0f, 0.0f));
    rotate = glm::rotate(rotate, radZ, glm::vec3(0.0f, 0.0f, 1.0f));

    // Create the scale matrix from the scale vector.
    glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), scale);

    // Combine translation, rotation, and scale matrices.
    return translate * rotate * scale_mat;
}

void Transform::set_transform_matrix(glm::mat4 transform) {
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::quat orientation;

    if (glm::decompose(transform, this->scale, orientation, this->position, skew, perspective)) {
        // Convert quaternion to Euler angles (in degrees)
        glm::vec3 euler_angles = glm::eulerAngles(orientation);
        rotation = glm::degrees(euler_angles); // Convert radians to degrees
    } else {
        std::cerr << "Failed to decompose transformation matrix." << std::endl;
    }
}

std::string Transform::get_string_repr() const {
    return std::format("Position: ({}, {}, {})\nRotation: ({}, {}, {})\nScale: ({}, {}, {})\n", position.x, position.y,
                       position.z, rotation.x, rotation.y, rotation.z, scale.x, scale.y, scale.z);
}

glm::vec3 Transform::compute_forward_vector() const {
    glm::vec3 forward;
    forward.x = cos(rotation.x * two_pi) * cos(rotation.y * two_pi);
    forward.y = sin(rotation.x * two_pi);
    forward.z = cos(rotation.x * two_pi) * sin(rotation.y * two_pi);
    return glm::normalize(forward);
}

glm::vec3 Transform::compute_right_vector() const {
    glm::vec3 forward = compute_forward_vector();
    glm::vec3 world_up(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(forward, world_up));
    return right;
}

glm::vec3 Transform::compute_up_vector() const {
    glm::vec3 forward = compute_forward_vector();
    glm::vec3 right = compute_right_vector();
    glm::vec3 up = glm::normalize(glm::cross(right, forward));
    return up;
}
