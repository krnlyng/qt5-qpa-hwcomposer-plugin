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

#include "hwcomposer_backend.h"
#include "hwcomposer_backend_v0.h"
#include "hwcomposer_backend_v1.h"


HwComposerBackend::HwComposerBackend(hw_module_t *hwc_module)
    : hwc_module(hwc_module)
{
}

HwComposerBackend::~HwComposerBackend()
{
    // XXX: Close/free hwc_module?
}

HwComposerBackend *
HwComposerBackend::create()
{
    hw_module_t *hwc_module = NULL;

    // Open hardware composer
    HWC_PLUGIN_ASSERT_ZERO(hw_get_module(HWC_HARDWARE_MODULE_ID, (const hw_module_t **)(&hwc_module)));

    fprintf(stderr, "== hwcomposer ==\n");
    fprintf(stderr, " * Module API Version: %x\n", hwc_module->module_api_version);
    fprintf(stderr, " * HAL API Version: %x\n", hwc_module->hal_api_version); /* should be zero */
    fprintf(stderr, " * Identifier: %s\n", hwc_module->id);
    fprintf(stderr, " * Name: %s\n", hwc_module->name);
    fprintf(stderr, " * Author: %s\n", hwc_module->author);
    fprintf(stderr, "== hwcomposer ==\n");

    // Determine which backend we use based on the supported module API version
    switch (hwc_module->module_api_version) {
        case HWC_DEVICE_API_VERSION_0_1:
        case HWC_DEVICE_API_VERSION_0_2:
        case HWC_DEVICE_API_VERSION_0_3:
            return new HwComposerBackend_v0(hwc_module);
            break;
#ifdef HWC_PLUGIN_HAVE_HWCOMPOSER1_API
        case HWC_DEVICE_API_VERSION_1_0:
        case HWC_DEVICE_API_VERSION_1_1:
        case HWC_DEVICE_API_VERSION_1_2:
        case HWC_DEVICE_API_VERSION_1_3:
            return new HwComposerBackend_v1(hwc_module);
            break;
#endif /* HWC_PLUGIN_HAVE_HWCOMPOSER1_API */
        default:
            fprintf(stderr, "Unknown hwcomposer API: 0x%x\n",
                    hwc_module->module_api_version);
            return NULL;
            break;
    }
}

void
HwComposerBackend::destroy(HwComposerBackend *backend)
{
    delete backend;
}
