#ifndef _MODEL_H
#define _MODEL_H

#include<_common.h>
#include<_textureloader.h>
#include<_objloader.h>
#include<_boundingBox.h>
class _model
{
    public:
        _model();
        // Copy contructor for preloading models for multiple instances
        _model(const _model& other);
        virtual ~_model();

        _textureLoader *texLoader = new _textureLoader();
        _objLoader *model3DLoader = new _objLoader();

        vec3f rotation;
        vec3f position;
        vec3f scale;
        col3f color;

        // for physics
        vec3 velocity;
        vec3 acceleration;

        vector<_boundingBox> boundingBoxes; // Models can have multiple bounding boxes for collision detection

        // Model enabled flag -- disabled by default but init functions set to true. Must be manually set back once changed.
        bool enabled = false;

        enum modelType {TEAPOT, TORUS, CUBE, SPHERE, CUSTOM};
        modelType currentModel;

        // Initialize model with texture + model
        void initModel(char* texPath, char* modelPath, modelType currentModel);
        // Initialize model with texture only (primitive model ONLY)
        void initModel(char* texPath, modelType currentModel);
        // Draw the model
        void drawModel();
        // Display bounding boxes for debugging
        void displayBoundingBoxes();
        // Add a bounding box to the model
        void addBoundingBox(vec3f dim, vec3f pos, vec3f scale);
        // Add a bounding box to model with position/scale tied to model's
        void addBoundingBox(vec3f dim);
        // Simple AABB (T/F) collision check with another bounding box for ALL in model
        bool isCollidingWith(const _boundingBox& otherBox);
        // Penetration Depth collision check with another bounding box for ALL in model
        vector<_boundingBox::collisionType> checkCollisionWith(const _boundingBox& otherBox);

    protected:

    private:
};

#endif // _MODEL_H
