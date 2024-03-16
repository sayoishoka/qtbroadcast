#include <QSlider>
#include <QToolTip>

class MySlider : public QSlider
{
    Q_OBJECT

public:
    MySlider(QWidget *parent = nullptr) : QSlider(parent) {
        setOrientation(Qt::Horizontal);
        connect(this, &QSlider::valueChanged, this, &MySlider::updateToolTip);
    }

protected:
    void enterEvent(QEvent *event) override {
        QSlider::enterEvent(event);
        QToolTip::showText(QCursor::pos(), QString::number(value()));
    }

    void leaveEvent(QEvent *event) override {
        QSlider::leaveEvent(event);
        QToolTip::hideText();
    }

private slots:
    void updateToolTip(int value) {
        QToolTip::showText(QCursor::pos(), QString::number(value));
    }
};
