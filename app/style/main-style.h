#ifndef MAINSTYLE_H
#define MAINSTYLE_H

#include <QProxyStyle>

class MainStyle : public QProxyStyle
{
    Q_OBJECT
public:
    explicit MainStyle(const QString& key);


    // 重写
public:
    int pixelMetric(PixelMetric m, const QStyleOption *opt, const QWidget *widget) const override;
    QRect subElementRect(SubElement sr, const QStyleOption *opt, const QWidget *widget) const override;
    void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *widget) const override;
    void drawControl(ControlElement element, const QStyleOption *opt, QPainter *p, const QWidget *w = nullptr) const override;
    QSize sizeFromContents(ContentsType ct, const QStyleOption *opt, const QSize &csz, const QWidget *widget) const override;

    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const override;

};

#endif // MAINSTYLE_H
