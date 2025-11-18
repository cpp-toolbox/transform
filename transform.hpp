#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <optional>

#include "sbpt_generated_includes.hpp"

enum TransformApplicationOrder {
    ScaleTranslationRotation,
    ScaleRotationTranslation,
};

/**
 * @brief a transform class for translation, rotation and scale
 *
 * @todo Every transform should hold an optional child transform so that we can stack transforms easily.
 *
 * @note since we use a unique_ptr for the child transform, the default copy constructor is deleted because unique_ptrs
 * don't have a copy constructor, that is why we have defined our own here
 *
 * @note functions that have the the word full indicate that that function accounts for the entire transform hierarchy
 * of child transforms when computing the value.
 *
 */

class Transform {
  public:
    Transform(glm::vec3 translation = glm::vec3(0), glm::vec3 rotation = glm::vec3(0), glm::vec3 scale = glm::vec3(1),
              const TransformApplicationOrder &transform_application_order =
                  TransformApplicationOrder::ScaleRotationTranslation)
        : translation(translation), rotation(rotation), scale(scale),
          transform_application_order(transform_application_order) {};

    /**
     * @brief Deep copy constructor.
     *
     * Creates a new Transform as a deep copy of another Transform.
     * The new instance duplicates all transform data and recursively
     * copies any existing child hierarchy.
     *
     * @param other The Transform instance to copy.
     */
    Transform(const Transform &other)
        : translation(other.translation), rotation(other.rotation), scale(other.scale),
          transform_application_order(other.transform_application_order) {

        // NOTE: his is recursive because the = invokes the copy assignment operator, this function just kicks off the
        // recursion
        if (other.child)
            child = std::make_unique<Transform>(*other.child);
    }

    /**
     * @brief Deep copy assignment operator.
     *
     * Assigns the contents of another Transform to this one.
     * Performs a deep copy of all member variables, including the
     * recursively owned child hierarchy if present.
     *
     * @param other The Transform instance to copy from.
     * @return Reference to this Transform.
     */
    Transform &operator=(const Transform &other) {
        if (this == &other)
            return *this;

        translation = other.translation;
        rotation = other.rotation;
        scale = other.scale;
        transform_application_order = other.transform_application_order;

        // NOTE: recursive
        child = other.child ? std::make_unique<Transform>(*other.child) : nullptr;
        return *this;
    }

    /**
     * @brief Move constructor.
     *
     * Transfers ownership of resources from another Transform to this one.
     * The source Transform is left in a valid but unspecified state.
     *
     * @param other The Transform instance to move from.
     */
    Transform(Transform &&other) noexcept = default;

    /**
     * @brief Move assignment operator.
     *
     * Transfers ownership of resources from another Transform to this one.
     * The source Transform is left in a valid but unspecified state.
     *
     * @param other The Transform instance to move from.
     * @return Reference to this Transform.
     */
    Transform &operator=(Transform &&other) noexcept = default;

    TransformApplicationOrder transform_application_order;

    // startfold setters

    void set_translation_x(const double &x);
    void set_translation_y(const double &y);
    void set_translation_z(const double &z);
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

    void set_transform_matrix(const glm::mat4 &matrix);

    std::optional<glm::mat4> transform_matrix_override;
    /**
     * @brief sets the transform matrix overide which will be used until you clear the override
     *
     * the matrix override is used instead of constructing the matrix from the internal translation, scale, and rotation
     * properties until it is cleared
     */
    void set_transform_matrix_override(const glm::mat4 &transform);
    void clear_transform_matrix_override();

    void reset();
    void reset_scale();
    // endfold

    // startfold getters
    // NOTE: missing a few of the "full" getters for certain matrices here

    Transform get_inverse_transform() const;
    glm::mat4 get_transform_matrix() const;
    glm::mat4 get_full_transform_matrix() const;

    // rotation

    glm::vec3 get_rotation() const { return rotation; }
    glm::vec3 get_full_rotation() const;

    double get_rotation_pitch() const { return rotation.x; }
    double get_full_rotation_pitch() const;

    double get_rotation_yaw() const { return rotation.y; }
    double get_full_rotation_yaw() const;

    glm::mat4 get_rotation_transform_matrix() const;

    // scale

    glm::vec3 get_scale() const { return scale; }
    glm::vec3 get_full_scale() const;

    glm::mat4 get_scale_transform_matrix() const;

    // translation

    glm::vec3 get_translation() const { return translation; }
    glm::vec3 get_full_translation() const;

    glm::mat4 get_translation_transform_matrix() const;

    // endfold

    // directional vectors

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

    void set_child(Transform new_child) {
        // Create a new Transform on the heap by moving 'new_child' into it.
        child = std::make_unique<Transform>(std::move(new_child));
    }

    std::unique_ptr<Transform> child = nullptr;

  private:
    // TODO: remove this
    void update_transform_matrix();

    // NOTE: this is here because if it's not true, then we don't have to recompute it
    bool modified_since_last_call_to_get_transform_matrix;

    glm::vec3 translation; // Position in 3D space
    glm::vec3 rotation;    // Euler angles in turns (pitch, yaw, roll)
    glm::vec3 scale;       // Scale factors
};

glm::mat4 create_billboard_transform(const Transform &transform);

#endif // TRANSFORM_HPP
