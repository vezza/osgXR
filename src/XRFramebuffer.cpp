// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2021 James Hogan <james@albanarts.com>

#include "XRFramebuffer.h"

#include <osg/State>
#include <osg/Version>

using namespace osgXR;

#if(OSG_VERSION_GREATER_OR_EQUAL(3, 4, 0))
typedef osg::GLExtensions OSG_GLExtensions;
#else
typedef osg::FBOExtensions OSG_GLExtensions;
#endif

static const OSG_GLExtensions* getGLExtensions(const osg::State& state)
{
#if(OSG_VERSION_GREATER_OR_EQUAL(3, 4, 0))
    return state.get<osg::GLExtensions>();
#else
    return osg::FBOExtensions::instance(state.getContextID(), true);
#endif
}

XRFramebuffer::XRFramebuffer(uint32_t width, uint32_t height,
                             GLuint texture, GLuint depthTexture) :
    _width(width),
    _height(height),
    _depthFormat(GL_DEPTH_COMPONENT16),
    _fbo(0),
    _texture(texture),
    _depthTexture(depthTexture),
    _generated(false),
    _boundTexture(false),
    _boundDepthTexture(false),
    _deleteDepthTexture(false)
{
}

XRFramebuffer::~XRFramebuffer()
{
    // FIXME
    /*
    if (_fbo)
    {
        const OSG_GLExtensions *fbo_ext = getGLExtensions(state);
        fbo_ext->glDeleteFramebuffers(1, &_fbo);
    }
    if (_deleteDepthTexture)
    {
        glDeleteTextures(1, &_depthTexture);
    }
    */
}

bool XRFramebuffer::valid(osg::State &state) const
{
    if (!_fbo)
        return false;

    const OSG_GLExtensions *fbo_ext = getGLExtensions(state);
    GLenum complete = fbo_ext->glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);
    switch (complete) {
    case GL_FRAMEBUFFER_COMPLETE:
        return true;
    case GL_FRAMEBUFFER_UNDEFINED:
        OSG_WARN << "FBO Undefined" << std::endl;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        OSG_WARN << "FBO Incomplete attachment" << std::endl;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        OSG_WARN << "FBO Incomplete missing attachment" << std::endl;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        OSG_WARN << "FBO Incomplete draw buffer" << std::endl;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        OSG_WARN << "FBO Incomplete read buffer" << std::endl;
        break;
    case GL_FRAMEBUFFER_UNSUPPORTED:
        OSG_WARN << "FBO Incomplete unsupported" << std::endl;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        OSG_WARN << "FBO Incomplete multisample" << std::endl;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        OSG_WARN << "FBO Incomplete layer targets" << std::endl;
        break;
    default:
        OSG_WARN << "FBO Incomplete ???" << std::endl;
        break;
    }
    return false;
}

void XRFramebuffer::bind(osg::State &state)
{
    const OSG_GLExtensions *fbo_ext = getGLExtensions(state);

    if (!_fbo && !_generated)
    {
        fbo_ext->glGenFramebuffers(1, &_fbo);
        _generated = true;
    }

    if (_fbo) {
        fbo_ext->glBindFramebuffer(GL_FRAMEBUFFER_EXT, _fbo);
        if (!_boundTexture && _texture)
        {
            fbo_ext->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);
            _boundTexture = true;
        }
        if (!_boundDepthTexture)
        {
            if (!_depthTexture)
            {
                glGenTextures(1, &_depthTexture);
                glBindTexture(GL_TEXTURE_2D, _depthTexture);
                glTexImage2D(GL_TEXTURE_2D, 0, _depthFormat, _width, _height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
                glBindTexture(GL_TEXTURE_2D, 0);

                _deleteDepthTexture = true;
            }

            fbo_ext->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthTexture, 0);
            _boundDepthTexture = true;

            valid(state);
        }
    }
}

void XRFramebuffer::unbind(osg::State &state)
{
    const OSG_GLExtensions *fbo_ext = getGLExtensions(state);

    if (_fbo && _generated)
        fbo_ext->glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
}