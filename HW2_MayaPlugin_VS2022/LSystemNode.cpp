#include "LSystemNode.h"

#include "LSystem.h"
#include "cylinder.h"

#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnStringData.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnMesh.h>
#include <maya/MGlobal.h>
#include <maya/MTime.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MStatus.h>

MTypeId LSystemNode::id(0x001226F0);
MObject LSystemNode::angle;
MObject LSystemNode::step;
MObject LSystemNode::grammarfile;
MObject LSystemNode::time;
MObject LSystemNode::mesh;

void* LSystemNode::creator()
{
    return new LSystemNode();
}
MStatus LSystemNode::initialize()
{
    MStatus status;
    // attribute points to
    MFnNumericAttribute nAttr;
    MFnTypedAttribute tAttr;
    MFnUnitAttribute uAttr;
    MFnStringData sData;
    MString grammar = "";
    MObject grammarObj = sData.create(grammar, &status);

    angle = nAttr.create("angle", "a", MFnNumericData::kDouble, 90.0, &status);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    addAttribute(angle);

    step = nAttr.create("step", "s", MFnNumericData::kDouble, 1.0, &status);
    nAttr.setKeyable(true);
    nAttr.setMin(0.0001);
    nAttr.setStorable(true);
    addAttribute(step);

    grammarfile = tAttr.create("grammarFile", "g", MFnData::kString, grammarObj, &status);
    tAttr.setStorable(true);
    tAttr.setWritable(true);
    addAttribute(grammarfile);


    time = uAttr.create("time", "t", MFnUnitAttribute::kTime, 0.0, &status);
    uAttr.setKeyable(true);
    uAttr.setStorable(true);
    addAttribute(time);

    mesh = tAttr.create("outputMesh", "o", MFnData::kMesh, MObject::kNullObj, &status);

    tAttr.setStorable(false);
    tAttr.setWritable(false);
    tAttr.setReadable(true);

    addAttribute(mesh);
    attributeAffects(angle, mesh);
    attributeAffects(step, mesh);
    attributeAffects(grammarfile, mesh);
    attributeAffects(time, mesh);

    return MS::kSuccess;
}


MStatus LSystemNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;
    double angleInput = data.inputValue(angle).asDouble();
    double stepInput = data.inputValue(step).asDouble();
    MString grammarPath = data.inputValue(grammarfile).asString();
    MTime tVal = data.inputValue(time).asTime();
    int iter = (int)tVal.value();

    LSystem lsys;
    std::vector<LSystem::Branch> branches;

   if (grammarPath.length() > 0)
        lsys.loadProgram(grammarPath.asChar());
    else
        lsys.loadProgramFromString("F\nF->F[+F]F[-F]F\n");

    lsys.setDefaultAngle((float)angleInput);
    lsys.setDefaultStep((float)stepInput);
    lsys.process(iter, branches);

    MPointArray points;
    MIntArray faceCounts;
    MIntArray faceConnects;

    double radius = 0.05 * stepInput;

    for (size_t i = 0; i < branches.size(); ++i)
    {
        const vec3& a = branches[i].first;
        const vec3& b = branches[i].second;

        MPoint start(a[0], a[1], a[2]);
        MPoint end(b[0], b[1], b[2]);

        CylinderMesh cyl(start, end, radius);
        cyl.appendToMesh(points, faceCounts, faceConnects);
    }

    MFnMeshData meshDataFn;
    MObject outData = meshDataFn.create();
    MFnMesh meshFn;

    if (branches.empty())
    {
        MPoint start(0, 0, 0);
        MPoint end(0, 2, 0);

        CylinderMesh cyl(start, end, 0.1);
        cyl.appendToMesh(points, faceCounts, faceConnects);
    }

    else
    {
        meshFn.create(points.length(),
            faceCounts.length(),
            points,
            faceCounts,
            faceConnects,
            outData);
    }

    MDataHandle out = data.outputValue(mesh);
    out.set(outData);
    out.setClean();
    data.setClean(plug);

    return MS::kSuccess;
}

