//
//  stlMesh.cpp
//  Testcpp
//
//  Created by Antongiuseppe Stissi on 2/12/17.
//  Copyright © 2017 Antongiuseppe Stissi. All rights reserved.
//

#include "StlMesh.hpp"
#include <fstream>
#include <iostream>
#include <math.h>
#include <algorithm>

//
// StlMesh Class
//

StlMesh::StlMesh(std::string filename)
{
    bool isBinary = true;
    
    std::ifstream fileStream (filename.c_str(), std::ios::in);
    
    if (!fileStream) { std::cout << "File not found."; return; }
    
    std::string firstWord;
    fileStream >> firstWord;
    
    if (firstWord == "solid") { isBinary = false; }
    
    fileStream.close();
    
    if (isBinary)
    {
        info.fileType = "Binary";
        ParseBinary(filename);
    } else
    {
        info.fileType = "ASCII";
        ParseASCII(filename);
    }
}

void StlMesh::ParseBinary(std::string filename)
{
    std::ifstream fileStream (filename.c_str(), std::ios::in | std::ios::binary);
    
    if (!fileStream)
    {
        std::cout << "File not found.";
        return;
    }
    
    char bufferHeaderInfo[80] = "";
    char bufferNTriangles[4];
    
    fileStream.read(bufferHeaderInfo,80);
    fileStream.read(bufferNTriangles,4);
    
    info.header = (std::string) bufferHeaderInfo;
    info.nTriangles = *((unsigned int*) bufferNTriangles);
    
    Vector3 minCoord;
    Vector3 maxCoord;
    
    info.nEdges = (3*info.nTriangles)/2;
    info.nVertex = 2 + info.nEdges - info.nTriangles;
    
    info.surfaceArea = 0.0f;
    info.volume = 0.0f;
    
    for (unsigned int i = 0; i < info.nTriangles; i++)
    {
        Triangle tri = ParseTriangle(fileStream);
        triangles.push_back(tri);
        info.surfaceArea += tri.Area();
        info.volume += tri.SignedVolume();
        
        if (i == 0) { minCoord = tri.v1; maxCoord = tri.v1;}
        
        minCoord = MinCoord(MinCoord(minCoord, tri.v1), MinCoord(tri.v2, tri.v3));
        maxCoord = MaxCoord(MaxCoord(maxCoord, tri.v1), MaxCoord(tri.v2, tri.v3));
    }
    info.volume = fabsf(info.volume);
    info.size = Vector3(fabsf(maxCoord.x - minCoord.x), fabsf(maxCoord.y - minCoord.y), fabsf(maxCoord.z - minCoord.z));

}

void StlMesh::ParseASCII(std::string filename)
{
    std::ifstream fileStream (filename.c_str(), std::ios::in);
    
    getline(fileStream, info.header);
    
    int i = 0;
    
    Vector3 minCoord, maxCoord;
    
    info.surfaceArea = 0.0f;
    info.volume = 0.0f;
    
    bool finished = false;
    while(!finished)
    {
        std::string firstWord;
        fileStream >> firstWord;
        
        if (firstWord == "endsolid")
        {
            finished = true;
            continue;
        }
        
        Triangle tri = ParseTriangleASCII(fileStream);
        
        if (i == 0) { minCoord = tri.v1; maxCoord = tri.v1;}
        
        minCoord = MinCoord(MinCoord(minCoord, tri.v1), MinCoord(tri.v2, tri.v3));
        maxCoord = MaxCoord(MaxCoord(maxCoord, tri.v1), MaxCoord(tri.v2, tri.v3));
        
        triangles.push_back(tri);
        info.surfaceArea += tri.Area();
        info.volume += tri.SignedVolume();
        i++;
        
    }
    info.volume = fabsf(info.volume);
    info.nTriangles = i;
    info.size = Vector3(fabsf(maxCoord.x - minCoord.x), fabsf(maxCoord.y - minCoord.y), fabsf(maxCoord.z - minCoord.z));
    info.nEdges = (3*info.nTriangles)/2;
    info.nVertex = 2 + info.nEdges - info.nTriangles;

}


std::string StlMesh::Info()
{
    std::string infoString = std::string("Header: ") + std::to_string(info.header) + "\n";
    infoString += std::string("File Type: ") + std::to_string(info.fileType) + "\n";
    infoString += std::string("Triangles: ") + std::to_string(info.nTriangles) + "\n";
    infoString += std::string("Edges: ") + std::to_string(info.nEdges) + "\n";
    infoString += std::string("Vertices: ") + std::to_string(info.nVertex) + "\n";
    infoString += std::string("Surface Area: ") + std::to_string(info.surfaceArea) + "\n";
    infoString += std::string("Volume: ") + std::to_string(info.volume) + "\n";
    infoString += std::string("Size (x, y, z): ") + std::to_string(info.size.x) + ", " + std::to_string(info.size.y) + ", " + std::to_string(info.size.z) + "\n";

    return infoString;
}

std::string StlMesh::InfoJS()
{
    std::string infoString = std::string("{header: \"") + std::to_string(info.header) + "\", ";
    infoString += std::string("fileType: \"") + std::to_string(info.fileType) + "\", ";
    infoString += std::string("triangles: ") + std::to_string(info.nTriangles) + ", ";
    infoString += std::string("edges: ") + std::to_string(info.nEdges) + ", ";
    infoString += std::string("vertices: ") + std::to_string(info.nVertex) + ", ";
    infoString += std::string("surfaceArea: ") + std::to_string(info.surfaceArea) + ", ";
    infoString += std::string("volume: ") + std::to_string(info.volume) + ", ";
    infoString += std::string("size: {x: ") + std::to_string(info.size.x) + ", y: " + std::to_string(info.size.y) + ", z: " + std::to_string(info.size.z) + "}}";

    return infoString;
}

Triangle StlMesh::ParseTriangle(std::ifstream& stream)
{
    Vector3 normal = ParseVector3(stream);
    Vector3 v1 = ParseVector3(stream);
    Vector3 v2 = ParseVector3(stream);
    Vector3 v3 = ParseVector3(stream);
    
    char dummyBuf[2];
    stream.read(dummyBuf, 2);
    
    return Triangle(normal, v1, v2, v3);
}

Triangle StlMesh::ParseTriangleASCII(std::ifstream& stream)
{
    return Triangle(ParseVector3ASCII(stream, 1), ParseVector3ASCII(stream, 0), ParseVector3ASCII(stream, 0),ParseVector3ASCII(stream, 2));
}


Vector3 StlMesh::ParseVector3(std::ifstream& stream)
{
    float x = ParseFloat(stream);
    float y = ParseFloat(stream);
    float z = ParseFloat(stream);
    return Vector3(x, y , z);
}

Vector3 StlMesh::ParseVector3ASCII(std::ifstream& stream, int dummies)
{
    float x, y, z;
    std::string tmp;
    getline(stream, tmp);
    sscanf(tmp.c_str(), "%*s %f %f %f", &x, &y, &z);

    while (dummies > 0)
    {
        getline(stream, tmp);
        dummies--;
    }
    
    return Vector3(x, y, z);
}


float StlMesh::ParseFloat(std::ifstream& stream) {
    char bufferFloat[4];
    stream.read(bufferFloat, 4);
    return *((float*) bufferFloat);
}

Vector3 StlMesh::MinCoord(Vector3 v1, Vector3 v2)
{
    return Vector3(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z));
}

Vector3 StlMesh::MaxCoord(Vector3 v1, Vector3 v2)
{
    return Vector3(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));
}


//
// Triangle Struct
//

float Triangle::Area()
{
    float a = v1.DistanceTo(v2);
    float b = v2.DistanceTo(v3);
    float c = v3.DistanceTo(v1);
    float p = 0.5f * (a + b + c);
    
    return sqrtf(p * (p-a) * (p - b) * (p - c));
}

float Triangle::SignedVolume()
{
    float v321 = v3.x * v2.y * v1.z;
    float v231 = v2.x * v3.y * v1.z;
    float v312 = v3.x * v1.y * v2.z;
    float v132 = v1.x * v3.y * v2.z;
    float v213 = v2.x * v1.y * v3.z;
    float v123 = v1.x * v2.y * v3.z;

    return (1.0f/6.0f) * (-v321 + v231 + v312 - v132 - v213 + v123);
}

//
// Vector3 Struct
//

float Vector3::DistanceTo(Vector3 v)
{
    return sqrtf(powf(x - v.x, 2) + powf(y - v.y, 2) + pow(z - v.z, 2));
}

