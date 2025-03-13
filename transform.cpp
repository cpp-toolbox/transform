#include "transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <format>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Transform::Transform() : position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f) {}

#include <iostream> // Include iostream for std::cout
//
constexpr float two_pi = glm::two_pi<float>();

glm::mat4 Transform::get_rotation_transform_matrix() const {

    // convert rotations from turns to radians.
    float rad_x = rotation.x * two_pi;
    float rad_y = rotation.y * two_pi;
    float rad_z = rotation.z * two_pi;

    // create the rotation matrices for each axis.
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), rad_x, glm::vec3(1.0f, 0.0f, 0.0f));
    rotate = glm::rotate(rotate, rad_y, glm::vec3(0.0f, 1.0f, 0.0f));
    rotate = glm::rotate(rotate, rad_z, glm::vec3(0.0f, 0.0f, 1.0f));
    return rotate;
}
glm::mat4 Transform::get_scale_transform_matrix() const { return glm::scale(glm::mat4(1.0f), scale); }
glm::mat4 Transform::get_translation_transform_matrix() const { return glm::translate(glm::mat4(1.0f), position); }

glm::mat4 Transform::get_transform_matrix() const {
    return get_translation_transform_matrix() * get_rotation_transform_matrix() * get_scale_transform_matrix();
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

glm::mat4 create_billboard_transform(const Transform &transform) {
    return create_billboard_transform(transform.compute_right_vector(), transform.compute_up_vector(),
                                      transform.compute_forward_vector());
}

// a billboard matrix is one such that it takes the basis and transforms it to be the basis that is passed in
// this allows you to take quads and make them face the camera very easily which is the main use case
// NOTE: look doesn't always have to be the direction that the camera is facing, sometimes you might want to instead
// just get the vector that goes from the cameras position to the object as the look vector.
glm::mat4 create_billboard_transform(const glm::vec3 &right, const glm::vec3 &up, const glm::vec3 &look) {
    // Create the rotation matrix to orient the object
    glm::mat4 billboard_mat(1.0f);

    // the matrix has its columns sideways
    billboard_mat[0] = glm::vec4(right, 0.0f); // right vector
    billboard_mat[1] = glm::vec4(up, 0.0f);    // up vector
    billboard_mat[2] = glm::vec4(-look, 0.0f); // look vector (inverted for proper facing)

    return billboard_mat;
}

glm::mat4 create_billboard_transform(const glm::vec3 &look) {
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(look, up));
    glm::vec3 new_up = glm::normalize(glm::cross(right, look));
    glm::mat4 billboard_mat(1.0f);
    // set the right, up, and look vectors as columns for the rotation matrix
    billboard_mat[0] = glm::vec4(right, 0.0f);  // right vector
    billboard_mat[1] = glm::vec4(new_up, 0.0f); // new up vector
    billboard_mat[2] = glm::vec4(-look, 0.0f);  // look vector (inverted for proper facing)

    return billboard_mat;
}

glm::mat4 create_billboard_transform_with_lock_axis(const glm::vec3 &lock_axis, const glm::vec3 &look) {
    // Calculate the right vector using the cross product of the look vector and the lock axis
    glm::vec3 right = glm::normalize(glm::cross(look, lock_axis));

    // Use the lock axis directly as the up vector (it remains fixed)
    glm::vec3 up = lock_axis;

    // Create the rotation matrix
    glm::mat4 billboard_mat(1.0f);

    // Set the right, up, and look vectors as columns for the rotation matrix
    billboard_mat[0] = glm::vec4(right, 0.0f); // Right vector
    billboard_mat[1] = glm::vec4(up, 0.0f);    // Up vector (lock axis)
    billboard_mat[2] = glm::vec4(-look, 0.0f); // Look vector (inverted for proper facing)

    return billboard_mat;
}
