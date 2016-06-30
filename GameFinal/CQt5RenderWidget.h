#ifndef __QT5_RENDER_WIDGET_CLASS_H__
#define __QT5_RENDER_WIDGET_CLASS_H__

#include "CQt5App.h"
#include <qwidget.h>
#include <qtimer.h>

namespace gf
{
	class CQt5Window;

	class CQt5RenderWidget : public QWidget
	{
	public:
		CQt5RenderWidget(QWidget* parent, CQt5Window* window)
			: QWidget(parent),
			mWindow(window)
		{
			this->setWindowTitle(QString("GameFinal App"));
			this->setFixedSize(800, 600);

			setAttribute(Qt::WA_PaintOnScreen, true);
			setAttribute(Qt::WA_NativeWindow, true);

			mQTimer = new QTimer(this);
			QObject::connect(mQTimer, SIGNAL(timeout()), this, SLOT(update()));
			mQTimer->start();
		}

		virtual QPaintEngine* paintEngine() const
		{
			return nullptr;
		}

		virtual void paintEvent(QPaintEvent *event);

	private:
		QTimer*								mQTimer;
		CQt5Window*							mWindow;

	};
}


#endif
