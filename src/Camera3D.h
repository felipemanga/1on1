#pragma once
#include <Femto>

class Camera3D {
public:
    Point3D position = {15, 50, -10};
    Point3D speed;
    f32 rotation = f32(1.0f);
    u16 zBuffer[screenWidth];
    void follow(Point3D target, f32 targetRotation);
    s32 distanceTo(const Point3D& target);
    f32 angleTo(const Point3D& target);
} inline camera3D;
