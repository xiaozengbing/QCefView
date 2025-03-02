﻿#include "CCefClientDelegate.h"

#include <QBitmap>
#include <QByteArray>
#include <QCursor>
#include <QIcon>

#include "QCefViewPrivate.h"

Qt::CursorShape
mapCursorShape(cef_cursor_type_t& type)
{
  static Qt::CursorShape sCursorTable[] = {
    Qt::ArrowCursor,        // CT_POINTER = 0,
    Qt::CrossCursor,        // CT_CROSS,
    Qt::PointingHandCursor, // CT_HAND,
    Qt::IBeamCursor,        // CT_IBEAM,
    Qt::WaitCursor,         // CT_WAIT,
    Qt::WhatsThisCursor,    // CT_HELP,
    Qt::SizeHorCursor,      // CT_EASTRESIZE,
    Qt::SizeVerCursor,      // CT_NORTHRESIZE,
    Qt::SizeBDiagCursor,    // CT_NORTHEASTRESIZE,
    Qt::SizeFDiagCursor,    // CT_NORTHWESTRESIZE,
    Qt::SizeVerCursor,      // CT_SOUTHRESIZE,
    Qt::SizeFDiagCursor,    // CT_SOUTHEASTRESIZE,
    Qt::SizeBDiagCursor,    // CT_SOUTHWESTRESIZE,
    Qt::SizeHorCursor,      // CT_WESTRESIZE,
    Qt::SizeVerCursor,      // CT_NORTHSOUTHRESIZE,
    Qt::SizeHorCursor,      // CT_EASTWESTRESIZE,
    Qt::SizeAllCursor,      // CT_NORTHEASTSOUTHWESTRESIZE,
    Qt::SizeAllCursor,      // CT_NORTHWESTSOUTHEASTRESIZE,
    Qt::SplitHCursor,       // CT_COLUMNRESIZE,
    Qt::SplitVCursor,       // CT_ROWRESIZE,
    Qt::ArrowCursor,        // CT_MIDDLEPANNING,
    Qt::ArrowCursor,        // CT_EASTPANNING,
    Qt::ArrowCursor,        // CT_NORTHPANNING,
    Qt::ArrowCursor,        // CT_NORTHEASTPANNING,
    Qt::ArrowCursor,        // CT_NORTHWESTPANNING,
    Qt::ArrowCursor,        // CT_SOUTHPANNING,
    Qt::ArrowCursor,        // CT_SOUTHEASTPANNING,
    Qt::ArrowCursor,        // CT_SOUTHWESTPANNING,
    Qt::ArrowCursor,        // CT_WESTPANNING,
    Qt::ArrowCursor,        // CT_MOVE,
    Qt::ArrowCursor,        // CT_VERTICALTEXT,
    Qt::ArrowCursor,        // CT_CELL,
    Qt::ArrowCursor,        // CT_CONTEXTMENU,
    Qt::ArrowCursor,        // CT_ALIAS,
    Qt::ArrowCursor,        // CT_PROGRESS,
    Qt::ArrowCursor,        // CT_NODROP,
    Qt::ArrowCursor,        // CT_COPY,
    Qt::ArrowCursor,        // CT_NONE,
    Qt::ForbiddenCursor,    // CT_NOTALLOWED,
    Qt::ArrowCursor,        // CT_ZOOMIN,
    Qt::ArrowCursor,        // CT_ZOOMOUT,
    Qt::OpenHandCursor,     // CT_GRAB,
    Qt::ClosedHandCursor,   // CT_GRABBING,
    Qt::ArrowCursor,        // CT_MIDDLE_PANNING_VERTICAL,
    Qt::ArrowCursor,        // CT_MIDDLE_PANNING_HORIZONTAL,
    Qt::ArrowCursor,        // CT_CUSTOM,
    Qt::ArrowCursor,        // CT_DND_NONE,
    Qt::DragMoveCursor,     // CT_DND_MOVE,
    Qt::DragCopyCursor,     // CT_DND_COPY,
    Qt::DragLinkCursor,     // CT_DND_LINK,
  };
  return sCursorTable[type];
}

void
CCefClientDelegate::draggableRegionChanged(CefRefPtr<CefBrowser>& browser,
                                           const std::vector<CefDraggableRegion>& regions)
{
  if (!IsValidBrowser(browser))
    return;

  // Determine new draggable region.
  QRegion draggableRegion;
  QRegion nonDraggableRegion;
  std::vector<CefDraggableRegion>::const_iterator it = regions.begin();
  for (; it != regions.end(); ++it) {
    if (it->draggable) {
      draggableRegion += QRegion(it->bounds.x, it->bounds.y, it->bounds.width, it->bounds.height);
    } else {
      nonDraggableRegion += QRegion(it->bounds.x, it->bounds.y, it->bounds.width, it->bounds.height);
    }
  }

  pCefViewPrivate_->q_ptr->draggableRegionChanged(draggableRegion, nonDraggableRegion);
}

void
CCefClientDelegate::addressChanged(CefRefPtr<CefBrowser>& browser, int64_t frameId, const std::string& url)
{
  if (!IsValidBrowser(browser))
    return;

  auto u = QString::fromStdString(url);
  pCefViewPrivate_->q_ptr->addressChanged(frameId, u);
}

void
CCefClientDelegate::titleChanged(CefRefPtr<CefBrowser>& browser, const std::string& title)
{
  if (!IsValidBrowser(browser))
    return;

  auto t = QString::fromStdString(title);
  pCefViewPrivate_->q_ptr->titleChanged(t);
}

void
CCefClientDelegate::faviconURLChanged(CefRefPtr<CefBrowser> browser, const std::vector<CefString>& icon_urls)
{
  if (!IsValidBrowser(browser))
    return;

  QStringList urls;
  for (auto& iconUrl : icon_urls) {
    urls.append(QString::fromStdString(iconUrl.ToString()));
  }

  pCefViewPrivate_->q_ptr->faviconURLChanged(urls);
}

void
CCefClientDelegate::faviconChanged(CefRefPtr<CefImage> image)
{
  QIcon icon;

  int width = 0;
  int height = 0;
  CefRefPtr<CefBinaryValue> data = image->GetAsPNG(1.0, true, width, height);
  int bufsize = (int)data->GetSize();
  if (bufsize > 0) {
    QByteArray buffer(bufsize + 4, char(0));
    data->GetData(buffer.data(), bufsize, 0);

    QBitmap bitmap;
    bitmap.loadFromData(buffer);

    icon = QIcon(bitmap);
  }

  pCefViewPrivate_->q_ptr->faviconChanged(icon);
}

void
CCefClientDelegate::fullscreenModeChanged(CefRefPtr<CefBrowser>& browser, bool fullscreen)
{
  if (!IsValidBrowser(browser))
    return;

  pCefViewPrivate_->q_ptr->fullscreenModeChanged(fullscreen);
}

bool
CCefClientDelegate::tooltipMessage(CefRefPtr<CefBrowser>& browser, const std::string& text)
{
  // allow the tooltip action
  return false;
}

void
CCefClientDelegate::statusMessage(CefRefPtr<CefBrowser>& browser, const std::string& value)
{
  if (!IsValidBrowser(browser))
    return;

  auto msg = QString::fromStdString(value);
  pCefViewPrivate_->q_ptr->statusMessage(msg);
}

void
CCefClientDelegate::consoleMessage(CefRefPtr<CefBrowser>& browser, const std::string& message, int level)
{
  if (!IsValidBrowser(browser))
    return;

  auto msg = QString::fromStdString(message);
  pCefViewPrivate_->q_ptr->consoleMessage(msg, level);
}

void
CCefClientDelegate::loadingProgressChanged(CefRefPtr<CefBrowser>& browser, double progress)
{
  if (!IsValidBrowser(browser))
    return;

  pCefViewPrivate_->q_ptr->loadingProgressChanged(progress);
}

bool
CCefClientDelegate::cursorChanged(CefRefPtr<CefBrowser> browser,
                                  CefCursorHandle cursor,
                                  cef_cursor_type_t type,
                                  const CefCursorInfo& custom_cursor_info)
{
  if (!IsValidBrowser(browser))
    return false;

#if defined(CEF_USE_OSR)
  QCursor cur;
  if (type != CT_CUSTOM) {
    cur.setShape(mapCursorShape(type));
  } else {
  }

  QMetaObject::invokeMethod(pCefViewPrivate_, "onCefUpdateCursor", Q_ARG(const QCursor&, cur));

  return true;
#else
  return false;
#endif
}
