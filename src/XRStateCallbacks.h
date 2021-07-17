// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2021 James Hogan <james@albanarts.com>

#ifndef OSGXR_XRSTATE_CALLBACKS
#define OSGXR_XRSTATE_CALLBACKS 1

#include "XRState.h"

#include <osg/Camera>
#include <osg/GraphicsContext>
#include <osg/View>

#include <osgUtil/SceneView>

namespace osgXR {

class UpdateSlaveCallback : public osg::View::Slave::UpdateSlaveCallback
{
    public:

        UpdateSlaveCallback(uint32_t viewIndex, osg::ref_ptr<XRState> xrState) :
            _viewIndex(viewIndex),
            _xrState(xrState)
        {
        }

        virtual void updateSlave(osg::View& view, osg::View::Slave& slave)
        {
            _xrState->updateSlave(_viewIndex, view, slave);
        }

    protected:

        uint32_t _viewIndex;
        osg::observer_ptr<XRState> _xrState;
};

class ComputeStereoMatricesCallback : public osgUtil::SceneView::ComputeStereoMatricesCallback
{
    public:

        ComputeStereoMatricesCallback(osg::ref_ptr<XRState> xrState) :
            _xrState(xrState)
        {
        }

        virtual osg::Matrixd computeLeftEyeProjection(const osg::Matrixd& projection) const
        {
            return _xrState->getEyeProjection(0, projection);
        }

        virtual osg::Matrixd computeLeftEyeView(const osg::Matrixd& view) const
        {
            return _xrState->getEyeView(0, view);
        }

        virtual osg::Matrixd computeRightEyeProjection(const osg::Matrixd& projection) const
        {
            return _xrState->getEyeProjection(1, projection);
        }

        virtual osg::Matrixd computeRightEyeView(const osg::Matrixd& view) const
        {
            return _xrState->getEyeView(1, view);
        }

    protected:

        osg::observer_ptr<XRState> _xrState;
};

class InitialDrawCallback : public osg::Camera::DrawCallback
{
    public:

        InitialDrawCallback(osg::ref_ptr<XRState> xrState) :
            _xrState(xrState)
        {
        }

        virtual void operator()(osg::RenderInfo& renderInfo) const
        {
            _xrState->initialDrawCallback(renderInfo);
        }

    protected:

        osg::observer_ptr<XRState> _xrState;
};

class PreDrawCallback : public osg::Camera::DrawCallback
{
    public:

        PreDrawCallback(osg::ref_ptr<XRState::XRSwapchain> xrSwapchain) :
            _xrSwapchain(xrSwapchain)
        {
        }

        virtual void operator()(osg::RenderInfo& renderInfo) const
        {
            _xrSwapchain->preDrawCallback(renderInfo);
        }

    protected:

        osg::observer_ptr<XRState::XRSwapchain> _xrSwapchain;
};

class PostDrawCallback : public osg::Camera::DrawCallback
{
    public:

        PostDrawCallback(osg::ref_ptr<XRState::XRSwapchain> xrSwapchain) :
            _xrSwapchain(xrSwapchain)
        {
        }

        virtual void operator()(osg::RenderInfo& renderInfo) const
        {
            _xrSwapchain->postDrawCallback(renderInfo);
        }

    protected:

        osg::observer_ptr<XRState::XRSwapchain> _xrSwapchain;
};

class SwapCallback : public osg::GraphicsContext::SwapCallback
{
    public:

        explicit SwapCallback(osg::ref_ptr<XRState> xrState) :
            _xrState(xrState),
            _frameIndex(0)
        {
        }

        void swapBuffersImplementation(osg::GraphicsContext* gc)
        {
            _xrState->swapBuffersImplementation(gc);
        }

        int frameIndex() const
        {
            return _frameIndex;
        }

    private:

        osg::observer_ptr<XRState> _xrState;
        int _frameIndex;
};

}

#endif