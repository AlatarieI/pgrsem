//----------------------------------------------------------------------------------------
/**
 * \file       scene_object.h
 * \author     skorelu1
 * \date       2025/04/24
 * \brief      Hierarchical scene object with transformation capabilities
 *
 *  Represents an object in 3D scene graph hierarchy with parent-child relationships,
 *  local transformations, and rendering properties. Handles dirty flag propagation
 *  for transformation updates.
 *
 */
//----------------------------------------------------------------------------------------

#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include <glm.hpp>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <gtc/matrix_transform.hpp>

/// Scene graph node with transformation hierarchy
class SceneObject {
public:
    int shaderIdx;          ///< Index of associated shader program
    std::vector<SceneObject*> children{};  ///< Child objects in hierarchy
    SceneObject* parent;    ///< Parent object in hierarchy
    int parentIdx;          ///< Index in parent's children list (-1 if root)
    bool transparent;       ///< Transparency flag for rendering order
    std::string name;       ///< Optional object identifier

    glm::vec3 position{};   ///< Local position in 3D space
    glm::vec3 rotation{};   ///< Euler rotation angles in degrees (XYZ order)
    glm::vec3 scale{};      ///< Local scale factors
    glm::mat4 transform;    ///< Combined transformation matrix

    bool isDirty;           ///< Flag indicating need for matrix recalculation
    int modelIndex;         ///< Index of associated 3D model
    int lightIdx;           ///< Index of associated light source (-1 if none)

    /// Default constructor
    SceneObject();

    /// Parameterized constructor
    /**
     * \param[in] shaderIdx   Shader program index for rendering
     * \param[in] modelIndex  3D model index in model repository
     * \param[in] position    Initial world position
     * \param[in] rotation    Initial rotation angles (degrees)
     * \param[in] scale       Initial scale factors
     */
    SceneObject(int shaderIdx, int modelIndex,
                glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

    /// Get computed model matrix
    /**
     * \return Combined transformation matrix accounting for hierarchy
     */
    glm::mat4 getModelMatrix();

    /// Update transformation hierarchy
    /**
     * Recursively updates transformation matrices for this object
     * and all its children if dirty flag is set
     */
    void updateSelfAndChildren();
};

#endif // SCENE_OBJECT_H