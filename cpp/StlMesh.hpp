//
//  stlMesh.hpp
//  Testcpp
//
//  Created by Antongiuseppe Stissi on 2/12/17.
//  Copyright © 2017 Antongiuseppe Stissi. All rights reserved.
//

#ifndef stlMesh_hpp
#define stlMesh_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>


struct Vector3
{
    float x;
    float y;
    float z;
    
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    float DistanceTo(Vector3 v);
};

struct Triangle
{
    Vector3 normal;
    Vector3 v1;
    Vector3 v2;
    Vector3 v3;
    
    Triangle(Vector3 normal, Vector3 v1, Vector3 v2, Vector3 v3) : normal(normal), v1(v1), v2(v2), v3(v3) {}
    float Area();
    float SignedVolume();
};

struct MeshInfo
{
    std::string fileType;
    std::string header;
    float volume;
    float surfaceArea;
    Vector3 size;
    unsigned int nTriangles;
    unsigned int nEdges;
    unsigned int nVertex;
};

class StlMesh
{
public:
    StlMesh(std:: string filename);
    std::string Info();
    std::string InfoJS();
    
private:
    std::string header;
    std::vector<Triangle> triangles;
    MeshInfo info;

    void ParseBinary(std::string filename);
    void ParseASCII(std::string filename);
    Triangle ParseTriangle(std::ifstream& stream);
    Triangle ParseTriangleASCII(std::ifstream& stream);
    Vector3 ParseVector3(std::ifstream& stream);
    Vector3 ParseVector3ASCII(std::ifstream& stream, int dummies);
    float ParseFloat(std::ifstream& stream);
    Vector3 MinCoord(Vector3 v1, Vector3 v2);
    Vector3 MaxCoord(Vector3 v1, Vector3 v2);
};



namespace std {
    template<typename T>
    std::string to_string(const T &n) {
        std::ostringstream s;
        s << n;
        return s.str();
    }
}

#endif /* stlMesh_hpp */
