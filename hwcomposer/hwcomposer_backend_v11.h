/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: Thomas Perl <thomas.perl@jolla.com>
**
** This file is part of the hwcomposer plugin.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef HWCOMPOSER_BACKEND_V11_H
#define HWCOMPOSER_BACKEND_V11_H

#ifdef HWC_PLUGIN_HAVE_HWCOMPOSER1_API

#include "hwcomposer_backend.h"

// libhybris access to the native hwcomposer window
#include <hwcomposer_window.h>

#include "hwcinterface.h"

class QWindow;

class HWC11WindowSurface;
class HWC11Thread;
class HWC11FdReleaseQueue;

class HwComposerBackend_v11 : public QObject, public HwComposerBackend, public HwcInterface::Compositor {
public:
    HwComposerBackend_v11(hw_module_t *hwc_module, hw_device_t *hw_device, int num_displays);
    virtual ~HwComposerBackend_v11();

    virtual EGLNativeDisplayType display();
    virtual EGLNativeWindowType createWindow(int width, int height);
    virtual void destroyWindow(EGLNativeWindowType window);
    virtual void swap(EGLNativeDisplayType display, EGLSurface surface);
    virtual void sleepDisplay(bool sleep);
    virtual float refreshRate();

    HwcInterface::Compositor *hwcInterface() Q_DECL_OVERRIDE { return this; }
    void scheduleLayerList(HwcInterface::LayerList *list) Q_DECL_OVERRIDE;
    const HwcInterface::LayerList *acceptedLayerList() const Q_DECL_OVERRIDE;
    void swapLayerList(HwcInterface::LayerList *list) Q_DECL_OVERRIDE;
    void setReleaseLayerListCallback(ReleaseLayerListCallback callback) { m_releaseLayerListCallback = callback; }
    void setBufferAvailableCallback(BufferAvailableCallback callback, void *cbData) {
        m_bufferAvailableCallback = callback;
        m_bufferAvailableCallbackData = cbData;
    }
    void setInvalidateCallback(InvalidateCallback callback, void *data) {
        m_invalidateCallback = callback;
        m_invalidateCallbackData = data;
    }

    void present(HWComposerNativeWindowBuffer *b);

    bool requestUpdate(QWindow *window);
    void onVSync();
    uint32_t hwcVersion() { return hwc_version; }

protected:
    void timerEvent(QTimerEvent *te);
    bool event(QEvent *e);

private:
    friend class HWC11FdReleaseQueue;
    friend class HWC11Thread;
    inline bool waitForComposer() const { return m_layerListBuffers.size() > 0 || m_eglSurfaceBuffer; }
    void startVSyncCountdown();
    void stopVSyncCountdown();
    void deliverUpdateRequests();

    HWC11Thread *m_thread;
    HwcInterface::LayerList *m_scheduledLayerList;

    ReleaseLayerListCallback m_releaseLayerListCallback;
    BufferAvailableCallback m_bufferAvailableCallback;
    void *m_bufferAvailableCallbackData;
    InvalidateCallback m_invalidateCallback;
    void *m_invalidateCallbackData;

    HWComposerNativeWindowBuffer *m_eglSurfaceBuffer;
    bool m_eglWithLayerList;
    QVarLengthArray<void *, 8> m_layerListBuffers;

    QSet<QWindow *> m_windowsPendingUpdate;
    int m_vsyncCountDown;
    int m_timeToUpdateTimer;

    QAtomicInt m_swappingLayersOnly;

    uint32_t hwc_version;
};

#endif /* HWC_PLUGIN_HAVE_HWCOMPOSER1_API */

#endif /* HWCOMPOSER_BACKEND_V11_H */
