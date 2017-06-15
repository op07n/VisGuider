#ifndef STDAFX_H
#define STDAFX_H

/// marble
// widget
#include <marble/MarbleWidget.h>
#include <marble/MarbleGlobal.h>
#include <marble/GeoDataDocument.h>
#include <marble/GeoDataPlacemark.h>
#include <marble/GeoDataLineString.h>
#include <marble/GeoDataTreeModel.h>
#include <marble/MarbleModel.h>
// style
#include <marble/GeoDataStyle.h>
#include <marble/GeoDataIconStyle.h>
#include <marble/GeoDataLabelStyle.h>
#include <marble/GeoDataLineStyle.h>
#include <marble/GeoDataPolyStyle.h>
#include <marble/GeoDataBalloonStyle.h>
// layer
#include <marble/MarbleMap.h>
#include <marble/GeoPainter.h>
#include <marble/LayerInterface.h>
#include <PopupLayer.h>

#include <marble/GeoDataLookAt.h>

#include <marble/GeoDataLatLonBox.h>
#include <marble/GeoDataLinearRing.h>
using namespace Marble;

/// Webview
#include <QWebView>
#include <QWebFrame>

/// Jansson
#include <jansson.h>

#include <QFontDatabase>
#include <QDebug>
#include <QFontInfo>
#include <QTime>
#include <QTimer>
#include <QKeyEvent>
#include <QString>
#include <QStringList>
#include <QColor>
#include <QWidget>
#include <QImage>
#include <QApplication>

#include <map>
#include <vector>

#endif // STDAFX_H
