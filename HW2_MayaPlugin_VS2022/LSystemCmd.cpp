#include "LSystemCmd.h"
#include "LSystem.h"
#include <maya/MGlobal.h>
#include <list>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include <maya/MString.h>
#include <maya/MStatus.h>
#include <sstream>
#include <iomanip>
LSystemCmd::LSystemCmd() : MPxCommand()
{
}

LSystemCmd::~LSystemCmd() 
{
}

MStatus LSystemCmd::doIt( const MArgList& args )
{
	// message in Maya output window
    cout<<"Implement Me!"<<endl;
	std::cout.flush();

	// message in scriptor editor
	MGlobal::displayInfo("Implement Me!");

	MStatus status;
	MSyntax syntax;
    syntax.addFlag("-s", "-step", MSyntax::kDouble);
    syntax.addFlag("-a", "-angle", MSyntax::kDouble);
    syntax.addFlag("-g", "-gram", MSyntax::kString);
    syntax.addFlag("-i", "-iter", MSyntax::kUnsigned);

    MArgDatabase argData(syntax, args, &status);
    if (status != MS::kSuccess) {
        MGlobal::displayError("failed to parse args.");
        return status;
    }
    double step = 1.0;
    double angle = 90.;
    MString grammar = "F\nF->F[+F]F[-F]F\n";
    unsigned int iter = 0;
   

    if (argData.isFlagSet("-step")) {
        argData.getFlagArgument("-step", 0, step);
    }
    if (argData.isFlagSet("-angle")) {
        argData.getFlagArgument("-angle", 0, angle);
    }
    if (argData.isFlagSet("-gram")) {
        argData.getFlagArgument("-gram", 0, grammar);
    }
    if (argData.isFlagSet("-iter")) {
        argData.getFlagArgument("-iter", 0, iter);
    }

    LSystem lsys;
    std::vector<LSystem::Branch> branches;

    lsys.loadProgramFromString(grammar.asChar());
    lsys.setDefaultAngle((float)angle);
    lsys.setDefaultStep((float)step);
    lsys.process(iter, branches);
    
    if (branches.empty()) {
        MGlobal::displayWarning("No branches");
        return MS::kSuccess;
    }

    int created = 0;
    for (size_t i = 0; i < branches.size(); ++i) {

        const vec3& a = branches[i].first;
        const vec3& b = branches[i].second;
     
        MString name = "segment";
        name += (int)i;
        MString cmd;
        cmd += "curve -d 1 ";
        cmd += "-p "; cmd += a[0]; cmd += " "; cmd += a[1]; cmd += " "; cmd += a[2]; cmd += " ";
        cmd += "-p "; cmd += b[0]; cmd += " "; cmd += b[1]; cmd += " "; cmd += b[2]; cmd += " ";
        cmd += "-k 0 -k 1 ";
        cmd += "-name \""; cmd += name; cmd += "\";";

        MStatus s = MGlobal::executeCommand(cmd, false, true);

    }
    MString msg;
    msg += "[C++] iter="; msg += (int)iter;
    msg += " angle="; msg += angle;
    msg += " step="; msg += step;
    msg += " grammarLen="; msg += (int)grammar.length();

    MGlobal::displayInfo(msg);
    return MStatus::kSuccess;
}