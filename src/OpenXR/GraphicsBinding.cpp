// SPDX-License-Identifier: LGPL-2.1-only
// Copyright (C) 2021 James Hogan <james@albanarts.com>

#include "GraphicsBinding.h"
#include "GraphicsBindingX11.h"

#include <vector>

using namespace osgXR;
using namespace OpenXR;

namespace osgXR {

namespace OpenXR {

class GraphicsBindingProxy : public osg::Referenced
{
    public:
        virtual ~GraphicsBindingProxy() {}

        virtual GraphicsBinding *create(osgViewer::GraphicsWindow *window) = 0;
};

template <typename GRAPHICS_BINDING>
class GraphicsBindingProxyImpl : public GraphicsBindingProxy
{
    protected:
        typedef GRAPHICS_BINDING Binding;
        typedef typename Binding::GraphicsWindow Window;

        virtual ~GraphicsBindingProxyImpl() {}

    public:
        virtual GraphicsBinding *create(osgViewer::GraphicsWindow *window)
        {
            Window *win = dynamic_cast<Window *>(window);
            if (!win)
                return nullptr;

            return new Binding(win);
        }
};

} // osgXR::OpenXR

} // osgXR

typedef std::vector<osg::ref_ptr<GraphicsBindingProxy> > ProxyList;

static ProxyList proxies = {
#ifdef OSGXR_USE_X11
    new GraphicsBindingProxyImpl<GraphicsBindingX11>(),
#endif
};

osg::ref_ptr<GraphicsBinding> osgXR::OpenXR::createGraphicsBinding(osgViewer::GraphicsWindow *window)
{
    GraphicsBinding *ret = nullptr;
    for (GraphicsBindingProxy *proxy: proxies)
    {
        ret = proxy->create(window);
        if (ret)
            break;
    }
    return ret;
}