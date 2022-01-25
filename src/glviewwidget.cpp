#include "document.h"
#include "glviewwidget.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Shaders/defaultvaos.h"
#include "Shaders/transparencyshader.h"
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <QPainter>
#include <QCursor>
#include <QHelpEvent>
#include <QToolTip>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QEvent>
#include <cmath>
#include <chrono>
#include <iostream>

struct Matrices
{
	glm::mat4 u_projection;
	glm::mat4 u_camera;
	glm::ivec4 u_screenSize;
	float      u_ctime;
	float      u_zoom;
	float	   u_pad[2];
};

GLViewWidget::GLViewWidget(QWidget * p) :
	QOpenGLWidget(p),
	timer(this)
{
	TransparencyShader::Shader.AddRef();

	timer.setSingleShot(false);
	timer.setInterval(20);
	connect(&timer, &QTimer::timeout, this, [this]() { repaint(); } );

	current_time = std::chrono::high_resolution_clock::now();
}

GLViewWidget::~GLViewWidget()
{
	glAssert();
    TransparencyShader::Shader.Release(this);
}

glm::vec2 GLViewWidget::GetScreenPosition(QMouseEvent * event)
{
	return glm::vec2(event->pos().x() - width()*.5f,
			   -1 * (event->pos().y() - height()*.5f));
}

glm::vec2 GLViewWidget::GetWorldPosition(QMouseEvent * event)
{
    return w->document->GetScreenCenter() + GetScreenPosition(event) / w->GetZoom();
}

glm::vec2 GLViewWidget::GetScreenPosition()
{
	if(!QWidget::underMouse())
		return glm::vec2(0, 0);

	QPoint pos = mapFromGlobal(QCursor::pos());

	return glm::vec2(pos.x() - width()*.5f,
			   -1 * (pos.y() - height()*.5f));
}

glm::vec2 GLViewWidget::GetWorldPosition()
{
    return w->document->GetScreenCenter() + GetScreenPosition() / w->GetZoom();
}


Bitwise   GLViewWidget::GetFlags(QMouseEvent * event)
{
	auto modifier = event->modifiers();

	modifier &= ~Qt::AltModifier;

	if(modifier == Qt::ControlModifier)
		return Bitwise::XOR;
	else if(modifier == Qt::ShiftModifier)
		return Bitwise::OR;
	else if(modifier == (Qt::ShiftModifier|Qt::ControlModifier))
		return Bitwise::AND;

	return Bitwise::SET;
}

void GLViewWidget::need_repaint()
{
	if(!timer.isActive())
		timer.start();
}

void GLViewWidget::initializeGL()
{
	static bool initialized = false;

	if(!initialized)
	{
		initialized = true;
		QOpenGLFunctions_3_2_Core::initializeOpenGLFunctions();
	}

    glClearColor(0, 0, 0, 1);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenBuffers(1, &m_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrices), nullptr, GL_DYNAMIC_DRAW);

	glAssert();

}

void GLViewWidget::mouseMoveEvent(QMouseEvent * event)
{
	w->SetStatusBarMessage(GetWorldPosition(event));

	if(m_canvasDrag && w && w->document)
	{
		auto screenPos = GetScreenPosition();
		auto delta    = m_screenPos - screenPos;

		glm::vec2 dimensions         = w->document->GetDimensions();
		glm::vec2 world_position     = m_scrollPos * dimensions + delta / w->GetZoom();
		glm::vec2 scroll_destination = world_position / dimensions;

		w->SetScroll(scroll_destination);
	}

	super::mouseMoveEvent(event);
}


void GLViewWidget::mousePressEvent(QMouseEvent * event)
{
	if((event->button() & Qt::MiddleButton)
	&& w->document != nullptr)
	{
		m_canvasDrag  = true;
		m_screenPos   = GetScreenPosition();
		m_scrollPos   = w->GetScroll();
	}

	if((event->button() & Qt::LeftButton) == false)
		super::mousePressEvent(event);
/*	else
	{
        if(w->toolbox.OnLeftDown(GetWorldPosition(event), GetFlags(event), (event->modifiers() & Qt::AltModifier)))
			need_repaint();

		if(w->toolbox.HaveTool() == false)
		{
			setMouseTracking(false);
			w->SetStatusBarMessage();
		}
	}*/
}

void GLViewWidget::mouseReleaseEvent(QMouseEvent * event)
{
	if((event->button() & Qt::MiddleButton))
	{
		m_canvasDrag  = false;
	}

	if((event->button() & Qt::LeftButton) == false)
		super::mouseReleaseEvent(event);
/*	else
	{
        if(w->toolbox.OnLeftUp(GetWorldPosition(event), GetFlags(event), (event->modifiers() & Qt::AltModifier)))
			need_repaint();

        if(w->toolbox.HaveTool() == false)
		{
			setMouseTracking(false);
			w->SetStatusBarMessage();
		}
	}*/
}

void GLViewWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
	if((event->button() & Qt::LeftButton) == false)
		super::mouseDoubleClickEvent(event);
/*	else
	{
        if(w->toolbox.OnDoubleClick(GetWorldPosition(event), GetFlags(event)))
			need_repaint();

		if(w->toolbox.HaveTool() == false)
		{
			setMouseTracking(false);
			w->SetStatusBarMessage();
		}
	}*/
}

template<typename T>
inline int get_sign(T it)
{
	return it < (T)0? -1 : 1;
}

void GLViewWidget::wheelEvent(QWheelEvent * wheel)
{
    if(!w)
	{
		super::wheelEvent(wheel);
		return;
	}

	if(wheel->modifiers() & Qt::ControlModifier)
	{
		if(std::fabs(wheel->angleDelta().y()) > 0)
		{
            if(w->document == nullptr) return;

			auto pos = wheel->position();
			float angle = wheel->angleDelta().y();
			float factor = std::pow(1.0015, angle);

            factor = w->SetZoom(w->GetZoom() * factor);

		//	glm::vec2 scroll = glm::mix(scroll_destination, scroll_start, glm::vec2(factor));

			if(factor != 1)
			{
                glm::vec2 scroll_start       = w->GetScroll();
                glm::vec2 dimensions         = w->document->GetDimensions();
				glm::vec2 mouse_position     = glm::vec2(pos.x() - width()*.5f, -1 * (pos.y() - height()*.5f));
				glm::vec2 world_position     = scroll_start * dimensions + mouse_position * (1 - factor);
				glm::vec2 scroll_destination = world_position / dimensions;

                w->SetScroll(scroll_destination);
			}

			return;
		}
	}
	else if(wheel->buttons() != Qt::ControlModifier)
	{
		if(std::fabs(wheel->angleDelta().x()) > 0)
		{
            w->ui->horizontalScrollBar->event(wheel);
		}
		else
		{
            w->ui->verticalScrollBar->event(wheel);
		}

		return;
	}

	super::wheelEvent(wheel);
}


bool GLViewWidget::event(QEvent *event)
{
	if(event->type() != QEvent::ToolTip)
		return super::event(event);

   QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);

   QString string; // = window->getToolTip(helpEvent->pos());

   if(!string.isEmpty())
   {
	   QToolTip::showText(helpEvent->globalPos(), string);
   }
   else
   {
	   QToolTip::hideText();
	   event->ignore();
   }

   return true;
}

void GLViewWidget::paintGL()
{
    if(w->document == nullptr)
		return;

   // if(w->document->m_metaroom.m_selection.Changed()) w->OnSelectionChanged();

	w->SetStatusBarMessage(GetWorldPosition());

    glAssert();
	int width = size().width();
	int height = size().height();

	Matrices mat;

	mat.u_projection = glm::ortho(
		(float)-width/2,
		(float) width/2,
		(float)-height/2,
		(float) height/2,
		(float)-1,
		(float)+1);

	auto window_pos = mapToGlobal(QPoint());

    mat.u_camera = glm::scale(glm::mat4(1), glm::vec3(w->GetZoom()));
    mat.u_camera = glm::translate(mat.u_camera, glm::vec3(-w->document->GetScreenCenter(), 0));
	mat.u_screenSize = glm::ivec4(width, height, window_pos.x(), window_pos.y());

	long long time =
		std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::high_resolution_clock::now() - current_time
				).count();

	mat.u_ctime = time / 1000;
	mat.u_zoom  = w->GetZoom();

	glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrices), &mat);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_ubo);

	TransparencyShader::Shader.bind(this);
	glDefaultVAOs::BindVAO(this);
	glDefaultVAOs::RenderSquare(this);

    w->document->Render(this);

    glAssert();
}

void 	GLViewWidget::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w, h);
    glViewport(0, 0, w, h);
}

#include <GL/glu.h>
#include <QMessageBox>

void GLViewWidget::displayOpenGlError(const char * file, const char * function, int line)
{
    GLenum error = glGetError();

    if(error == GL_NO_ERROR) return;

    do
    {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("FILE: %1\nFUNC: %2\nLINE: %3\nERROR: %4")
                                 .arg(file, function, QString::number(line), (const char *) gluErrorString(error)));

    } while((error = glGetError()) != GL_NO_ERROR);

    w->close();
}
