// Copyright (C) 2021 Petros Koutsolampros

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "aglmapviewrenderer.h"
#include "aglmapview.h"
#include "agl/model/aglmapsmodel.h"

#include <QQuickOpenGLUtils>

void AGLMapViewRenderer::synchronize(QQuickFramebufferObject *item) {
    AGLMapView *glView = static_cast<AGLMapView *>(item);
    m_eyePosX = glView->getEyePosX();
    m_eyePosY = glView->getEyePosY();
    m_zoomFactor = glView->getZoomFactor();
    m_mouseDragRect = glView->getMouseDragRect();
    m_foregroundColour = glView->getForegroundColour();
    m_backgroundColour = glView->getBackgroundColour();
    m_backgroundColourChanged = true;
    recalcView();
}


AGLMapViewRenderer::AGLMapViewRenderer(const QQuickFramebufferObject *item,
                                       const GraphDocument *pDoc, const QColor &foregrounColour,
                                       const QColor &backgroundColour, int antialiasingSamples,
                                       bool highlightOnHover)
    : m_item(static_cast<const AGLMapView *>(item)), m_model(new AGLMapsModel(*pDoc)),
      m_foregroundColour(foregrounColour),
      m_backgroundColour(backgroundColour), m_antialiasingSamples(antialiasingSamples),
      m_highlightOnHover(highlightOnHover) {
    m_core = QCoreApplication::arguments().contains(QStringLiteral("--coreprofile"));
    if (m_antialiasingSamples) {
        QSurfaceFormat format;
        format.setSamples(m_antialiasingSamples);
    }

    loadAxes();

    m_model->loadGLObjects();

    m_dragLine.setStrokeColour(m_foregroundColour);
    m_selectionRect.setStrokeColour(m_backgroundColour);

    QQuickOpenGLUtils::resetOpenGLState();

    m_backgroundColourChanged = true;

    m_selectionRect.initializeGL(m_core);
    m_dragLine.initializeGL(m_core);
    m_axes.initializeGL(m_core);

    m_model->initializeGL(m_core);

    m_model->loadGLObjectsRequiringGLContext();

    m_mModel.setToIdentity();

    m_mView.setToIdentity();
    m_mView.translate(0, 0, -1);
}

AGLMapViewRenderer::~AGLMapViewRenderer() {
    m_selectionRect.cleanup();
    m_dragLine.cleanup();
    m_axes.cleanup();
    m_model->cleanup();
}

void AGLMapViewRenderer::render() {
    if (!m_item->getGraphDocument().hasMetaGraph())
        return;

    if (m_backgroundColourChanged) {
        // TODO: This should be happening in the ctor, however
        // this particular qt opengl implementation does not
        // work in that way, so we have to do it here
        glClearColor(m_backgroundColour.redF(),
                     m_backgroundColour.greenF(),
                     m_backgroundColour.blueF(),
                     1);
        m_backgroundColourChanged = false;
    }

    glEnable(GL_MULTISAMPLE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_model->updateGL(m_core);

    m_axes.paintGL(m_mProj, m_mView, m_mModel);

    m_model->paintGL(m_mProj, m_mView, m_mModel);

    float pos[] = {
        float(std::min(m_mouseDragRect.bottomRight().x(), m_mouseDragRect.topLeft().x())),
        float(std::min(m_mouseDragRect.bottomRight().y(), m_mouseDragRect.topLeft().y())),
        float(std::max(m_mouseDragRect.bottomRight().x(), m_mouseDragRect.topLeft().x())),
        float(std::max(m_mouseDragRect.bottomRight().y(), m_mouseDragRect.topLeft().y()))};
    m_selectionRect.setSelectionBounds(QMatrix2x2(pos));
    m_selectionRect.paintGL(m_mProj, m_mView, m_mModel);

    //    if ((m_mouseMode & MOUSE_MODE_SECOND_POINT) == MOUSE_MODE_SECOND_POINT) {
    //        float pos[] = {float(m_tempFirstPoint.x), float(m_tempFirstPoint.y),
    //        float(m_tempSecondPoint.x),
    //                       float(m_tempSecondPoint.y)};
    //        m_dragLine.paintGL(m_mProj, m_mView, m_mModel, QMatrix2x2(pos));
    //    }

    QQuickOpenGLUtils::resetOpenGLState();
}

void AGLMapViewRenderer::recalcView() {
    GLfloat screenRatio = GLfloat(m_viewportSize.width()) / m_viewportSize.height();
    m_mProj.setToIdentity();

    if (m_perspectiveView) {
        m_mProj.perspective(45.0f, screenRatio, 0.01f, 100.0f);
        m_mProj.scale(1.0f, 1.0f, m_zoomFactor);
    } else {
        m_mProj.ortho(-m_zoomFactor * 0.5f * screenRatio, m_zoomFactor * 0.5f * screenRatio,
                      -m_zoomFactor * 0.5f, m_zoomFactor * 0.5f, 0, 10);
    }
    m_mProj.translate(m_eyePosX, m_eyePosY, 0.0f);
}

void AGLMapViewRenderer::loadAxes() {
    std::vector<std::pair<SimpleLine, PafColor>> axesData;
    axesData.push_back(std::pair<SimpleLine, PafColor>(SimpleLine(0, 0, 1, 0), PafColor(1, 0, 0)));
    axesData.push_back(std::pair<SimpleLine, PafColor>(SimpleLine(0, 0, 0, 1), PafColor(0, 1, 0)));
    m_axes.loadLineData(axesData);
}


