#pragma once
#include "Vector3D.h"
#include "Matrix4x4.h"

class Camera
{
public:
    Camera();

    void update();
    void updateViewMatrix();
    void updateProjectionMatrix(float fov, float aspectRatio, float nearZ, float farZ);

    void moveForward(float amount);
    void strafeRight(float amount);
    void rotate(float pitchDelta, float yawDelta);

    void setPosition(const Vector3D& pos);
    void setRotation(float pitch, float yaw);

    Vector3D getPosition() const;
    Vector3D getRotation() const;

    const Matrix4x4& getViewMatrix() const;
    const Matrix4x4& getProjectionMatrix() const;

    float getFOV() const;
    void zoomIn(float delta);
    void zoomOut(float delta);

private:
    void updateCameraVectors();

    //Vector3D m_position;

    Matrix4x4 m_viewMatrix;
    Matrix4x4 m_projMatrix;

    Vector3D m_position = Vector3D(0, 0, -2);
    Vector3D m_forwardDir = Vector3D(0, 0, 1);
    Vector3D m_rightDir = Vector3D(1, 0, 0);
    Vector3D m_upDir = Vector3D(0, 1, 0);

    float m_pitch = 0.0f;
    float m_yaw = 0.0f;
    float m_speed = 0.05f;

    float m_fov = 1.57f; // default FOV (90 degrees)
};
