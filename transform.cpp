#include "transform.hpp"
#include <glm/gtx/matrix_decompose.hpp>

Transform::Transform() : position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f) {}

glm::mat4 Transform::get_transform_matrix() const {
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotate = glm::rotate(rotate, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotate = glm::rotate(rotate, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), scale);

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

void Transform::print() const {
    std::cout << "Position: (" << position.x << ", " << position.y << ", " << position.z << ")\n"
              << "Rotation: (" << rotation.x << ", " << rotation.y << ", " << rotation.z << ")\n"
              << "Scale: (" << scale.x << ", " << scale.y << ", " << scale.z << ")\n";
}

glm::vec3 Transform::compute_forward_vector() const {
    glm::vec3 forward;
    forward.x = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
    forward.y = sin(glm::radians(rotation.x));
    forward.z = cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
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
