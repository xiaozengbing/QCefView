﻿#include <QCefView.h>

#pragma region qt_headers
#include <QPainter>
#include <QPoint>
#include <QResizeEvent>
#include <QStyleOption>
#include <QVBoxLayout>
#include <QtDebug>
#pragma endregion qt_headers

#include <QCefContext.h>

#include "details/QCefEventPrivate.h"
#include "details/QCefViewPrivate.h"
#include "details/utils/CommonUtils.h"

QCefView::QCefView(const QString url, const QCefSetting* setting, QWidget* parent /*= 0*/)
  : QWidget(parent)
  , d_ptr(new QCefViewPrivate(QCefContext::instance()->d_func(), this, url, setting))
{
#if defined(CEF_USE_OSR)
  setBackgroundRole(QPalette::Window);
  setAttribute(Qt::WA_OpaquePaintEvent);
#endif

  setMouseTracking(true);
  setFocusPolicy(Qt::WheelFocus);
}

QCefView::QCefView(QWidget* parent /*= 0*/)
  : QCefView("about:blank", nullptr, parent)
{}

QCefView::~QCefView()
{
  qDebug() << this << "is being destructed";
}

void
QCefView::addLocalFolderResource(const QString& path, const QString& url, int priority /*= 0*/)
{
  Q_D(QCefView);

  d->addLocalFolderResource(path, url, priority);
}

void
QCefView::addArchiveResource(const QString& path,
                             const QString& url,
                             const QString& password /*= ""*/,
                             int priority /*= 0*/)
{
  Q_D(QCefView);

  d->addArchiveResource(path, url, password, priority);
}

int
QCefView::browserId()
{
  Q_D(QCefView);

  return d->browserId();
}

void
QCefView::navigateToString(const QString& content)
{
  Q_D(QCefView);

  d->navigateToString(content);
}

void
QCefView::navigateToUrl(const QString& url)
{
  Q_D(QCefView);

  d->navigateToUrl(url);
}

bool
QCefView::browserCanGoBack()
{
  Q_D(QCefView);

  return d->browserCanGoBack();
}

bool
QCefView::browserCanGoForward()
{
  Q_D(QCefView);

  return d->browserCanGoForward();
}

void
QCefView::browserGoBack()
{
  Q_D(QCefView);

  d->browserGoBack();
}

void
QCefView::browserGoForward()
{
  Q_D(QCefView);

  d->browserGoForward();
}

bool
QCefView::browserIsLoading()
{
  Q_D(QCefView);

  return d->browserIsLoading();
}

void
QCefView::browserReload()
{
  Q_D(QCefView);

  d->browserReload();
}

void
QCefView::browserStopLoad()
{
  Q_D(QCefView);

  d->browserStopLoad();
}

bool
QCefView::triggerEvent(const QCefEvent& event)
{
  Q_D(QCefView);

  return d->triggerEvent(event.eventName(), event.d_func()->args, CefViewBrowserClient::MAIN_FRAME);
}

bool
QCefView::triggerEvent(const QCefEvent& event, qint64 frameId)
{
  Q_D(QCefView);

  return d->triggerEvent(event.eventName(), event.d_func()->args, frameId);
}

bool
QCefView::broadcastEvent(const QCefEvent& event)
{
  Q_D(QCefView);

  return d->triggerEvent(event.eventName(), event.d_func()->args, CefViewBrowserClient::ALL_FRAMES);
}

bool
QCefView::responseQCefQuery(const QCefQuery& query)
{
  Q_D(QCefView);

  return d->responseQCefQuery(query);
}

bool
QCefView::executeJavascript(qint64 frameId, const QString& code, const QString& url)
{
  Q_D(QCefView);

  return d->executeJavascript(frameId, code, url);
}

bool
QCefView::executeJavascriptWithResult(qint64 frameId, const QString& code, const QString& url, qint64 context)
{
  Q_D(QCefView);

  return d->executeJavascriptWithResult(frameId, code, url, context);
}

bool
QCefView::setPreference(const QString& name, const QVariant& value, const QString& error)
{
  Q_D(QCefView);

  return d->setPreference(name, value, error);
}

void
QCefView::setDisablePopupContextMenu(bool disable)
{
  Q_D(QCefView);

  d->disablePopuContextMenu_ = disable;
}

bool
QCefView::isPopupContextMenuDisabled()
{
  Q_D(QCefView);

  return d->disablePopuContextMenu_;
}

void
QCefView::setFocus(Qt::FocusReason reason)
{
  Q_D(QCefView);

  d->setCefWindowFocus(true);
}

void
QCefView::onBrowserWindowCreated(QWindow* win)
{}

bool
QCefView::onBeforePopup(qint64 frameId,
                        const QString& targetUrl,
                        const QString& targetFrameName,
                        QCefView::CefWindowOpenDisposition targetDisposition,
                        QCefSetting& settings,
                        bool& DisableJavascriptAccess)
{
  // return false to allow the popup browser
  return false;
}

void
QCefView::onPopupCreated(QWindow* wnd)
{}

QVariant
QCefView::inputMethodQuery(Qt::InputMethodQuery query) const
{
#if defined(CEF_USE_OSR)
  Q_D(const QCefView);
  auto r = d->onViewInputMethodQuery(query);
  if (r.isValid())
    return r;
#endif

  return QWidget::inputMethodQuery(query);
}

void
QCefView::paintEvent(QPaintEvent* event)
{
  Q_D(QCefView);

#if defined(CEF_USE_OSR)
  // 1. constructs painter for current widget
  QPainter painter(this);

  // 2. paint background with background role
  painter.fillRect(rect(), palette().color(backgroundRole()));

  // 3. paint widget with its stylesheet
  QStyleOption opt;
  opt.initFrom(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

  // 4. paint the CEF view and popup
  // get current scale factor
  qreal scaleFactor = devicePixelRatio();

  // perform the painting
  {
    // paint cef view
    QMutexLocker lock(&(d->osr.qViewPaintLock_));
    int width = d->osr.qCefViewFrame_.width() / scaleFactor;
    int height = d->osr.qCefViewFrame_.height() / scaleFactor;
    painter.drawImage(QRect{ 0, 0, width, height }, d->osr.qCefViewFrame_);
  }
  {
    // paint cef popup
    QMutexLocker lock(&(d->osr.qPopupPaintLock_));
    if (d->osr.showPopup_) {
      painter.drawImage(d->osr.qPopupRect_, d->osr.qCefPopupFrame_);
    }
  }
#endif

  // 5. call base paintEvent (empty implementation)
  QWidget::paintEvent(event);
}

void
QCefView::inputMethodEvent(QInputMethodEvent* event)
{
  Q_D(QCefView);
  d->onViewInputMethodEvent(event);
}

void
QCefView::showEvent(QShowEvent* event)
{
  Q_D(QCefView);
  d->onViewVisibilityChanged(true);
  QWidget::showEvent(event);
}

void
QCefView::hideEvent(QHideEvent* event)
{
  Q_D(QCefView);
  d->onViewVisibilityChanged(false);
  QWidget::hideEvent(event);
}

void
QCefView::focusInEvent(QFocusEvent* event)
{
  Q_D(QCefView);
  d->onViewFocusChanged(true);
  QWidget::focusInEvent(event);
}

void
QCefView::focusOutEvent(QFocusEvent* event)
{
  Q_D(QCefView);
  d->onViewFocusChanged(false);
  QWidget::focusOutEvent(event);
}

void
QCefView::resizeEvent(QResizeEvent* event)
{
  Q_D(QCefView);
  d->onViewSizeChanged(event->size(), event->oldSize());
  QWidget::resizeEvent(event);
}

void
QCefView::keyPressEvent(QKeyEvent* event)
{
  Q_D(QCefView);
  d->onViewKeyEvent(event);
  QWidget::keyPressEvent(event);
}

void
QCefView::keyReleaseEvent(QKeyEvent* event)
{
  Q_D(QCefView);
  d->onViewKeyEvent(event);
  QWidget::keyReleaseEvent(event);
}

void
QCefView::mouseMoveEvent(QMouseEvent* event)
{
  Q_D(QCefView);
  d->onViewMouseEvent(event);
  QWidget::mouseMoveEvent(event);
}

void
QCefView::mousePressEvent(QMouseEvent* event)
{
  Q_D(QCefView);
  d->onViewMouseEvent(event);
  QWidget::mousePressEvent(event);
}

void
QCefView::mouseReleaseEvent(QMouseEvent* event)
{
  Q_D(QCefView);
  d->onViewMouseEvent(event);
  QWidget::mouseReleaseEvent(event);
}

void
QCefView::wheelEvent(QWheelEvent* event)
{
  Q_D(QCefView);
  d->onViewWheelEvent(event);
  QWidget::wheelEvent(event);
}

void
QCefView::contextMenuEvent(QContextMenuEvent* event)
{
  FLog();

#if defined(CEF_USE_OSR)
  Q_D(QCefView);

  if (d->osr.isShowingContextMenu_) {
    d->osr.contextMenu_->popup(mapToGlobal(event->pos()));
  }
#endif
}
