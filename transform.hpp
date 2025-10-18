#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "sbpt_generated_includes.hpp"

enum TransformApplicationOrder {
    ScaleTranslationRotation,
    ScaleRotationTranslation,
};

class Transform {
  public:
    Transform(glm::vec3 translation = glm::vec3(0), glm::vec3 rotation = glm::vec3(0), glm::vec3 scale = glm::vec3(1),
              const TransformApplicationOrder &transform_application_order =
                  TransformApplicationOrder::ScaleRotationTranslation)
        : translation(translation), rotation(rotation), scale(scale),
          transform_application_order(transform_application_order) {};

    TransformApplicationOrder transform_application_order;

    void set_translation_y(const double &y);
    void set_translation(const double &x, const double &y, const double &z);
    void set_translation(const glm::vec3 &new_position);
    void reset_translation();

    void add_translation(const double &x, const double &y, const double &z);
    void add_translation(const glm::vec3 &add_position);

    void set_rotation(const glm::vec3 &pitch_yaw_roll);

    void set_rotation_pitch(const double &new_pitch);
    void set_rotation_yaw(const double &new_yaw);
    void reset_yaw();
    void reset_pitch();
    void set_rotation_roll(const double &new_roll);

    void add_rotation_pitch(const double &pitch);
    void add_rotation_yaw(const double &yaw);
    void add_rotation_roll(const double &roll);
    void reset_rotation();

    void set_scale(const double &scale);
    void set_scale(const double &x, const double &y, const double &z);
    void set_scale(const glm::vec3 &new_scale);

    void set_scale_x(const double &new_scale);
    void set_scale_y(const double &new_scale);
    void set_scale_z(const double &new_scale);

    void reset();
    void reset_scale();

    Transform get_inverse_transform() const;
    glm::mat4 get_transform_matrix();
    glm::vec3 get_rotation() const { return rotation; }

    double get_rotation_pitch() const { return rotation.x; }
    double get_rotation_yaw() const { return rotation.y; }
    glm::mat4 get_rotation_transform_matrix() const;
    glm::vec3 get_scale() const { return scale; }
    glm::mat4 get_scale_transform_matrix() const;
    glm::vec3 get_translation() const { return translation; }
    glm::mat4 get_translation_transform_matrix() const;

    void set_transform_matrix(const glm::mat4 &matrix);

    glm::vec3 compute_forward_vector() const;
    glm::vec3 compute_xz_forward_vector() const;
    glm::vec2 compute_xz_forward_vector_R2() const;
    glm::vec3 compute_right_vector() const;
    glm::vec3 compute_up_vector() const;
    std::string to_string() const;

    friend std::ostream &operator<<(std::ostream &os, const Transform &transform) {
        os << transform.to_string();
        return os;
    }

  private:
    glm::mat4 transform_matrix = glm::mat4(1);
    bool transform_needs_update =
        true; // because if you initialize and call get transform matrix the if statement shoud go through
    void update_transform_matrix();

    glm::vec3 translation; // Position in 3D space
    glm::vec3 rotation;    // Euler angles in turns (pitch, yaw, roll)
    glm::vec3 scale;       // Scale factors
};

glm::mat4 create_billboard_transform(const Transform &transform);
glm::mat4 create_translation_and_look_transform(const glm::vec3 &position, const glm::vec3 &look_vector,
                                                const glm::vec3 &up_hint = glm::vec3(0.0f, 1.0f, 0.0f));

glm::mat4 change_of_basis_move_y_to_look_dir(const glm::vec3 &position, const glm::vec3 &look_vector,
                                             const glm::vec3 &up_hint = glm::vec3(0.0f, 1.0f, 0.0f));

glm::mat4 create_billboard_transform(const glm::vec3 &right, const glm::vec3 &up, const glm::vec3 &look);
glm::mat4 create_billboard_transform(const glm::vec3 &look);
glm::mat4 create_billboard_transform_with_lock_axis(const glm::vec3 &lock_axis, const glm::vec3 &look);

bool angle_between_vectors_is_within(glm::vec3 v, glm::vec3 w, double turns);
bool vector_is_within_centered_sector(glm::vec3 center, glm::vec3 other, double sector_angle_turns);

#endif // TRANSFORM_HPP
