#pragma once

#define GLEW_STATIC

#include "pch.h"
#include "DxUtility.h"
#include "glew.h"
#include "wglew.h"
#include <iostream>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000


class xrMathHelper {
public:
    static float XrVector3f_Length(const XrVector3f v) {
        return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    }
    static float XrQuaternionf_Dot(const XrQuaternionf left, XrQuaternionf right) {
        float dot;
        dot = left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w;
        return dot;
    }
    static float XrVector3f_Dot(const XrVector3f a, const XrVector3f b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    static XrVector3f XrVector3f_Abs(const XrVector3f a) {
        XrVector3f result;
        result.x = std::abs(a.x);
        result.y = std::abs(a.y);
        result.z = std::abs(a.z);
        return result;
    }
    static XrVector3f XrVector3f_Multi(const XrVector3f a, const float b) {
        XrVector3f result;
        result.x = a.x * b;
        result.y = a.y * b;
        result.z = a.z * b;
        return result;
    }
    static XrVector3f XrVector3f_Add(const XrVector3f a, const float b) {
        XrVector3f result;
        result.x = a.x + b;
        result.y = a.y + b;
        result.z = a.z + b;
        return result;
    }
    static XrVector3f XrVector3f_Add(const XrVector3f a, const XrVector3f b) {
        XrVector3f result;
        result.x = a.x + b.x;
        result.y = a.y + b.y;
        result.z = a.z + b.z;
        return result;
    }
    static XrVector3f XrVector3f_Sub(const XrVector3f a, const XrVector3f b) {
        XrVector3f result;
        result.x = a.x - b.x;
        result.y = a.y - b.y;
        result.z = a.z - b.z;
        return result;
    }
    static float XrRcpSqrt(const float x) {
        const float SMALLEST_NON_DENORMAL = 1.1754943508222875e-038f;
        const float rcp = (x >= SMALLEST_NON_DENORMAL) ? 1.0f / sqrtf(x) : 1.0f;
        return rcp;
    }
    static XrVector3f XrVector3f_Normalize(XrVector3f v) {
        XrVector3f result;
        const float lengthRcp = XrRcpSqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        result.x = v.x * lengthRcp;
        result.y = v.y * lengthRcp;
        result.z = v.z * lengthRcp;
        return result;
    }
    static XrQuaternionf XrQuaternionf_Multiply(const XrQuaternionf a, const XrQuaternionf b) {
        XrQuaternionf result;
        result.x = (b.w * a.x) + (b.x * a.w) + (b.y * a.z) - (b.z * a.y);
        result.y = (b.w * a.y) - (b.x * a.z) + (b.y * a.w) + (b.z * a.x);
        result.z = (b.w * a.z) + (b.x * a.y) - (b.y * a.x) + (b.z * a.w);
        result.w = (b.w * a.w) - (b.x * a.x) - (b.y * a.y) - (b.z * a.z);
        return result;
    }

};
struct OpenGLProgram {
    unsigned int triangleProgram;
    unsigned int fullScreenRectProgram;
    unsigned int triangleVAO;
    unsigned int rectVAO;

    unsigned int fbo = 0;
    GLuint glSharedTextur[1];

    DirectX::XMMATRIX g_matModel;
    DirectX::XMMATRIX g_matView;
    DirectX::XMMATRIX g_matProjection;
    float fov = 70.0f;
    std::map<std::string, int> uniformLocations;
    //===============================
    // handlers to share the resouurces
    HANDLE glHandleDX9Device;
    HANDLE g_hDXSharedTexture;
    HANDLE glHandleSharedTexture[1];
    HDC g_hDCGL = NULL;
    //========================
    // actions varibles
    float m_MovingSpeed {0.05};
    float m_OrbitingSpeed{0.5};
    float m_OrbitAngleY;
    float m_OrbitAngleX;
    float m_OrbitAngleZ;

    XrVector3f m_position{0, 0, 0};
    XrVector3f m_scale{1.0, 1.0, 1.0};
    XrQuaternionf m_quaternion{xr::math::Quaternion::RotationAxisAngle({0, 1, 0}, 0)};
    DirectX::XMMATRIX translationMat;
    DirectX::XMMATRIX rotationMat;
    DirectX::XMMATRIX scalingMat;

    //==============================
    OpenGLProgram(){};
    ~OpenGLProgram(){};



void createTriangleProgram();
    void createFullScreenRectProgram();
    void initWindow(HWND hWndGL);
    void initOpenGLProgram(ID3D11Device* m_device,ID3D11Texture2D* m_dxColorbuffer);
    void setUniform(unsigned int handle, const char* name, DirectX::XMMATRIX m);
    void updateModelMatrix();
    void render();
    void scale(float newValue);
    void translation(XrVector3f direction);
    void rotation(XrVector3f delta);
};