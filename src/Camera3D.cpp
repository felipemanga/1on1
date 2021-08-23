#include <Femto>
#include "Camera3D.h"
#include "Terrain.h"
#include "Renderer.h"

f32 Camera3D::angleTo(const Point3D& target) {
    PROFILER;
    auto delta = target - Ground::relative(camera3D.position, target);
    return -atan2(delta.x, delta.z);
}

s32 Camera3D::distanceTo(const Point3D& target) {
    PROFILER;
    return (Ground::relative(camera3D.position, target) - target).xz().lengthSquared();
}

void Camera3D::follow(Point3D target, f32 targetRotation) {
    PROFILER;
    Point3D forwardOffset {f32(50.0f), 0, 0};
    forwardOffset.rotateXZ(targetRotation);

    auto& terrain = gameRenderer->get<Ground>();
    s32 groundHeight = (terrain.getHeightAtPoint({position.x, position.z}) + 5);
    if (target.y < groundHeight)
        target.y = groundHeight;

    auto delta = (target + forwardOffset) - position;
    f32 lookAtRotation = -atan2(delta.x, delta.z);
    rotation += angleDelta(rotation, lookAtRotation) * f32(0.3f);
    // rotation = lookAtRotation;

    // Point3D offset {0, f32(3.0f), f32(-4.0f) + sin(frame++ * f32(0.1f)) * 2};
    Point3D offset {0, f32(3.0f), f32(4.5f)};
    offset.rotateXZ(targetRotation + PI/2);

    position.tweenTo(target + offset, 2);
    // position = target + offset;

    // delta = (target + offset) - position;
    // delta *= f32(0.3f);
    // speed.tweenTo(delta, 5);
    // position += speed * f32(0.5f);
}
