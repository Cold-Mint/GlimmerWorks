//
// Created by Cold-Mint on 2025/10/10.
//
#include "ResourceRef.h"

#include <cassert>

#include "../log/LogCat.h"


const std::string &glimmer::ResourceRefArg::GetName() {
    return name_;
}

void glimmer::ResourceRefArg::SetName(const std::string &name) {
    name_ = name;
}

void glimmer::ResourceRefArg::SetDataFromInt(const int data) {
    data_ = std::to_string(data);
    argType_ = RESOURCE_REF_ARG_TYPE_INT;
}

void glimmer::ResourceRefArg::SetDataFromFloat(const float data) {
    data_ = std::to_string(data);
    argType_ = RESOURCE_REF_ARG_TYPE_FLOAT;
}

void glimmer::ResourceRefArg::SetDataFromBool(const bool data) {
    data_ = std::to_string(data);
    argType_ = RESOURCE_REF_ARG_TYPE_BOOL;
}

void glimmer::ResourceRefArg::SetDataFromString(const std::string &data) {
    data_ = data;
    argType_ = RESOURCE_REF_ARG_TYPE_STRING;
}

void glimmer::ResourceRefArg::SetDataFromResourceRef(ResourceRef &data) {
    ResourceRefMessage resourceRefMessage;
    data.ToMessage(resourceRefMessage);
    data_ = resourceRefMessage.SerializeAsString();
    argType_ = RESOURCE_REF_ARG_TYPE_REF;
}


int glimmer::ResourceRefArg::AsInt() const {
    if (argType_ != RESOURCE_REF_ARG_TYPE_INT) {
        return 0;
    }
    try {
        return std::stoi(data_);
    } catch (const std::exception &) {
        return 0;
    }
}

bool glimmer::ResourceRefArg::AsBool() const {
    if (argType_ != RESOURCE_REF_ARG_TYPE_BOOL) {
        return false;
    }
    return data_ == "1" || data_ == "true";
}

std::string glimmer::ResourceRefArg::AsString() const {
    if (argType_ != RESOURCE_REF_ARG_TYPE_STRING) {
        return "";
    }
    return data_;
}

float glimmer::ResourceRefArg::AsFloat() const {
    if (argType_ != RESOURCE_REF_ARG_TYPE_FLOAT) {
        return 0;
    }
    try {
        return std::stof(data_);
    } catch (const std::exception &) {
        return 0;
    }
}

std::optional<glimmer::ResourceRef> glimmer::ResourceRefArg::AsResourceRef() const {
    if (argType_ != RESOURCE_REF_ARG_TYPE_REF) {
        return std::nullopt;
    }
    ResourceRefMessage resourceRefMessage;
    resourceRefMessage.ParseFromString(data_);
    ResourceRef resourceRef;
    resourceRef.FromMessage(resourceRefMessage);
    return resourceRef;
}

uint32_t glimmer::ResourceRefArg::GetArgType() const {
    return argType_;
}

void glimmer::ResourceRefArg::FromMessage(const ResourceRefArgMessage &resourceRefArgMessage) {
    name_ = resourceRefArgMessage.name();
    argType_ = resourceRefArgMessage.argtype();
    data_ = resourceRefArgMessage.data();
}

void glimmer::ResourceRefArg::ToMessage(ResourceRefArgMessage &resourceRefArgMessage) {
    resourceRefArgMessage.set_name(name_);
    resourceRefArgMessage.set_argtype(argType_);
    resourceRefArgMessage.set_data(data_);
}

void glimmer::ResourceRef::SetSelfPackageId(const std::string &selfPackageId) {
    if (packId_ == RESOURCE_REF_SELF) {
        packId_ = selfPackageId;
    }
    selfPackageId_ = selfPackageId;
    bindPackage_ = true;
}

const std::string &glimmer::ResourceRef::GetSelfPackageId() const {
    return selfPackageId_;
}


void glimmer::ResourceRef::AddArg(const ResourceRefArg &arg) {
    args_.push_back(arg);
}

size_t glimmer::ResourceRef::GetArgCount() const {
    return args_.size();
}


std::optional<glimmer::ResourceRefArg> glimmer::ResourceRef::GetArg(const int index) const {
    if (index >= args_.size()) {
        return std::nullopt;
    }
    return args_[index];
}


bool glimmer::ResourceRef::RemoveArg(const int index) {
    if (index >= args_.size()) {
        return false;
    }
    args_.erase(args_.begin() + index);
    return true;
}

void glimmer::ResourceRef::SetPackageId(const std::string &packId) {
    packId_ = packId;
}

void glimmer::ResourceRef::FromMessage(const ResourceRefMessage &resourceRefMessage) {
    packId_ = resourceRefMessage.packid();
    resourceType_ = resourceRefMessage.resourcetype();
    resourceKey_ = resourceRefMessage.resourcekey();
    selfPackageId_ = resourceRefMessage.selfpackageid();
    const size_t size = resourceRefMessage.args_size();
    args_.resize(size);
    args_.clear();
    for (int i = 0; i < size; ++i) {
        auto &resourceRefArgMessage = resourceRefMessage.args(i);
        ResourceRefArg resourceRefArg;
        resourceRefArg.FromMessage(resourceRefArgMessage);
        args_.push_back(std::move(resourceRefArg));
    }
    bindPackage_ = true;
}

void glimmer::ResourceRef::ToMessage(ResourceRefMessage &resourceRefMessage) {
    resourceRefMessage.set_packid(packId_);
    resourceRefMessage.set_resourcetype(resourceType_);
    resourceRefMessage.set_resourcekey(resourceKey_);
    resourceRefMessage.set_selfpackageid(selfPackageId_);
    const size_t size = args_.size();
    for (int i = 0; i < size; ++i) {
        ResourceRefArg &arg = args_[i];
        arg.ToMessage(*resourceRefMessage.add_args());
    }
}

std::string glimmer::ResourceRef::GetPackageId() const {
    if (!bindPackage_) {
        LogCat::e("Unbound packages may return placeholders.");
        assert(false);
    }
    return packId_;
}

void glimmer::ResourceRef::SetResourceType(const uint32_t resourceType) {
    resourceType_ = resourceType;
}

uint32_t glimmer::ResourceRef::GetResourceType() const {
    return resourceType_;
}

void glimmer::ResourceRef::SetResourceKey(const std::string &resourceKey) {
    resourceKey_ = resourceKey;
}

std::string glimmer::ResourceRef::GetResourceKey() const {
    return resourceKey_;
}
