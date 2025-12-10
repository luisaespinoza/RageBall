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

        bool ownsResources = true; // required for proper deletion of inherited resouces + copied classes. This is set to true for a sinular instance, but NOT for copies! Only the template owns their heap-allocated memory

        _textureLoader *texLoader = nullptr;
        _objLoader *model3DLoader = nullptr;

        vec3f rotation = {0.0f,0.0f,0.0f};
        vec3f position = {0.0f,1.0f,0.0f};
        vec3f scale = {1.0f,1.0f,1.0f};
        col3f color = {1.0f,1.0f,1.0f};
        // physics
        vec3f velocity = {0.0f,0.0f,0.0f};
        vec3f acceleration = {0.0f,0.0f,0.0f};

        vector<_boundingBox> boundingBoxes; // Models can have multiple bounding boxes for collision detection

        // Model enabled flag -- disabled by default but init functions set to true. Must be manually set back once changed.
        bool enabled = false;

        enum modelType {TEAPOT, TORUS, CUBE, SPHERE, CUSTOM};
        modelType currentModel;
        int modelId;

        // Initialize model with texture + model
        void initModel(char* texPath, char* modelPath, modelType currentModel);
        // Initialize model with texture only (primitive model ONLY)
        void initModel(char* texPath, modelType currentModel);
        // Draw the model
        virtual void drawModel();
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
        static int nextModelId;
};

#endif // _MODEL_H
