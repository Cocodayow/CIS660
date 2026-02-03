#pragma once

#include <maya/MPxNode.h>
#include <maya/MTypeId.h>

class LSystemNode : public MPxNode
{
public:
    LSystemNode() = default;
    ~LSystemNode() override = default;

    static void* creator();
    static MStatus initialize();
    MStatus compute(const MPlug& plug, MDataBlock& data) override;

public:
    static MTypeId id;
    static MObject angle;
    static MObject step;
    static MObject grammarfile;
    static MObject time;
    static MObject mesh;
};
