#include "transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <iostream>

constexpr float two_pi = glm::two_pi<float>();

// TODO: this works now, but do we want default behavior to be this, in reality this is the the way we do stuff for a
// fps camera
glm::mat4 Transform::get_rotation_transform_matrix() const {

    // convert rotations from turns to radians
    float rad_x = rotation.x * two_pi; // Pitch
    float rad_y = rotation.y * two_pi; // Yaw
    float rad_z = rotation.z * two_pi; // Roll

    // compute yaw rotation around the up (y-axis) vector
    glm::mat4 yaw_matrix = glm::rotate(glm::mat4(1.0f), rad_y, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 yaw_direction = yaw_matrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f); // Yaw direction is a new vector

    // compute right vector via cross product of up and yaw direction
    glm::vec3 right_direction = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), yaw_direction));

    // compute pitch rotation around the right vector
    glm::mat4 pitch_matrix = glm::rotate(glm::mat4(1.0f), rad_x, right_direction);

    // Compute roll rotation around the resulting axis (yaw direction)
    glm::mat4 roll_matrix = glm::rotate(glm::mat4(1.0f), rad_z, yaw_direction);

    // Combine all rotations (yaw -> pitch -> roll)
    glm::mat4 result = roll_matrix * pitch_matrix * yaw_matrix;

    return result;
}
glm::mat4 Transform::get_scale_transform_matrix() const { return glm::scale(glm::mat4(1.0f), scale); }
glm::mat4 Transform::get_translation_transform_matrix() const { return glm::translate(glm::mat4(1.0f), translation); }

Transform Transform::get_inverse_transform() const {
    glm::vec3 inverse_translation = -translation;
    glm::vec3 inverse_rotation = -rotation;
    // invert scale (avoid division by zero)
    glm::vec3 inverse_scale = glm::vec3(1.0f) / scale;
    return Transform(inverse_translation, inverse_rotation, inverse_scale, transform_application_order);
}

glm::mat4 Transform::get_transform_matrix() const {

    if (transform_matrix_override)
        return *transform_matrix_override;

    glm::mat4 transform_matrix = glm::mat4(1);

    switch (transform_application_order) {
    case ScaleTranslationRotation:
        transform_matrix =
            get_rotation_transform_matrix() * get_translation_transform_matrix() * get_scale_transform_matrix();
        break;
    case ScaleRotationTranslation: // most likely what you want.
        transform_matrix =
            get_translation_transform_matrix() * get_rotation_transform_matrix() * get_scale_transform_matrix();
        break;
    };
    return transform_matrix;
}

glm::mat4 Transform::get_full_transform_matrix() const {
    glm::mat4 transform_matrix(1.0f);

    switch (transform_application_order) {
    case ScaleTranslationRotation:
        transform_matrix =
            get_rotation_transform_matrix() * get_translation_transform_matrix() * get_scale_transform_matrix();
        break;

    case ScaleRotationTranslation: // most common
        transform_matrix =
            get_translation_transform_matrix() * get_rotation_transform_matrix() * get_scale_transform_matrix();
        break;
    }

    // If there is a child, multiply its transform as well
    if (child) {
        transform_matrix *= child->get_full_transform_matrix();
    }

    return transform_matrix;
}

void Transform::set_translation_x(const double &x) { translation.x = x; }
void Transform::set_translation_y(const double &y) { translation.y = y; }
void Transform::set_translation_z(const double &z) { translation.z = z; }

void Transform::set_translation(const double &x, const double &y, const double &z) {
    set_translation(glm::vec3(x, y, z));
}
void Transform::set_translation(const glm::vec3 &new_translation) { translation = new_translation; }

void Transform::reset_translation() { set_translation(glm::vec3(0)); }

void Transform::add_translation(const double &x, const double &y, const double &z) {
    add_translation(glm::vec3(x, y, z));
}

void Transform::add_translation(const glm::vec3 &add_translation) { translation += add_translation; }

void Transform::set_rotation(const glm::vec3 &new_rotation) { rotation = new_rotation; }

void Transform::set_rotation_pitch(const double &new_pitch) { rotation.x = new_pitch; }
void Transform::set_rotation_yaw(const double &new_yaw) { rotation.y = new_yaw; }
void Transform::reset_yaw() { set_rotation_yaw(0); }
void Transform::reset_pitch() { set_rotation_pitch(0); }
void Transform::set_rotation_roll(const double &new_roll) { rotation.z = new_roll; }

void Transform::add_rotation_pitch(const double &pitch) { rotation.x += pitch; }
void Transform::add_rotation_yaw(const double &yaw) { rotation.y += yaw; }
void Transform::add_rotation_roll(const double &roll) { rotation.z += roll; }

void Transform::reset_rotation() { set_rotation(glm::vec3(0)); }

void Transform::set_scale(const double &uniform_scale) { scale = glm::vec3(uniform_scale); }

void Transform::set_scale(const glm::vec3 &new_scale) { scale = new_scale; }

void Transform::set_scale(const double &x, const double &y, const double &z) { set_scale(glm::vec3(x, y, z)); }

void Transform::set_scale_x(const double &new_scale) { scale.x = new_scale; }

void Transform::set_scale_y(const double &new_scale) { scale.y = new_scale; }

void Transform::set_scale_z(const double &new_scale) { scale.z = new_scale; }

void Transform::reset() {
    reset_scale();
    reset_rotation();
    reset_translation();
}

void Transform::reset_scale() { set_scale(glm::vec3(1)); }

// NOTE: subtle potential bug if you set the transform matrix and then leter change the any of rot, pos scale
// then the new matrix will not be what you expect because it will recompute and not use this one
void Transform::set_transform_matrix(const glm::mat4 &matrix) {
    bool is_translation_rotation_scale_matrix =
        glm::all(glm::equal(matrix[3], glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::epsilon<float>()));

    if (!is_translation_rotation_scale_matrix) {
        std::cout << "failed to set transform matrix, because matrix is not trs matrix" << std::endl;
        return;
    }

    glm::vec3 scale, translation, skew;
    glm::quat rotation;
    glm::vec4 perspective;

    if (!glm::decompose(matrix, scale, rotation, translation, skew, perspective)) {
        return;
    }

    // ensure perspective is negligible
    if (glm::length(perspective) > glm::epsilon<float>()) {
        return;
    }

    this->translation = translation;
    this->rotation = glm::eulerAngles(rotation) / glm::two_pi<float>(); // convert radians to turns
    this->scale = scale;
}

void Transform::set_transform_matrix_override(const glm::mat4 &transform) { transform_matrix_override = transform; }

void Transform::clear_transform_matrix_override() { transform_matrix_override = std::nullopt; }

std::string Transform::to_string() const {
    std::ostringstream oss;
    oss << "Position: (" << translation.x << ", " << translation.y << ", " << translation.z << ")\n"
        << "Rotation: (" << rotation.x << ", " << rotation.y << ", " << rotation.z << ")\n"
        << "Scale: (" << scale.x << ", " << scale.y << ", " << scale.z << ")\n";
    return oss.str();
}

glm::vec3 Transform::get_full_translation() const {
    if (child)
        return translation + child->get_full_translation();
    return translation;
}

glm::vec3 Transform::get_full_scale() const {
    if (child)
        return scale * child->get_full_scale();
    return scale;
}

glm::vec3 Transform::get_full_rotation() const {
    if (child)
        return rotation + child->get_full_rotation();
    return rotation;
}

double Transform::get_full_rotation_pitch() const {
    if (child)
        return rotation.x + child->get_full_rotation_pitch();
    return rotation.x;
}

double Transform::get_full_rotation_yaw() const {
    if (child)
        return rotation.y + child->get_full_rotation_yaw();
    return rotation.y;
}

glm::vec3 Transform::compute_forward_vector() const {
    glm::vec3 forward;
    forward.x = cos(rotation.x * two_pi) * cos(rotation.y * two_pi);
    forward.y = sin(rotation.x * two_pi);
    forward.z = cos(rotation.x * two_pi) * sin(rotation.y * two_pi);
    return glm::normalize(forward);
}

glm::vec3 Transform::compute_xz_forward_vector() const {
    glm::vec3 forward;
    forward.x = cos(rotation.y * two_pi);
    forward.z = sin(rotation.y * two_pi);
    forward.y = 0.0f; // No Y component
    return glm::normalize(forward);
}

glm::vec2 Transform::compute_xz_forward_vector_R2() const {
    glm::vec2 forward;
    forward.x = cos(rotation.y * two_pi);
    forward.y = sin(rotation.y * two_pi);
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
    return linalg_utils::create_billboard_transform(transform.compute_right_vector(), transform.compute_up_vector(),
                                                    transform.compute_forward_vector());
}
