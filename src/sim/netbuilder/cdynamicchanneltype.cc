//==========================================================================
//   CDYNAMICCHANNELTYPE.CC
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <cstring>
#include <cstdio>
#include <ctime>
#include <iostream>

#include "cdynamicchanneltype.h"
#include "cnedloader.h"
#include "cnednetworkbuilder.h"
#include "common/fileutil.h"
#include "common/stringutil.h"
#include "sim/pythonutil.h"

namespace omnetpp {

using namespace common;

cDynamicChannelType::cDynamicChannelType(cNedLoader *nedLoader, const char *qname) :
    cChannelType(qname), nedLoader(nedLoader)
{
}

cNedDeclaration *cDynamicChannelType::getDecl() const
{
    // do not store the pointer, because the declaration object may have been
    // thrown out of cNedLoader to conserve memory
    cNedDeclaration *decl = nedLoader->getDecl(getFullName());
    ASSERT(decl->getType() == cNedDeclaration::CHANNEL);
    return decl;
}

std::string cDynamicChannelType::str() const
{
    return getDecl()->str();
}

std::string cDynamicChannelType::getNedSource() const
{
    return getDecl()->getNedSource();
}

cChannel *cDynamicChannelType::createChannelObject()
{
    cNedDeclaration *decl = getDecl();

#ifdef WITH_PYTHONSIM
    std::string pythonClassName = getQualifiedPythonClassName(decl);
    if (!pythonClassName.empty())
        return instantiatePythonObjectChecked<cChannel>(pythonClassName.c_str());
#else
    if (decl->getProperties()->get("pythonClass") != nullptr)
        throw cRuntimeError("Cannot instantiate channel `%s': Python support is not enabled", getFullName());
#endif

    const char *classname = decl->getImplementationClassName();
    ASSERT(classname != nullptr);
    return instantiateChannelClass(classname);
}

void cDynamicChannelType::addParametersTo(cChannel *channel)
{
    cNedDeclaration *decl = getDecl();
    cConfiguration *cfg = channel->getEnvir()->getConfig();
    cNedNetworkBuilder(nedLoader, cfg).addParametersAndGatesTo(channel, decl);  // adds only parameters, because channels have no gates
}

void cDynamicChannelType::applyPatternAssignments(cComponent *component)
{
    cConfiguration *cfg = component->getEnvir()->getConfig();
    cNedNetworkBuilder(nedLoader, cfg).assignParametersFromPatterns(component);
}

cProperties *cDynamicChannelType::getProperties() const
{
    cNedDeclaration *decl = getDecl();
    return decl->getProperties();
}

cProperties *cDynamicChannelType::getParamProperties(const char *paramName) const
{
    cNedDeclaration *decl = getDecl();
    return decl->getParamProperties(paramName);
}

cProperties *cDynamicChannelType::getGateProperties(const char *gateName) const
{
    throw cRuntimeError("cDynamicChannelType::getGateProperties(): Channels have no gates");
}

cProperties *cDynamicChannelType::getSubmoduleProperties(const char *submoduleName, const char *submoduleType) const
{
    throw cRuntimeError("cDynamicChannelType::getSubmoduleProperties(): Channels do not contain submodules");
}

cProperties *cDynamicChannelType::getConnectionProperties(int connectionId, const char *channelType) const
{
    throw cRuntimeError("cDynamicChannelType::getConnectionProperties(): Channels do not contain further connections");
}

std::string cDynamicChannelType::getPackageProperty(const char *name) const
{
    cNedDeclaration *decl = getDecl();
    return decl->getPackageProperty(name);
}

const char *cDynamicChannelType::getImplementationClassName() const
{
    cNedDeclaration *decl = getDecl();
    return decl->getImplementationClassName();
}

std::string cDynamicChannelType::getCxxNamespaceForType(const char *type) const
{
    cNedDeclaration *decl = nedLoader->getDecl(type);
    return decl->getPackageProperty("namespace");
}

std::string cDynamicChannelType::getCxxNamespace() const
{
    cNedDeclaration *decl = getDecl();
    return decl->getCxxNamespace();
}

const char *cDynamicChannelType::getSourceFileName() const
{
    cNedDeclaration *decl = getDecl();
    return decl->getSourceFileName();
}

bool cDynamicChannelType::isInnerType() const
{
    cNedDeclaration *decl = getDecl();
    return decl->isInnerType();
}

void cDynamicChannelType::clearSharedParImpls()
{
    cChannelType::clearSharedParImpls();
    getDecl()->clearSharedParImpls();
}

}  // namespace omnetpp

