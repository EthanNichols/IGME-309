#pragma once

#include <vector>
#include "glm/glm.hpp"

#include "Simplex\Simplex.h"

namespace Simplex {

class Collider {
    std::vector<glm::vec2> _points;
    std::vector<glm::vec2> _normals;
    float _depth;
    glm::mat4 _transform;

public:
    Collider(std::vector<glm::vec2> points, float depth, bool recenter = false) :
        _points(points), _depth(depth) {
        _normals.reserve(points.size( ));
        for (std::size_t i = 0; i < points.size( ); ++i) {
            std::size_t n = (i + 1) % points.size( );
            glm::vec2 diff = glm::normalize(points[n] - points[i]);
            _normals.emplace_back(diff.y, -diff.x);
        }
        if (recenter) {
            glm::vec2 average;
            for (auto &v : points) {
                average += v;
            }
            average /= points.size( );
            for (auto &v : points) {
                v += average;
            }
        }
    }

    // matrix MUST NOT include any rotation besides roll!
    void SetModelMatrix(glm::mat4 matrix) {
        _transform = matrix;
    }

    glm::mat4 GetModelMatrix(void) {
        return _transform;
    }

    void AddToRenderList(void) {
        MeshManager *meshMgr = MeshManager::GetInstance( );
        std::vector<glm::vec3> tpoints;
        tpoints.reserve(_points.size( ));
        for (auto &p : _points) {
            tpoints.emplace_back(glm::vec4(p.x, p.y, 0, 1) * _transform);
        }
        for (unsigned i = 0; i < tpoints.size( ); ++i) {
            unsigned n = (i + 1) % tpoints.size( );
            glm::vec3 offset(0, 0, _depth);
            meshMgr->AddLineToRenderList(Simplex::IDENTITY_M4, tpoints[i], tpoints[n], Simplex::C_RED, Simplex::C_RED);
            meshMgr->AddLineToRenderList(Simplex::IDENTITY_M4, tpoints[i], tpoints[i] + offset, Simplex::C_RED, Simplex::C_RED);
            meshMgr->AddLineToRenderList(Simplex::IDENTITY_M4, tpoints[i] + offset, tpoints[n] + offset, Simplex::C_RED, Simplex::C_RED);
        }
    }

    bool IsColliding(Collider other) {
        std::vector<glm::vec2> tpoints;
        tpoints.reserve(_points.size( ));
        for (auto &p : _points) {
            tpoints.emplace_back(glm::vec4(p.x, p.y, 0, 1) * _transform);
        }
        std::vector<glm::vec2> opoints;
        opoints.reserve(_points.size( ));
        for (auto &p : other._points) {
            opoints.emplace_back(glm::vec4(p.x, p.y, 0, 1) * _transform);
        }

        float tmin = std::numeric_limits<float>::max( ), tmax = std::numeric_limits<float>::min( );
        float omin = std::numeric_limits<float>::max( ), omax = std::numeric_limits<float>::min( );
        // project onto each of our normals...
        // (there's a better way to do this, but I can't be arsed)
        for (auto &axis : _normals) {
            for (auto &point : tpoints) {
                float proj = glm::dot(axis, point);
                if (proj < tmin) tmin = proj;
                if (proj > tmax) tmax = proj;
            }
            for (auto &point : opoints) {
                float proj = glm::dot(axis, point);
                if (proj < omin) omin = proj;
                if (proj > omax) omax = proj;
            }
            if ((tmin > omin || omin > tmax) &&
                (omin > tmin || tmin > omax)) {
                return false;
            }
        }
        // ...then all of theirs
        for (auto &axis : other._normals) {
            for (auto &point : tpoints) {
                float proj = glm::dot(axis, point);
                if (proj < tmin) tmin = proj;
                if (proj > tmax) tmax = proj;
            }
            for (auto &point : opoints) {
                float proj = glm::dot(axis, point);
                if (proj < omin) omin = proj;
                if (proj > omax) omax = proj;
            }
            if ((tmin > omin || omin > tmax) &&
                (omin > tmin || tmin > omax)) {
                return false;
            }
        }
    }

    ~Collider( ) = default;
};

}
