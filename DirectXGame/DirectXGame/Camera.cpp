#include "Camera.h"
#include <cmath>

Camera::Camera() : m_position(0, 0, -2), m_pitch(0), m_yaw(0)
{
    updateViewMatrix();
    updateProjectionMatrix(1.57f, 1.0f, 0.1f, 100.0f);
}

void Camera::update()
{
    updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    // Convert pitch and yaw from degrees to radians
    float pitchRad = m_pitch;
    float yawRad = m_yaw;

    // Forward direction
    m_forwardDir.m_x = cos(pitchRad) * sin(yawRad);
    m_forwardDir.m_y = sin(pitchRad);
    m_forwardDir.m_z = cos(pitchRad) * cos(yawRad);
    m_forwardDir.normalize();

    // Right direction
    m_rightDir = m_forwardDir.cross(Vector3D(0, 1, 0));
    m_rightDir.normalize();

    // Up direction
    m_upDir = m_rightDir.cross(m_forwardDir);
    m_upDir.normalize();

    // Update the view matrix using lookAt
    Vector3D target = m_position + m_forwardDir;
    m_viewMatrix.setLookAtLH(m_position, target, m_upDir);
}

void Camera::updateViewMatrix()
{
    Matrix4x4 rotX, rotY;
    rotX.setIdentity();
    rotY.setIdentity();

    rotX.setRotationX(m_pitch);
    rotY.setRotationY(m_yaw);

    Matrix4x4 rotMatrix = rotX.multiplyTo(rotY);
    Vector3D forward = rotMatrix.getZDirection();
    Vector3D target = m_position + forward;

    Matrix4x4 viewMatrix;
    viewMatrix.setLookAtLH(m_position, target, Vector3D(0, 1, 0));
    m_viewMatrix = viewMatrix;
}

void Camera::updateProjectionMatrix(float fov, float aspectRatio, float nearZ, float farZ)
{
    m_fov = fov;
    m_projMatrix.setPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);
}

void Camera::moveForward(float amount)
{
    m_position = m_position + (m_forwardDir * amount * m_speed);
}

void Camera::strafeRight(float amount)
{
    m_position = m_position + (m_rightDir * amount * m_speed);
}

void Camera::rotate(float pitchDelta, float yawDelta)
{
    m_pitch += pitchDelta;
    m_yaw += yawDelta;
    updateCameraVectors();
}

void Camera::setPosition(const Vector3D& pos)
{
    m_position = pos;
    updateViewMatrix();
}

void Camera::setRotation(float pitch, float yaw)
{
    m_pitch = pitch;
    m_yaw = yaw;
    updateViewMatrix();
}

Vector3D Camera::getPosition() const
{
    return m_position;
}

Vector3D Camera::getRotation() const
{
    return Vector3D(m_pitch, m_yaw, 0);
}

const Matrix4x4& Camera::getViewMatrix() const
{
    return m_viewMatrix;
}

const Matrix4x4& Camera::getProjectionMatrix() const
{
    return m_projMatrix;
}

float Camera::getFOV() const
{
    return m_fov;
}

void Camera::zoomIn(float delta)
{
    m_fov -= delta;
    if (m_fov < 0.1f) m_fov = 0.1f; // prevent too narrow
    updateProjectionMatrix(m_fov, 1.0f, 0.1f, 100.0f); // aspectRatio gets updated later
}

void Camera::zoomOut(float delta)
{
    m_fov += delta;
    if (m_fov > 3.0f) m_fov = 3.0f; // prevent too wide
    updateProjectionMatrix(m_fov, 1.0f, 0.1f, 100.0f);
}
